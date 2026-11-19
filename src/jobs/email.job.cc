#include "jobs/email.job.hpp"

#include <memory>

#include "services/mail.service.hpp"

namespace api::jobs {

namespace {
std::unique_ptr<bullmq::Worker> gWorker;

bullmq::Task<bullmq::Json> processEmail(bullmq::Job &job) {
    std::string to = job.data["to"].asString();
    std::string subject = job.data["subject"].asString();
    std::string tpl = job.name;
    bullmq::Json data = job.data;
    auto info = co_await services::mail::sendTemplate(to, subject, tpl, data);
    co_return bullmq::obj("messageId", info.messageId);
}
}  // namespace

void startEmailWorker(bullmq::Connection conn) {
    gWorker = std::make_unique<bullmq::Worker>("emails", processEmail, bullmq::WorkerOptions{.connection = conn, .concurrency = 2});
    gWorker->on("failed", [](const bullmq::Job &j) {
        std::fprintf(stderr, "📬 email job %s #%s failed: %s\n", j.name.c_str(), j.id.c_str(), j.failedReason.c_str());
    });
}

void stopEmailWorker() {
    if (gWorker) gWorker->close();
    gWorker.reset();
}

}  // namespace api::jobs
