#include "utils/token.hpp"

#include "config/config.hpp"

namespace api::utils {

std::string signToken(const std::string &userId, const std::string &role) {
    const auto &c = config::get();
    return express::jwt::sign(express::obj("sub", userId, "role", role), c.jwtSecret,
                              {.expiresIn = c.jwtExpiresIn, .issuer = "my-server"});
}

}  // namespace api::utils
