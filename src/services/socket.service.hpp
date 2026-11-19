#pragma once
/* Realtime (socketiopp). Clients connect with  io(URL, {transports: ["websocket"], auth: {token}}).
   A valid token joins the room "user:<id>", so you can notify one user from anywhere:
     services::socket::toUser(id, "notification", data);
   Socket event handlers live in socket.service.cc (onConnection).                                    */
#include <socketio/socketio.hpp>

#include <string>

namespace api::services::socket {

void init();  // before app.listen()
socketio::Server &io();
void broadcast(const std::string &event, const socketio::Json &data);
void toUser(const std::string &userId, const std::string &event, const socketio::Json &data);

}  // namespace api::services::socket
