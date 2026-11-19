#pragma once
// /api/v1/posts: CRUD, only the author (or an admin) may change a post
#include <express/express.hpp>

namespace api::controllers::posts {

express::Task<> list(express::Req req, express::Res res);  // ?tags=cpp&sort=-createdAt&page=1&limit=10
express::Task<> getOne(express::Req req, express::Res res);
express::Task<> create(express::Req req, express::Res res);
express::Task<> update(express::Req req, express::Res res);
express::Task<> remove(express::Req req, express::Res res);

}  // namespace api::controllers::posts
