//
// Created by 70725 on 2021/1/3.
//

#include "Torjan.h"
#include "trojanrequest.h"
#include "../Utils.h"
#include <openssl/ssl.h>
#include <spdlog/spdlog.h>
#include <utility>

Torjan::Torjan(boost::asio::io_context &ioc, std::string torjanAddress, const boost::asio::ip::address &address, uint16_t port)
        : Proxy(ioc, address, port), ssl_context(boost::asio::ssl::context::sslv23), torjanAddress(std::move(torjanAddress)),
          torjanSocket(ioc, ssl_context) {

    ssl_context.set_verify_mode(SSL_VERIFY_NONE);
    SSL_CTX_set_options(ssl_context.native_handle(), SSL_OP_NO_TICKET);
    SSL_set_tlsext_host_name(torjanSocket.native_handle(), torjanAddress.c_str());
}

Torjan::~Torjan() {
    boost::system::error_code error;
    torjanSocket.lowest_layer().shutdown(torjanSocket.lowest_layer().shutdown_both, error);
    torjanSocket.lowest_layer().close(error);
}

void Torjan::asyncStart(readyHandler handler) {
    auto self = shared_from_this();

    auto resolver = std::make_shared<boost::asio::ip::tcp::resolver>(ioc);
    resolver->async_resolve(torjanAddress, "2083",
            [this, self, resolver, handler](boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results) {
                if (ec) {
                    if (ec == boost::asio::error::operation_aborted) {
                        return;
                    }
                    SPDLOG_ERROR("host resolve fail! {}", ec.message());
                    return;
                }
                torjanSocket.lowest_layer().open(results->endpoint().protocol(), ec);
                if (ec) {
                    SPDLOG_ERROR("socket open fail! {}", ec.message());
                    return;
                }
                Utils::setSocketMask(torjanSocket.lowest_layer(), 0xff, ec);
                if (ec) {
                    SPDLOG_ERROR("set socket mask fail! {}", ec.message());
                    return;
                }

                torjanSocket.lowest_layer().async_connect(results->endpoint(),
                        [this, self, handler, results](const boost::system::error_code &ec) {
                            if (ec) {
                                if (ec == boost::asio::error::operation_aborted) {
                                    return;
                                }
                                SPDLOG_ERROR("torjan connect fail! {}", ec.message());
                                return;
                            }
                            torjanSocket.async_handshake(boost::asio::ssl::stream_base::client,
                                    [this, self, handler, results](const boost::system::error_code &ec) {
                                        if (ec) {
                                            if (ec == boost::asio::error::operation_aborted) {
                                                return;
                                            }
                                            SPDLOG_ERROR("torjan handshake fail! {}", ec.message());
                                            return;
                                        }
                                        auto torjanReq = TrojanRequest::generate(Utils::SHA224("372100"), address.to_string(), port, true);
                                        auto copyData = std::make_shared<std::string>(std::move(torjanReq));
                                        boost::asio::async_write(torjanSocket, boost::asio::buffer(*copyData),
                                                [self, handler, copyData](boost::system::error_code ec, std::size_t size) {
                                                    if (ec) {
                                                        if (ec == boost::asio::error::operation_aborted) {
                                                            return;
                                                        }
                                                        SPDLOG_ERROR("torjan consult fail! {}", ec.message());
                                                        return;
                                                    }
                                                    handler();
                                                });
                                    });
                        });
//
//                boost::asio::async_connect(torjanSocket.lowest_layer(), results,
//                        [this, self, handler](const boost::system::error_code &ec, const boost::asio::ip::tcp::endpoint &endpoint) {
//                            if (ec) {
//                                if (ec == boost::asio::error::operation_aborted) {
//                                    return;
//                                }
//                                SPDLOG_ERROR("torjan connect fail! {}", ec.message());
//                                return;
//                            }
//                            torjanSocket.async_handshake(boost::asio::ssl::stream_base::client,
//                                    [self, handler](const boost::system::error_code &ec) {
//                                        if (ec) {
//                                            if (ec == boost::asio::error::operation_aborted) {
//                                                return;
//                                            }
//                                            SPDLOG_ERROR("torjan handshake fail! {}", ec.message());
//                                            return;
//                                        }
//                                        handler();
//                                    });
//                        });
            });
}

void Torjan::asyncRead(char *buf, std::size_t size, readHandler handler) {
    auto self = shared_from_this();
    torjanSocket.async_read_some(boost::asio::buffer(buf, size),
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

void Torjan::asyncWrite(char *buf, std::size_t size, writeHandler handler) {
    auto self = shared_from_this();
    boost::asio::async_write(torjanSocket, boost::asio::buffer(buf, size),
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
