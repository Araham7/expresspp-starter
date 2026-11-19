#include "app.hpp"

#include "config/config.hpp"
#include "config/db.hpp"
#include "middlewares/error.middleware.hpp"
#include "routes/routes.hpp"
#include "services/mail.service.hpp"
#include "services/queue.service.hpp"
#include "services/socket.service.hpp"
#include "services/telegram.service.hpp"

namespace api {

void startServices(express::App &app) {
    db::connect(app);
    services::mail::init();
    services::queue::init();
    services::socket::init();  // before app.listen()
    services::telegram::init();
}

void stopServices() {
    services::telegram::shutdown();
    services::queue::shutdown();
}

void buildApp(express::App &app) {
    const auto &c = config::get();

    // settings
    app.disable("x-powered-by");
    app.set("trust proxy", c.behindProxy);
    app.set("body limit", "5mb");

    // global middleware (order matters, like in Express)
    app.use(express::helmet());                                        // security headers
    app.use(express::logger(c.isProduction() ? "combined" : "dev"));  // morgan
    app.use(express::cors({.origins = {c.clientUrl}, .credentials = c.clientUrl != "*"}));
    app.use(express::json());                                          // body-parser
    app.use(express::cookieParser(c.cookieSecret));                    // req.cookies(), signed cookies

    // static files: public/ at /, uploaded files at /uploads
    app.use(express::serveStatic("public"));
    app.use("/uploads", express::serveStatic("uploads"));

    // API (rate limited)
    app.use("/api", express::rateLimit({.windowMs = 15 * 60 * 1000, .limit = c.rateLimit}));
    app.use("/api/v1", routes::api());

    // Telegram webhook (only when TELEGRAM_WEBHOOK_URL is set)
    services::telegram::mountWebhook(app);

    // must be last
    app.use(middlewares::notFound);
    app.use(middlewares::errorHandler);
}

}  // namespace api
