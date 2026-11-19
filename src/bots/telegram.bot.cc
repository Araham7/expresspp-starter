#include "bots/telegram.bot.hpp"

#include "models/post.model.hpp"
#include "models/user.model.hpp"

namespace api::bots {

void registerTelegramBot(telegram::TelegramBot &bot) {
    bot.setMyCommands(telegram::arr(telegram::obj("command", "start", "description", "Start"),
                                    telegram::obj("command", "posts", "description", "Latest posts"),
                                    telegram::obj("command", "stats", "description", "Users and posts count")));

    bot.onText(std::regex(R"(^/start)"), [&bot](const telegram::Message &msg, const std::smatch &) {
        bot.sendMessage(msg["chat"]["id"], "Namaste " + msg["from"]["first_name"].asString() +
                                               "! 👋\nTry /posts or /stats.\nYour chat id: " + msg["chat"]["id"].asString());
    });

    // handlers run on the bot's own thread, so blocking database calls are fine here
    bot.onText(std::regex(R"(^/posts)"), [&bot](const telegram::Message &msg, const std::smatch &) {
        auto posts = models::Post().find(nevla::obj("published", true)).sort("-createdAt").limit(5).lean();
        if (posts.empty()) {
            bot.sendMessage(msg["chat"]["id"], "No posts yet.");
            return;
        }
        std::string text = "📝 <b>Latest posts</b>\n";
        for (const auto &p : posts) text += "• " + p["title"].asString() + "\n";
        bot.sendMessage(msg["chat"]["id"], text, telegram::obj("parse_mode", "HTML"));
    });

    bot.onText(std::regex(R"(^/stats)"), [&bot](const telegram::Message &msg, const std::smatch &) {
        auto users = models::User().countDocuments();
        auto posts = models::Post().countDocuments();
        bot.sendMessage(msg["chat"]["id"], "👥 users: " + std::to_string(users) + "\n📝 posts: " + std::to_string(posts));
    });
}

}  // namespace api::bots
