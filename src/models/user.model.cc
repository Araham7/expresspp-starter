#include "models/user.model.hpp"

namespace api::models {

nevla::Model User() { return nevla::model("User"); }

void registerUser() {
    nevla::Schema schema(
        {
            {"name", nevla::String().required().trim().minLength(2).maxLength(60)},
            {"email", nevla::String().required().trim().lowercase().unique().match(R"(^\S+@\S+\.\S+$)", "invalid email")},
            {"password", nevla::String().required().select(false)},  // a hash, never returned by find()
            {"role", nevla::String().enumOf({"user", "admin"}).def("user")},
            {"avatar", nevla::String().def("")},
        },
        {.timestamps = true});
    schema.toJSON([](const nevla::Document &, nevla::Value &out) {  // what the API sends back
        out.erase("password");
        out.erase("__v");
    });
    nevla::model("User", schema);
}

}  // namespace api::models
