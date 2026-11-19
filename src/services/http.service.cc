#include "services/http.service.hpp"

namespace api::services::http {

axios::Instance &github() {
    static axios::Instance api = axios::create({
        .baseURL = "https://api.github.com",
        .headers = {{"Accept", "application/vnd.github+json"}},
        .timeout = 5000,
    });
    return api;
}

}  // namespace api::services::http
