#pragma once
/* One response shape for the whole API:
     success: {"success": true, "message": "...", "data": ...}
     errors:  {"error": "...", ...}  (thrown express::HttpError, validation, nevla errors)              */
#include <nevla/nevla.hpp>

#include <string>

namespace api::utils {

inline nevla::Value ok(nevla::Value data = nevla::Value(), const std::string &message = "") {
    nevla::Value out = nevla::obj("success", true, "data", std::move(data));
    if (!message.empty()) out["message"] = message;
    return out;
}

inline nevla::Value list(const std::vector<nevla::Value> &rows) {
    nevla::Value arr = nevla::Value::array();
    for (const auto &r : rows) arr.push_back(r);
    return arr;
}

}  // namespace api::utils
