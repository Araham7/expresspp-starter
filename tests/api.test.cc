/* End-to-end API tests: starts the real app (in-memory DB, queue and mail) on a test port and talks to it
   with axiospp, like supertest in a Node project.   Run:  ctest --test-dir build --output-on-failure  */
#include <axios/axios.hpp>

#include <cstdlib>
#include <mutex>
#include <vector>
#include <iostream>
#include <thread>

#include "app.hpp"
#include "config/config.hpp"
#include "services/mail.service.hpp"

static int gPass = 0, gFail = 0;
#define CHECK(cond, name)                                         \
    do {                                                          \
        if (cond) {                                               \
            gPass++;                                              \
            std::cout << "  ✓ " << name << "\n";                  \
        } else {                                                  \
            gFail++;                                              \
            std::cout << "  ✗ " << name << "  (" #cond ")\n";     \
        }                                                         \
    } while (0)

static const int PORT = 3999;

// a tiny fake Telegram Bot API, so the bot can be tested without the internet
static std::mutex gTgMutex;
static std::vector<express::Json> gTgSent;
static bool gTgUpdateGiven = false;
static void fakeTelegram(express::Req req, express::Res res) {
    std::string method = req.param("method");
    express::Json ok = express::obj("ok", true, "result", true);
    if (method == "getUpdates") {
        express::Json list(Json::arrayValue);
        std::lock_guard<std::mutex> lk(gTgMutex);
        if (!gTgUpdateGiven) {
            gTgUpdateGiven = true;
            list.append(express::obj("update_id", 1, "message",
                                     express::obj("message_id", 1, "text", "/stats", "chat", express::obj("id", 5),
                                                  "from", express::obj("id", 5, "first_name", "Asha"))));
        }
        ok["result"] = list;
    } else if (method == "sendMessage") {
        std::lock_guard<std::mutex> lk(gTgMutex);
        gTgSent.push_back(req.json());
        ok["result"] = express::obj("message_id", 9);
    }
    res.json(ok);
}
static const std::string BASE = "http://127.0.0.1:" + std::to_string(PORT) + "/api/v1";

// status code of a request that may fail (axios throws for 4xx/5xx)
template <class F>
static int statusOf(F call) {
    try {
        return call().status;
    } catch (const axios::Error &e) {
        return e.status();
    }
}

