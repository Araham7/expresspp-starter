#pragma once
// Request schemas (like joi / zod files). Used as express::validate({.body = ...}) in the routes.
#include <express/express.hpp>

namespace api::validators {
namespace v = express::v;

inline v::Schema registerBody() {
    return v::object({
        {"name", v::string().trim().min(2).max(60).required()},
        {"email", v::string().trim().lowercase().email().required()},
        {"password", v::string().min(8).max(72).required()},
    });
}

inline v::Schema loginBody() {
    return v::object({
        {"email", v::string().trim().lowercase().email().required()},
        {"password", v::string().min(1).max(72).required()},
    });
}

inline v::Schema updateMeBody() {
    return v::object({{"name", v::string().trim().min(2).max(60)}});
}

}  // namespace api::validators
