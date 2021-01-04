//
// Created by 70725 on 2020/12/10.
//

#include "Session.h"
#include "Utils.h"
#include "proxy/Direct.h"
#include "proxy/Socks5.h"
#include "proxy/Torjan.h"

namespace asio = boost::asio;
namespace socket_option = boost::asio::detail::socket_option;

Session::Session(boost::asio::io_context &ioc, asio::ip::tcp::socket &socket) : localSocket(std::move(socket)) {
    localEndpoint.first = localSocket.remote_endpoint().address().to_string();
    localEndpoint.second = localSocket.remote_endpoint().port();
    resizeLocalBuf(LOCAL_BUF_LEN);
    resizeRemoteBuf(REMOTE_BUF_LEN);

    auto endpoint = Utils::getTargetEndpoint(localSocket.native_handle());
//    proxy = std::make_shared<Direct>(ioc, endpoint.address(), endpoint.port());
//    auto sock5Endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address("192.168.42.1"), 10808);
//    proxy = std::make_shared<Socks5>(ioc, sock5Endpoint, endpoint.address(), endpoint.port());

    proxy = std::make_shared<Torjan>(ioc, "torjan.tindongr.cloud", endpoint.address(), endpoint.port());

    SPDLOG_DEBUG("{}:{} connect to {}:{}", localEndpoint.first, localEndpoint.second, endpoint.address().to_string(),
            endpoint.port());
}

void Session::start() {
    auto self(shared_from_this());
    proxy->asyncStart([this, self]() {
        localAsyncRead();
        remoteAsyncRead();
    });
}

void Session::localAsyncRead() {
    auto self = shared_from_this();
    localSocket.async_read_some(asio::buffer(localBuf, localBufLen),
            [self, this](boost::system::error_code error, std::size_t size) {
                if (error) {
                    if (error == asio::error::operation_aborted) {
                        return;
                    }
                    SPDLOG_ERROR("Local read error! {}", error.message());
                    return;
                }
                if (size == localBufLen) {
                    resizeLocalBuf(localBufLen * 2);
                    SPDLOG_DEBUG("Local buf resize to {}", localBufLen * 2);
                }
                SPDLOG_DEBUG("Local buf use {}%", size * 100 / localBufLen);
                proxy->asyncWrite(localBuf, size, [this, self](std::size_t size) {
                    writenBytes += size;
                    localAsyncRead();
                });
            });
}

void Session::remoteAsyncRead() {
    auto self = shared_from_this();
    proxy->asyncRead(remoteBuf, remoteBufLen, [this, self](std::size_t size) {
        if (size == remoteBufLen) {
            resizeRemoteBuf(remoteBufLen * 2);
            SPDLOG_DEBUG("Remote buf resize to {}", remoteBufLen * 2);
        }
        SPDLOG_DEBUG("Remote buf use {}%", size * 100 / remoteBufLen);
        boost::asio::async_write(localSocket, asio::buffer(remoteBuf, size),
                [self, this](boost::system::error_code error, std::size_t size) {
                    if (error) {
                        if (error == asio::error::operation_aborted) {
                            return;
                        }
                        SPDLOG_ERROR("local write error! {}", error.message());
                        return;
                    }
                    receivedBytes += size;
                    remoteAsyncRead();
                });
    });
}

void Session::resizeLocalBuf(size_t size) {
    auto newBuf = (char *) realloc(localBuf, size);
    if (newBuf == nullptr) {
        if (localBuf == nullptr) {
            SPDLOG_ERROR("Alloc local buf fail!");
            throw std::bad_alloc();
        }
        SPDLOG_ERROR("Realloc local buf fail, new size {}", size);
        return;
    }
    localBufLen = size;
    localBuf = newBuf;
}

void Session::resizeRemoteBuf(size_t size) {
    auto newBuf = (char *) realloc(remoteBuf, size);
    if (newBuf == nullptr) {
        if (remoteBuf == nullptr) {
            SPDLOG_ERROR("Alloc local buf fail!");
            throw std::bad_alloc();
        }
        SPDLOG_ERROR("Realloc local buf fail, new size {}", size);
        return;
    }
    remoteBufLen = size;
    remoteBuf = newBuf;
}


void Session::destroy() {
    boost::system::error_code error;
    if (localSocket.is_open()) {
        localSocket.shutdown(localSocket.shutdown_both, error);
        if (error) {
            SPDLOG_ERROR("Local socket shutdown error! {}", error.message());
        }
        localSocket.close(error);
        if (error) {
            SPDLOG_ERROR("Local socket close error! {}", error.message());
        }
    }
}

Session::~Session() {
    destroy();
    if (localBuf != nullptr) {
        free(localBuf);
    }
    if (remoteBuf != nullptr) {
        free(remoteBuf);
    }
}