static void run() {
    auto api = axios::create({.baseURL = BASE, .timeout = 10000});

    CHECK(api.get("/health").get().data["status"].asString() == "ok", "GET /health");

    auto reg = api.post("/auth/register", axios::obj("name", "Asha", "email", "Asha@Example.com", "password", "longpassword")).get();
    CHECK(reg.status == 201 && reg.data["data"]["user"]["email"].asString() == "asha@example.com", "register (email lowercased)");
    CHECK(!reg.data["data"]["user"].isMember("password"), "password never returned");
    CHECK(reg.header("set-cookie").find("token=s%3A") != std::string::npos, "signed httpOnly token cookie");
    CHECK(statusOf([&] { return api.post("/auth/register", axios::obj("name", "Asha", "email", "asha@example.com", "password", "longpassword")).get(); }) == 409,
          "duplicate email -> 409");
    CHECK(statusOf([&] { return api.post("/auth/register", axios::obj("email", "x")).get(); }) == 400, "invalid body -> 400");

    CHECK(statusOf([&] { return api.post("/auth/login", axios::obj("email", "asha@example.com", "password", "wrong-one")).get(); }) == 401,
          "wrong password -> 401");
    auto login = api.post("/auth/login", axios::obj("email", "asha@example.com", "password", "longpassword")).get();
    std::string token = login.data["data"]["token"].asString();
    CHECK(login.status == 200 && !token.empty(), "login returns a JWT");

    auto me = axios::create({.baseURL = BASE, .headers = {{"Authorization", "Bearer " + token}}, .timeout = 10000});
    CHECK(statusOf([&] { return api.get("/users/me").get(); }) == 401, "no token -> 401");
    CHECK(me.get("/users/me").get().data["data"]["name"].asString() == "Asha", "GET /users/me");
    CHECK(me.patch("/users/me", axios::obj("name", "Asha Rani")).get().data["data"]["name"].asString() == "Asha Rani",
          "PATCH /users/me");
    CHECK(statusOf([&] { return me.get("/users").get(); }) == 403, "admin-only route -> 403");

    auto post = me.post("/posts", axios::obj("title", "Hello Express++", "body", "C++ backend", "tags", axios::arr("CPP"),
                                             "published", true)).get();
    std::string id = post.data["data"]["_id"].asString();
    CHECK(post.status == 201 && post.data["data"]["tags"][0].asString() == "cpp", "create post");
    me.post("/posts", axios::obj("title", "Secret draft", "body", "not yet")).get();
    auto list = api.get("/posts", {.params = {{"tags", "cpp"}}}).get();
    CHECK(list.data["data"]["total"].asInt() == 1, "public list shows only published posts");
    CHECK(api.get("/posts/" + id).get().data["data"]["author"]["name"].asString() == "Asha Rani", "get post (author populated)");
    CHECK(statusOf([&] { return api.get("/posts/not-an-id").get(); }) == 400, "bad id -> 400");
    CHECK(statusOf([&] { return api.get("/posts/0123456789abcdef01234567").get(); }) == 404, "missing post -> 404");
    CHECK(me.patch("/posts/" + id, axios::obj("title", "Hello again")).get().data["data"]["title"].asString() == "Hello again",
          "update own post");

    auto other = api.post("/auth/register", axios::obj("name", "Ravi", "email", "ravi@example.com", "password", "longpassword")).get();
    auto ravi = axios::create({.baseURL = BASE,
                               .headers = {{"Authorization", "Bearer " + other.data["data"]["token"].asString()}},
                               .timeout = 10000});
    CHECK(statusOf([&] { return ravi.del("/posts/" + id).get(); }) == 403, "someone else's post -> 403");
    CHECK(me.del("/posts/" + id).get().status == 204, "delete own post");

    CHECK(statusOf([&] { return api.get("/nope").get(); }) == 404, "unknown route -> 404 JSON");

    for (int i = 0; i < 200 && api::services::mail::sentCount() < 2; i++) std::this_thread::sleep_for(std::chrono::milliseconds(10));
    CHECK(api::services::mail::sentCount() == 2, "welcome mails sent by the queue worker");

    // Telegram bot (talking to the fake Telegram API below)
    auto sentTo = [](const std::string &chat, const std::string &needle) {
        std::lock_guard<std::mutex> lk(gTgMutex);
        for (auto &m : gTgSent)
            if (m["chat_id"].asString() == chat && m["text"].asString().find(needle) != std::string::npos) return true;
        return false;
    };
    bool adminNotified = false;
    for (int i = 0; i < 300 && !adminNotified; i++, std::this_thread::sleep_for(std::chrono::milliseconds(10)))
        adminNotified = sentTo("77", "New user: Ravi");
    CHECK(adminNotified, "telegram: admin notified about a new user");
    bool statsAnswered = false;
    for (int i = 0; i < 500 && !statsAnswered; i++, std::this_thread::sleep_for(std::chrono::milliseconds(10)))
        statsAnswered = sentTo("5", "users: ");
    CHECK(statsAnswered, "telegram: /stats command answered from the database (polling)");
}

int main() {
    // test settings (no .env file needed)
    setenv("NODE_ENV", "test", 1);
    setenv("PORT", std::to_string(PORT).c_str(), 1);
    setenv("MONGODB_URI", "memory://api_tests", 1);
    setenv("REDIS_HOST", "", 1);
    setenv("SMTP_HOST", "", 1);
    setenv("TELEGRAM_TOKEN", "1:TEST", 1);
    setenv("TELEGRAM_ADMIN_CHAT_ID", "77", 1);
    setenv("TELEGRAM_API_URL", ("http://127.0.0.1:" + std::to_string(PORT) + "/fake-telegram").c_str(), 1);
    api::config::load("tests/does-not-exist.env");

    express::App app;
    app.post("/fake-telegram/:bt/:method", fakeTelegram);
    api::startServices(app);
    api::buildApp(app);

    std::thread([] {
        while (!drogon::app().isRunning()) std::this_thread::sleep_for(std::chrono::milliseconds(20));
        try {
            run();
        } catch (const std::exception &e) {
            gFail++;
            std::cout << "  ✗ unexpected exception: " << e.what() << "\n";
        }
        express::App::stop();
    }).detach();
    app.listen(PORT, "127.0.0.1");
    api::stopServices();
    std::cout << gPass << " passed, " << gFail << " failed\n";
    return gFail == 0 ? 0 : 1;
}
