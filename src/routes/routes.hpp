#pragma once
// Every router of the API. routes/index.cc mounts them under /api/v1.
#include <express/express.hpp>

namespace api::routes {

express::Router api();  // /api/v1
express::Router auth();
express::Router users();
express::Router posts();

}  // namespace api::routes
