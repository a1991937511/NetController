//
// Created by pp on 2021/1/2.
//

#include "Direct.h"
#include "../Utils.h"

Direct::Direct(boost::asio::io_context &ioc, const boost::asio::ip::address &address, uint16_t port) : Proxy(ioc,
        address, port), remoteSocket(ioc) {}

void Direct::asyncStart(readyHandler handler) {
    boost::system::error_code ec;

    auto endpoint = boost::asio::ip::tcp::endpoint(address, port);
    remoteSocket.open(endpoint.protocol());
    Utils::setSocketMask(remoteSocket, 0xff, ec);
    if (ec) {
        SPDLOG_ERROR("set socket mask fail! {}", ec.message());
        return;
    }
    auto self = shared_from_this();
    remoteSocket.async_connect(endpoint, [self, this, handler](const boost::system::error_code &error) {
        if (error) {
            if (error == boost::asio::error::operation_aborted) {
                return;
            }
            SPDLOG_ERROR("Connect to remote fail! {}", error.message());
            return;
        }
        handler();
    });
}


void Direct::asyncRead(char *buf, std::size_t size, readHandler handler) {
    auto self = shared_from_this();
    remoteSocket.async_read_some(boost::asio::buffer(buf, size),
            [self, handler](boost::system::error_code ec, std::size_t size) {
                if (ec) {
                    if (ec == boost::asio::error::operation_aborted) {
                        return;
                    }
                    SPDLOG_ERROR("remote read fail! {}", ec.message());
                    return;
                }
                handler(size);
            });
}

void Direct::asyncWrite(char *buf, std::size_t size, writeHandler handler) {
    auto self = shared_from_this();
    boost::asio::async_write(remoteSocket, boost::asio::buffer(buf, size),
            [self, handler](boost::system::error_code ec, std::size_t size) {
                if (ec) {
                    if (ec == boost::asio::error::operation_aborted) {
                        return;
                    }
                    SPDLOG_ERROR("remote write fail! {}", ec.message());
                    return;
                }
                handler(size);
            });
}

Direct::~Direct() {
    boost::system::error_code error;
    remoteSocket.shutdown(remoteSocket.shutdown_both, error);
    remoteSocket.close(error);
}





