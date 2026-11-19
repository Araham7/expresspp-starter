#pragma once
// /api/v1/users: me, update me, avatar, list (admin)
#include <express/express.hpp>

namespace api::controllers::users {

express::Task<> getMe(express::Req req, express::Res res);
express::Task<> updateMe(express::Req req, express::Res res);
express::Task<> uploadAvatar(express::Req req, express::Res res);
express::Task<> list(express::Req req, express::Res res);  // ?role=admin&sort=-createdAt&page=2&limit=20

}  // namespace api::controllers::users
