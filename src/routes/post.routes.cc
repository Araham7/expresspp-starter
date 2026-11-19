#include "controllers/post.controller.hpp"
#include "middlewares/auth.middleware.hpp"
#include "routes/routes.hpp"
#include "validators/post.validator.hpp"

namespace api::routes {

express::Router posts() {
    namespace c = controllers::posts;
    using namespace middlewares;
    auto id = express::validate({.params = validators::idParams()});  // bad ids -> 400, not a DB error
    express::Router r;
    r.get("/", c::list);
    r.get("/:id", id, c::getOne);
    r.post("/", requireAuth(), express::validate({.body = validators::createPostBody()}), c::create);
    r.patch("/:id", requireAuth(), id, express::validate({.body = validators::updatePostBody()}), c::update);
    r.del("/:id", requireAuth(), id, c::remove);
    return r;
}

}  // namespace api::routes
