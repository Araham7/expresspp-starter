#include "controllers/user.controller.hpp"
#include "middlewares/auth.middleware.hpp"
#include "routes/routes.hpp"
#include "validators/auth.validator.hpp"

namespace api::routes {

express::Router users() {
    namespace c = controllers::users;
    using namespace middlewares;
    express::Router r;
    r.use(requireAuth());  // everything below needs a login

    auto avatar = express::upload({.dest = "uploads/avatars",
                                   .limits = {.fileSize = "2mb", .files = 1},
                                   .fileFilter = [](const express::UploadedFile &f) { return f.mimetype.rfind("image/", 0) == 0; }});

    r.get("/me", c::getMe);
    r.patch("/me", express::validate({.body = validators::updateMeBody()}), c::updateMe);
    r.post("/me/avatar", avatar.single("avatar"), c::uploadAvatar);
    r.get("/", requireRole("admin"), c::list);
    return r;
}

}  // namespace api::routes
