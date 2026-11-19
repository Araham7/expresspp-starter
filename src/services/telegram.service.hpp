#pragma once
/* Telegram bot (cpp-telegram-bot-api). Off when TELEGRAM_TOKEN is empty.
   Development: polling. Production: set TELEGRAM_WEBHOOK_URL (https://your-domain/api/v1/telegram/webhook).
     services::telegram::notifyAdmin("🆕 New user: asha@example.com");
   Bot commands live in src/bots/telegram.bot.cc                                                       */
#include <express/express.hpp>
#include <telegram/telegram.hpp>

#include <string>

namespace api::services::telegram {

void init();
void mountWebhook(express::App &app);  // in buildApp(), before the 404 handler
void shutdown();
bool enabled();
::telegram::TelegramBot &bot();          // only when enabled()
void notifyAdmin(const std::string &text);  // no-op without TELEGRAM_ADMIN_CHAT_ID

}  // namespace api::services::telegram
