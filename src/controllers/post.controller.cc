#include "controllers/post.controller.hpp"

#include "models/post.model.hpp"
#include "services/socket.service.hpp"
#include "utils/api_response.hpp"
#include "utils/convert.hpp"
#include "utils/token.hpp"

namespace api::controllers::posts {

using models::Post;

static nevla::Document mustFind(std::optional<nevla::Document> doc) {
    if (!doc) throw express::HttpError(404, "Post not found");
    return std::move(*doc);
}

static void mustOwn(const express::Req &req, const nevla::Document &post) {
    if (post["author"].str() != utils::userId(req) && utils::userRole(req) != "admin")
        throw express::HttpError(403, "Only the author can change this post");
}

express::Task<> list(express::Req req, express::Res res) {
    std::map<std::string, std::string> params(req.queries().begin(), req.queries().end());
    params["published"] = "true";  // the public list never shows drafts
    auto page = co_await nevla::asyncCall([params] {
        return Post().fromQuery(params, {.allow = {"tags", "author", "published", "title", "createdAt"},
                                         .defaultLimit = 10, .maxLimit = 50, .defaultSort = "-createdAt"});
    });
    res.json(utils::ok(page.toJSON()));
}

express::Task<> getOne(express::Req req, express::Res res) {
    nevla::PopulateOptions author{.path = "author", .select = "name avatar"};  // no { } inside co_await (GCC 13)
    auto post = mustFind(co_await Post().findById(req.param("id")).populate(author).execAsync());
    res.json(utils::ok(post.toJSON()));
}

express::Task<> create(express::Req req, express::Res res) {
    nevla::Value data = utils::toValue(express::validated(req));
    data["author"] = nevla::ObjectId(utils::userId(req));
    auto post = co_await Post().createAsync(data);
    nevla::Value json = post.toJSON();
    if (post.get<bool>("published"))  // live feed for everybody who is connected
        services::socket::broadcast("post:created", utils::toJson(json));
    res.status(201).json(utils::ok(json, "Post created"));
}

express::Task<> update(express::Req req, express::Res res) {
    auto post = mustFind(co_await Post().findById(req.param("id")).execAsync());
    mustOwn(req, post);
    post.set(utils::toValue(express::validated(req)));
    co_await post.saveAsync();
    res.json(utils::ok(post.toJSON(), "Post updated"));
}

express::Task<> remove(express::Req req, express::Res res) {
    auto post = mustFind(co_await Post().findById(req.param("id")).execAsync());
    mustOwn(req, post);
    nevla::Value filter = nevla::obj("_id", post._id());
    co_await Post().deleteOneAsync(filter);
    res.status(204).end();
}

}  // namespace api::controllers::posts
