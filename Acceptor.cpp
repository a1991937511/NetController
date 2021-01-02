//
// Created by 70725 on 2020/12/10.
//

#include "Acceptor.h"
#include "Session.h"
#include <spdlog/spdlog.h>

namespace socket_option = boost::asio::detail::socket_option;

Acceptor::Acceptor(asio::io_context &ioc) : ioc(ioc), acceptor(ioc) {}

void Acceptor::asyncAccept() {
    auto self = shared_from_this();
    acceptor.async_accept([self, this](const boost::system::error_code &error, boost::asio::ip::tcp::socket peer) {
        if (error) {
            SPDLOG_ERROR("Accept fail! {}", error.message());
            return;
        }
        SPDLOG_DEBUG("Accept local {}:{} session", peer.remote_endpoint().address().to_string(),
                peer.remote_endpoint().port());
        auto session = std::make_shared<Session>(ioc, peer);
        session->start();
        asyncAccept();
    });
}

void Acceptor::start() {
    acceptor.open(asio::ip::tcp::v6());
    acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.set_option(socket_option::boolean<SOL_SOCKET, SO_REUSEPORT>(true));
    acceptor.set_option(socket_option::boolean<SOL_IP, IP_TRANSPARENT>(true));
    acceptor.set_option(socket_option::boolean<SOL_IPV6, IP_TRANSPARENT>(true));

    acceptor.bind(asio::ip::tcp::endpoint(asio::ip::address_v6::any(), 12345));
    acceptor.listen();
    asyncAccept();
}
