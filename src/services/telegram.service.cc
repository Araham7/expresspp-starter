#include "services/telegram.service.hpp"

#include <telegram/express.hpp>

#include <memory>

#include "bots/telegram.bot.hpp"
#include "config/config.hpp"

namespace api::services::telegram {

namespace {
std::unique_ptr<::telegram::TelegramBot> gBot;
}

void init() {
    const auto &c = config::get();
    if (c.telegramToken.empty()) {
        std::printf("🤖 Telegram: off (no TELEGRAM_TOKEN)\n");
        return;
    }
    bool polling = c.telegramWebhookUrl.empty();
    gBot = std::make_unique<::telegram::TelegramBot>(c.telegramToken, ::telegram::Options{.polling = polling, .baseApiUrl = c.telegramApiUrl});
    bots::registerTelegramBot(*gBot);
    gBot->on("polling_error", [](const ::telegram::Error &e) { std::fprintf(stderr, "🤖 polling error: %s\n", e.what()); });

    if (polling) {
        gBot->deleteWebHook();  // a webhook left from production would block polling (409)
        std::printf("🤖 Telegram: polling\n");
    } else {
        gBot->setWebHook(c.telegramWebhookUrl, ::telegram::obj("secret_token", c.telegramSecret))
            .then([](::telegram::Json) { std::printf("🤖 Telegram: webhook set\n"); },
                  [](const ::telegram::Error &e) { std::fprintf(stderr, "🤖 setWebHook failed: %s\n", e.what()); });
    }
}

void mountWebhook(express::App &app) {
    const auto &c = config::get();
    if (!gBot || c.telegramWebhookUrl.empty()) return;
    app.post("/api/v1/telegram/webhook", ::telegram::webhook(*gBot, c.telegramSecret));
}

void shutdown() { gBot.reset(); }  // stops polling and the handler thread

bool enabled() { return gBot != nullptr; }

::telegram::TelegramBot &bot() {
    if (!gBot) throw std::logic_error("Telegram bot is off (set TELEGRAM_TOKEN)");
    return *gBot;
}

void notifyAdmin(const std::string &text) {
    const auto &c = config::get();
    if (gBot && !c.telegramAdminChat.empty()) gBot->sendMessage(c.telegramAdminChat, text);  // fire and forget
}

}  // namespace api::services::telegram
