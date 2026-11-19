#pragma once
/* Calling other APIs (axiospp). One preconfigured client per external service, like
   `const github = axios.create({...})` in a Node project.
     auto r = co_await services::http::github().get("/users/" + name);                               */
#include <axios/axios.hpp>

namespace api::services::http {

axios::Instance &github();

}  // namespace api::services::http
