#pragma once
/* Sending mail (nodemailerpp). SMTP when SMTP_HOST is set, otherwise the mail is printed to the log.
   Templates: views/emails/<name>.html (mustache: {{name}}).                                         */
#include <nodemailer/nodemailer.hpp>

#include <json/json.h>
#include <string>

namespace api::services::mail {

void init();
nodemailer::Pending send(nodemailer::Mail mail);  // co_await it (or .get())
// renders views/emails/<tpl>.html with data and sends it
nodemailer::Pending sendTemplate(const std::string &to, const std::string &subject, const std::string &tpl,
                                 const Json::Value &data);
int sentCount();  // for tests / health

}  // namespace api::services::mail
