#pragma once
/* Background jobs (bullmqpp). Redis when REDIS_HOST is set, otherwise an in-memory queue.
     services::queue::emails().add("welcome", bullmq::obj("to", email, "name", name), {.attempts = 3});
   Workers live in src/jobs/.                                                                        */
#include <bullmq/bullmq.hpp>

namespace api::services::queue {

void init();                     // queues + workers
void shutdown();                 // let running jobs finish
bullmq::Connection connection();
bullmq::Queue &emails();

}  // namespace api::services::queue
