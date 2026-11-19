#include "models/post.model.hpp"

namespace api::models {

nevla::Model Post() { return nevla::model("Post"); }

void registerPost() {
    nevla::Schema schema(
        {
            {"title", nevla::String().required().trim().minLength(3).maxLength(120)},
            {"body", nevla::String().required()},
            {"tags", nevla::Array(nevla::String().trim().lowercase())},
            {"published", nevla::Boolean().def(false)},
            {"author", nevla::Id().ref("User").required().index()},
        },
        {.timestamps = true});
    schema.toJSON([](const nevla::Document &, nevla::Value &out) { out.erase("__v"); });
    nevla::model("Post", schema);
}

}  // namespace api::models
