#include <chrono>

#include "routes/routes.hpp"
#include "services/mail.service.hpp"
#include "services/socket.service.hpp"
#include "services/telegram.service.hpp"

namespace api::routes {

static const auto gStarted = std::chrono::steady_clock::now();

express::Router api() {
    express::Router r;
    r.get("/health", [](express::Req, express::Res res) {
        auto up = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - gStarted).count();
        res.json(express::obj("status", "ok", "uptime", static_cast<int64_t>(up), "sockets",
                              static_cast<int64_t>(services::socket::io().size()), "mailsSent", services::mail::sentCount(), "telegram",
                              services::telegram::enabled()));
    });
    r.use("/auth", auth());
    r.use("/users", users());
    r.use("/posts", posts());
    return r;
}

}  // namespace api::routes
