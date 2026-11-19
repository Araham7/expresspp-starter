#pragma once
/* requireAuth(): token from "Authorization: Bearer <jwt>" or the signed httpOnly "token" cookie.
   requireRole("admin"): after requireAuth(), 403 for everybody else.                               */
#include <express/express.hpp>

#include <string>

namespace api::middlewares {

express::Middleware requireAuth();
express::Middleware optionalAuth();  // user if there is a valid token, otherwise just continue
express::Middleware requireRole(std::string role);

}  // namespace api::middlewares
