#pragma once
// JWT helpers: one place that knows what is inside our tokens.
#include <express/express.hpp>

#include <string>

namespace api::utils {

std::string signToken(const std::string &userId, const std::string &role);

// the logged-in user (after requireAuth): {"sub": "<user id>", "role": "user" | "admin", ...}
inline std::string userId(const express::Req &req) { return express::jwt::user(req)["sub"].asString(); }
inline std::string userRole(const express::Req &req) { return express::jwt::user(req)["role"].asString(); }

}  // namespace api::utils
