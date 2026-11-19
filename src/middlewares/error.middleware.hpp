#pragma once
/* notFound: last middleware, 404 JSON for unknown routes.
   errorHandler: logs server errors and hides their details in production. 4xx errors
   (HttpError, validation, nevla) keep Express++'s normal JSON body.                                 */
#include <express/express.hpp>

namespace api::middlewares {

void notFound(express::Req req, express::Res res);
void errorHandler(express::Error err, express::Req req, express::Res res, express::Next next);

}  // namespace api::middlewares
