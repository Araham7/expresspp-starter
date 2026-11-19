#include "middlewares/error.middleware.hpp"

#include <nevla/nevla.hpp>

#include "config/config.hpp"

namespace api::middlewares {

void notFound(express::Req req, express::Res res) {
    res.status(404).json(express::obj("error", "Route not found: " + req.method() + " " + req.originalUrl()));
}

void errorHandler(express::Error err, express::Req req, express::Res res, express::Next next) {
    // client mistakes: let Express++ answer (status, message, extra fields, headers).
    // nevla errors too: express::nv maps them to 400 / 404 / 409.
    if (err.status() < 500 || err.is<nevla::Error>()) {
        next(err.ptr());
        return;
    }
    std::fprintf(stderr, "💥 %s %s: %s\n", req.method().c_str(), req.originalUrl().c_str(), err.message().c_str());
    bool prod = config::get().isProduction();
    res.status(err.status()).json(express::obj("error", prod ? std::string("Internal Server Error") : err.message()));
}

}  // namespace api::middlewares
