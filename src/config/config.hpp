#pragma once
// All settings in one typed place (like config/index.js). Read from .env / the environment once.
#include <string>

namespace api::config {

struct Config {
    std::string nodeEnv = "development";
    int port = 3000;
    std::string host = "0.0.0.0";
    std::string clientUrl = "*";
    bool behindProxy = false;

    std::string mongoUri = "memory://my_server";
    std::string mongoDb = "my_server";

    std::string jwtSecret;
    std::string jwtExpiresIn = "7d";
    std::string cookieSecret;

    std::string redisHost;  // empty = in-memory queue
    int redisPort = 6379;
    std::string redisPassword;

    std::string smtpHost;   // empty = log mails instead of sending
    int smtpPort = 587;
    bool smtpSecure = false;
    std::string smtpUser, smtpPass;
    std::string mailFrom = "My Server <no-reply@example.com>";

    int rateLimit = 300;

    std::string telegramToken;        // empty = no Telegram bot
    std::string telegramWebhookUrl;   // empty = polling (dev); set = webhook (production)
    std::string telegramSecret;       // checked on every webhook request
    std::string telegramAdminChat;    // chat id that gets admin notifications (new users)
    std::string telegramApiUrl = "https://api.telegram.org";  // a local Bot API server (or a fake one in tests)

    bool isProduction() const { return nodeEnv == "production"; }
};

const Config &load(const std::string &envFile = ".env");  // call once at startup
const Config &get();                                       // after load()

}  // namespace api::config
