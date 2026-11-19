#pragma once
// Telegram bot commands (like a routes file, but for the bot)
#include <telegram/telegram.hpp>

namespace api::bots {
void registerTelegramBot(telegram::TelegramBot &bot);
}
