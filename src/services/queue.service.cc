#include "services/queue.service.hpp"

#include <memory>

#include "config/config.hpp"
#include "jobs/email.job.hpp"

namespace api::services::queue {

namespace {
std::unique_ptr<bullmq::Queue> gEmails;
}

bullmq::Connection connection() {
    const auto &c = config::get();
    if (c.redisHost.empty()) return bullmq::memory();
    return bullmq::redis({.host = c.redisHost, .port = c.redisPort, .password = c.redisPassword, .prefix = "my-server"});
}

void init() {
    gEmails = std::make_unique<bullmq::Queue>("emails", connection());
    jobs::startEmailWorker(connection());
    std::printf("📬 Queue: %s\n", config::get().redisHost.empty() ? "in-memory" : "redis");
}

void shutdown() {
    jobs::stopEmailWorker();
    gEmails.reset();
}

bullmq::Queue &emails() {
    if (!gEmails) throw std::logic_error("queue::init() was not called");
    return *gEmails;
}

}  // namespace api::services::queue
