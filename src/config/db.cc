#include "config/db.hpp"

#include <express/nevla.hpp>

#include "config/config.hpp"
#include "models/index.hpp"

namespace api::db {

void connect(express::App &app) {
    const auto &c = config::get();
    models::registerAll();                       // schemas first
    express::nv::use(app, c.mongoUri, c.mongoDb);  // connect + map nevla errors to 400/404/409
    std::printf("🍃 MongoDB: %s (db %s)\n", c.mongoUri.rfind("memory://", 0) == 0 ? "in-memory" : "connected",
                c.mongoDb.c_str());
}

}  // namespace api::db
