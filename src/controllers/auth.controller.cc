#include "controllers/auth.controller.hpp"

#include "config/config.hpp"
#include "models/user.model.hpp"
#include "services/queue.service.hpp"
#include "services/telegram.service.hpp"
#include "utils/api_response.hpp"
#include "utils/token.hpp"

namespace api::controllers::auth {

using models::User;

// token in the JSON (mobile apps, Postman) AND in a signed httpOnly cookie (browsers)
static void setAuthCookie(express::Res &res, const std::string &token) {
    res.cookie("token", token,
               {.httpOnly = true, .secure = config::get().isProduction(), .maxAge = 7 * 24 * 3600, .sameSite = "Lax",
                .sign = true});
}

express::Task<> registerUser(express::Req req, express::Res res) {
    express::Json body = express::validated(req);  // a copy: safe across co_await
    std::string name = body["name"].asString(), email = body["email"].asString();

    auto existing = co_await User().findOne(nevla::obj("email", email)).leanAsync();
    if (existing) throw express::HttpError(409, "Email already registered");

    std::string hash = co_await express::hash::makeAsync(body["password"].asString());  // argon2id, off the IO loop
    nevla::Value data = nevla::obj("name", name, "email", email, "password", hash);
    auto user = co_await User().createAsync(data);

    // don't make the user wait for the SMTP server: the worker sends it
    services::queue::emails().add("welcome", bullmq::obj("to", email, "name", name, "subject", "Welcome to My Server"),
                                  {.attempts = 3, .backoff = {.type = "exponential", .delayMs = 2000}});

    services::telegram::notifyAdmin("🆕 New user: " + name + " <" + email + ">");

    std::string token = utils::signToken(user.id(), user.get<std::string>("role"));
    setAuthCookie(res, token);
    res.status(201).json(utils::ok(nevla::obj("user", user.toJSON(), "token", token), "Account created"));
}

express::Task<> login(express::Req req, express::Res res) {
    express::Json body = express::validated(req);  // a copy: safe across co_await
    auto user = co_await User().findOne(nevla::obj("email", body["email"].asString())).select("+password").leanAsync();

    // same message for "no such user" and "wrong password": don't tell attackers which emails exist
    bool ok = user && co_await express::hash::verifyAsync(body["password"].asString(), (*user)["password"].asString());
    if (!ok) throw express::HttpError(401, "Wrong email or password");

    nevla::Value safe = *user;
    safe.erase("password");
    safe.erase("__v");
    std::string token = utils::signToken(safe["_id"].str(), safe["role"].asString());
    setAuthCookie(res, token);
    res.json(utils::ok(nevla::obj("user", safe, "token", token), "Logged in"));
}

void logout(express::Req, express::Res res) {
    res.clearCookie("token");
    res.json(utils::ok(nevla::Value(), "Logged out"));
}

}  // namespace api::controllers::auth
