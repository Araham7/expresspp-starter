#pragma once
// POST /api/v1/auth/register | /login | /logout
#include <express/express.hpp>

namespace api::controllers::auth {

express::Task<> registerUser(express::Req req, express::Res res);
express::Task<> login(express::Req req, express::Res res);
void logout(express::Req req, express::Res res);

}  // namespace api::controllers::auth
