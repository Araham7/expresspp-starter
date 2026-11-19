#pragma once
// express::Json (jsoncpp, what Express++ / validate() use) <-> nevla::Value (what nevla uses)
#include <express/express.hpp>
#include <nevla/nevla.hpp>

namespace api::utils {

inline nevla::Value toValue(const express::Json &j) { return nevla::Value::parse(express::stringify(j)); }
inline express::Json toJson(const nevla::Value &v) { return express::parseJson(v.toJson()); }

}  // namespace api::utils
