#pragma once
// MongoDB connection (like config/db.js): connects nevla and registers every model.
#include <express/express.hpp>

namespace api::db {
void connect(express::App &app);
}
