#include "config/config.hpp"

#include <express/env.hpp>

#include <stdexcept>

namespace api::config {

namespace {
Config gConfig;
bool gLoaded = false;
}  // namespace

const Config &load(const std::string &envFile) {
    namespace env = express::env;
    env::load(envFile);  // a missing .env is fine: real environment variables still work

    Config c;
    c.nodeEnv = env::get("NODE_ENV", "development");
    c.port = static_cast<int>(env::getInt("PORT", 3000));
    c.host = env::get("HOST", "0.0.0.0");
    c.clientUrl = env::get("CLIENT_URL", "*");
    c.behindProxy = env::getBool("BEHIND_PROXY", false);
    c.mongoUri = env::get("MONGODB_URI", "memory://my_server");
    c.mongoDb = env::get("MONGODB_DB", "my_server");
    c.jwtSecret = env::get("JWT_SECRET", "dev-only-jwt-secret");
    c.jwtExpiresIn = env::get("JWT_EXPIRES_IN", "7d");
    c.cookieSecret = env::get("COOKIE_SECRET", "dev-only-cookie-secret");
    c.redisHost = env::get("REDIS_HOST");
    c.redisPort = static_cast<int>(env::getInt("REDIS_PORT", 6379));
    c.redisPassword = env::get("REDIS_PASSWORD");
    c.smtpHost = env::get("SMTP_HOST");
    c.smtpPort = static_cast<int>(env::getInt("SMTP_PORT", 587));
    c.smtpSecure = env::getBool("SMTP_SECURE", false);
    c.smtpUser = env::get("SMTP_USER");
    c.smtpPass = env::get("SMTP_PASS");
    c.mailFrom = env::get("MAIL_FROM", c.mailFrom);
    c.rateLimit = static_cast<int>(env::getInt("RATE_LIMIT", 300));
    c.telegramToken = env::get("TELEGRAM_TOKEN");
    c.telegramWebhookUrl = env::get("TELEGRAM_WEBHOOK_URL");
    c.telegramSecret = env::get("TELEGRAM_WEBHOOK_SECRET", "dev-only-telegram-secret");
    c.telegramAdminChat = env::get("TELEGRAM_ADMIN_CHAT_ID");
    c.telegramApiUrl = env::get("TELEGRAM_API_URL", c.telegramApiUrl);

    // fail fast: never run production with the development secrets
    if (c.isProduction()) {
        for (const char *key : {"JWT_SECRET", "COOKIE_SECRET"}) {
            std::string v = env::get(key);
            if (v.empty() || v.rfind("change-me", 0) == 0 || v.size() < 32)
                throw std::runtime_error(std::string(key) + " must be set to a long random value in production");
        }
    }
    if (c.isProduction() && !c.telegramWebhookUrl.empty()) {
        std::string v = env::get("TELEGRAM_WEBHOOK_SECRET");
        if (v.size() < 16 || v.rfind("change-me", 0) == 0)
            throw std::runtime_error("TELEGRAM_WEBHOOK_SECRET must be set to a long random value in production");
    }
    gConfig = c;
    gLoaded = true;
    return gConfig;
}

const Config &get() {
    if (!gLoaded) throw std::logic_error("config::get() before config::load()");
    return gConfig;
}

}  // namespace api::config
