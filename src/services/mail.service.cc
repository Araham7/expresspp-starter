#include "services/mail.service.hpp"

#include <express/view.hpp>

#include <atomic>
#include <fstream>
#include <memory>
#include <sstream>

#include "config/config.hpp"

namespace api::services::mail {

namespace {
std::unique_ptr<nodemailer::Transporter> gTransport;
std::atomic<int> gSent{0};

std::string readFile(const std::string &path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("mail template not found: " + path);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}
}  // namespace

void init() {
    const auto &c = config::get();
    if (!c.smtpHost.empty()) {
        gTransport = std::make_unique<nodemailer::Transporter>(nodemailer::createTransport(
            {.host = c.smtpHost, .port = c.smtpPort, .secure = c.smtpSecure, .user = c.smtpUser, .pass = c.smtpPass}));
        std::printf("📧 Mail: SMTP %s:%d\n", c.smtpHost.c_str(), c.smtpPort);
    } else {
        gTransport = std::make_unique<nodemailer::Transporter>(nodemailer::createJsonTransport());
        std::printf("📧 Mail: no SMTP_HOST, mails are only logged\n");
    }
    gTransport->defaults({.from = c.mailFrom});
}

nodemailer::Pending send(nodemailer::Mail mail) {
    if (!gTransport) init();
    bool logOnly = config::get().smtpHost.empty();
    auto &t = *gTransport;
    return nodemailer::Pending([&t, mail, logOnly] {
        auto info = t.sendMail(mail).get();
        gSent++;
        if (logOnly) std::printf("📧 (log only) to=%s subject=\"%s\"\n", mail.to.empty() ? "" : mail.to[0].c_str(),
                                 mail.subject.c_str());
        return info;
    });
}

nodemailer::Pending sendTemplate(const std::string &to, const std::string &subject, const std::string &tpl,
                                 const Json::Value &data) {
    std::string html = express::renderTemplate(readFile("views/emails/" + tpl + ".html"), data);
    return send({.to = {to}, .subject = subject, .html = html});
}

int sentCount() { return gSent.load(); }

}  // namespace api::services::mail
