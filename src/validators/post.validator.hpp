#pragma once
#include <express/express.hpp>

namespace api::validators {
namespace v = express::v;

inline v::Schema createPostBody() {
    return v::object({
        {"title", v::string().trim().min(3).max(120).required()},
        {"body", v::string().min(1).required()},
        {"tags", v::array(v::string().trim().lowercase().max(30)).max(10)},
        {"published", v::boolean().defaultTo(false)},
    });
}

inline v::Schema updatePostBody() {  // same fields, all optional
    return v::object({
        {"title", v::string().trim().min(3).max(120)},
        {"body", v::string().min(1)},
        {"tags", v::array(v::string().trim().lowercase().max(30)).max(10)},
        {"published", v::boolean()},
    });
}

inline v::Schema idParams() {
    return v::object({{"id", v::string().pattern(std::regex("^[0-9a-f]{24}$"), "ObjectId").required()}});
}

}  // namespace api::validators
