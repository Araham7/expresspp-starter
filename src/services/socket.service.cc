#include "services/socket.service.hpp"

#include <express/jwt.hpp>

#include <memory>

#include "config/config.hpp"

namespace api::services::socket {

namespace {
std::unique_ptr<socketio::Server> gIo;

void onConnection(socketio::Socket s) {
    s.emit("welcome", socketio::obj("id", s.id()));
    s.on("ping", [](const socketio::Json &, socketio::Ack ack) { ack("pong"); });
    s.on("room:join", [s](const socketio::Json &room) { s.join("room:" + room.asString()); });
    s.on("room:message", [s](const socketio::Json &m) {
        s.to("room:" + m["room"].asString()).emit("room:message", m);  // everyone in the room except the sender
    });
}
}  // namespace

void init() {
    gIo = std::make_unique<socketio::Server>();
    // auth middleware: guests are allowed, a token (if given) must be valid
    gIo->use([](socketio::Socket s, socketio::Next next) {
        std::string token = s.handshake().auth["token"].asString();
        if (!token.empty()) {
            try {
                auto user = express::jwt::verify(token, config::get().jwtSecret, {.issuer = "my-server"});
                s.join("user:" + user["sub"].asString());
            } catch (const std::exception &) {
                return next.fail("invalid token");
            }
        }
        next();
    });
    gIo->on("connection", onConnection);
    gIo->attach();
}

socketio::Server &io() { return *gIo; }

void broadcast(const std::string &event, const socketio::Json &data) {
    if (gIo) gIo->emit(event, data);
}

void toUser(const std::string &userId, const std::string &event, const socketio::Json &data) {
    if (gIo) gIo->to("user:" + userId).emit(event, data);
}

}  // namespace api::services::socket
