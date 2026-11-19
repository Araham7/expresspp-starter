#include "middlewares/auth.middleware.hpp"

#include "config/config.hpp"

namespace api::middlewares {

static std::string findToken(const express::Req &req) {
    const std::string &h = req.header("authorization");
    if (h.rfind("Bearer ", 0) == 0) return h.substr(7);
    return req.signedCookie("token").value_or("");  // set by auth.controller login()
}

static express::Middleware jwtAuth(bool required) {
    return express::auth({.secret = config::get().jwtSecret,
                          .credentialsRequired = required,
                          .issuer = "my-server",
                          .getToken = findToken});
}

express::Middleware requireAuth() { return jwtAuth(true); }
express::Middleware optionalAuth() { return jwtAuth(false); }

express::Middleware requireRole(std::string role) {
    return [role](express::Req req, express::Res, express::Next next) {
        if (express::jwt::user(req)["role"].asString() != role)
            throw express::HttpError(403, "You need the '" + role + "' role for this");
        next();
    };
}

}  // namespace api::middlewares
