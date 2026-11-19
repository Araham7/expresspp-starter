/* Entry point (like server.js): load config, connect services, start listening.
     ./build/server              or   nodemonpp   (rebuild + restart on every save)                  */
#include <cstdio>
#include <exception>

#include "app.hpp"
#include "config/config.hpp"

int main() {
    try {
        const auto &c = api::config::load();

        express::App app;
        api::startServices(app);
        api::buildApp(app);

        app.listen(c.port, c.host, [&c] {
            std::printf("🚀 Server (%s) on http://localhost:%d  —  API: /api/v1  health: /api/v1/health\n",
                        c.nodeEnv.c_str(), c.port);
        });  // blocks until Ctrl+C / SIGTERM

        api::stopServices();  // graceful shutdown: running jobs finish
        std::printf("👋 bye\n");
    } catch (const std::exception &e) {
        std::fprintf(stderr, "❌ could not start: %s\n", e.what());
        return 1;
    }
}
