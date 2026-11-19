#include "controllers/auth.controller.hpp"
#include "routes/routes.hpp"
#include "validators/auth.validator.hpp"

namespace api::routes {

express::Router auth() {
    namespace c = controllers::auth;
    express::Router r;
    // stricter limit for login/register: 10 tries per 15 minutes per IP
    auto bruteForce = express::rateLimit({.windowMs = 15 * 60 * 1000, .limit = 10,
                                          .message = "Too many attempts, try again in 15 minutes",
                                          .skipSuccessfulRequests = true});
    r.post("/register", bruteForce, express::validate({.body = validators::registerBody()}), c::registerUser);
    r.post("/login", bruteForce, express::validate({.body = validators::loginBody()}), c::login);
    r.post("/logout", c::logout);
    return r;
}

}  // namespace api::routes
