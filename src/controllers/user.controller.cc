#include "controllers/user.controller.hpp"

#include "models/user.model.hpp"
#include "utils/api_response.hpp"
#include "utils/convert.hpp"
#include "utils/token.hpp"

namespace api::controllers::users {

using models::User;

static nevla::Document mustFind(std::optional<nevla::Document> doc) {
    if (!doc) throw express::HttpError(404, "User not found");
    return std::move(*doc);
}

// $set only these fields (validators run on them). Not doc.save(): the loaded user has no password
// (select(false)), and save() would validate the whole document.
static nevla::Async<std::optional<nevla::Document>> updateUser(std::string id, nevla::Value fields) {
    return nevla::asyncCall([id, fields] {
        return User().findByIdAndUpdate(id, nevla::obj("$set", fields), {.returnNew = true, .runValidators = true});
    });
}

express::Task<> getMe(express::Req req, express::Res res) {
    auto user = mustFind(co_await User().findById(utils::userId(req)).execAsync());
    res.json(utils::ok(user.toJSON()));
}

express::Task<> updateMe(express::Req req, express::Res res) {
    nevla::Value fields = utils::toValue(express::validated(req));  // only the validated fields
    auto user = mustFind(co_await updateUser(utils::userId(req), fields));
    res.json(utils::ok(user.toJSON(), "Profile updated"));
}

express::Task<> uploadAvatar(express::Req req, express::Res res) {
    auto *file = express::file(req, "avatar");
    if (!file) throw express::HttpError(400, "Send an image in the 'avatar' field");
    nevla::Value fields = nevla::obj("avatar", "/" + file->path);  // served by serveStatic("uploads") at /uploads/...
    auto user = mustFind(co_await updateUser(utils::userId(req), fields));
    res.status(201).json(utils::ok(nevla::obj("avatar", user.get<std::string>("avatar")), "Avatar saved"));
}

express::Task<> list(express::Req req, express::Res res) {
    std::map<std::string, std::string> params(req.queries().begin(), req.queries().end());
    auto page = co_await nevla::asyncCall([params] {
        return User().fromQuery(params, {.allow = {"role", "name", "email", "createdAt"}, .defaultSort = "-createdAt"});
    });
    res.json(utils::ok(page.toJSON()));
}

}  // namespace api::controllers::users
