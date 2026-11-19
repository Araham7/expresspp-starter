#pragma once
// Worker for the "emails" queue: job name = template name in views/emails/.
#include <bullmq/bullmq.hpp>

namespace api::jobs {
void startEmailWorker(bullmq::Connection conn);
void stopEmailWorker();
}
