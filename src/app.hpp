#pragma once
// Builds the Express++ app (like app.js): global middleware, static files, routes, errors.
// main.cc starts it; tests start the very same app.
#include <express/express.hpp>

namespace api {

void buildApp(express::App &app);  // config::load() must have run
void startServices(express::App &app);  // DB, mail, queue + workers, socket.io
void stopServices();

}  // namespace api
