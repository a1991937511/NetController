//
// Created by 70725 on 2020/12/30.
//

#include "Socks5.h"
#include "../Utils.h"
#include <spdlog/spdlog.h>
#include <utility>

Socks5::Socks5(boost::asio::io_context &ioc, boost::asio::ip::tcp::endpoint socks5Endpoint,
        const boost::asio::ip::address &address, uint16_t port) : Proxy(ioc, address, port), socks5(ioc),
                                                                  socks5Endpoint(std::move(socks5Endpoint)) {

}

void Socks5::asyncStart(readyHandler handler) {
    auto self = shared_from_this();
    boost::system::error_code ec;

    socks5.open(socks5Endpoint.protocol(), ec);
    if (ec) {
        SPDLOG_ERROR("socket open fail! {}", ec.message());
        return;
    }
    Utils::setSocketMask(socks5, 0xff, ec);
    if (ec) {
        SPDLOG_ERROR("set socket mask fail! {}", ec.message());
        return;
    }
    socks5.async_connect(socks5Endpoint, [this, handler, self](boost::system::error_code ec) {
        if (ec) {
            if (ec == boost::asio::error::operation_aborted) {
                return;
            }
            SPDLOG_ERROR("connect socks5 fail! {}", ec.message());
            return;
        }
        auto socks5Req = std::make_shared<std::vector<char>>(10);
        (*socks5Req)[0] = 0x05;
        (*socks5Req)[1] = 1;
        (*socks5Req)[2] = 0;
        boost::asio::async_write(socks5, boost::asio::buffer(*socks5Req, 3),
                [this, socks5Req, self, handler](boost::system::error_code ec, std::size_t size) {
                    if (ec) {
                        if (ec == boost::asio::error::operation_aborted) {
                            return;
                        }
                        SPDLOG_ERROR("socks5 write fail! {}", ec.message());
                        return;
                    }
                    boost::asio::async_read(socks5, boost::asio::buffer(*socks5Req, 2),
                            [socks5Req, this, self, handler](boost::system::error_code ec, std::size_t size) {
                                if (ec) {
                                    if (ec == boost::asio::error::operation_aborted) {
                                        return;
                                    }
                                    SPDLOG_ERROR("socks5 read fail! {}", ec.message());
                                    return;
                                }
                                if ((*socks5Req)[1] != 0) {
                                    SPDLOG_ERROR("socks5 consult fail! rep method {}", (*socks5Req)[1]);
                                    return;
                                }
                                (*socks5Req)[0] = 0x05;
                                (*socks5Req)[1] = 0x01;
                                (*socks5Req)[2] = 0x00;
                                (*socks5Req)[3] = 0x01;
                                *reinterpret_cast<uint32_t *>(socks5Req->data() + 4) = *(uint32_t *) address.to_v4().to_bytes().data();
                                *reinterpret_cast<uint16_t *>(socks5Req->data() + 8) = boost::asio::detail::socket_ops::host_to_network_short(port);
                                boost::asio::async_write(socks5, boost::asio::buffer(*socks5Req, 10),
                                        [socks5Req, this, self, handler](boost::system::error_code ec, std::size_t size) {
                                            if (ec) {
                                                if (ec == boost::asio::error::operation_aborted) {
                                                    return;
                                                }
                                                SPDLOG_ERROR("socks5 write fail! {}", ec.message());
                                                return;
                                            }
                                            boost::asio::async_read(socks5, boost::asio::buffer(*socks5Req, 10),
                                                    [socks5Req, handler, self](boost::system::error_code ec,
                                                            std::size_t size) {
                                                        if (ec) {
                                                            SPDLOG_ERROR("socks5 read fail! {}", ec.message());
                                                            return;
                                                        }
                                                        if ((*socks5Req)[1] != 0) {
                                                            SPDLOG_ERROR("socks5 consult fail! rep {}", (*socks5Req)[1]);
                                                            return;
                                                        }
                                                        handler();
                                                    });
                                        });
                            });
                });
    });
}

void Socks5::asyncRead(char *buf, std::size_t size, readHandler handler) {
    auto self = shared_from_this();
    socks5.async_read_some(boost::asio::buffer(buf, size),
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

void Socks5::asyncWrite(char *buf, std::size_t size, writeHandler handler) {
    auto self = shared_from_this();
    boost::asio::async_write(socks5, boost::asio::buffer(buf, size),
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

Socks5::~Socks5() {
    boost::system::error_code error;
    socks5.shutdown(socks5.shutdown_both, error);
    socks5.close(error);
}






