//
// Created by 70725 on 2020/12/30.
//

#ifndef NETCONTROLLER_SOCKS5_H
#define NETCONTROLLER_SOCKS5_H

#include "Proxy.h"

class Socks5 : public Proxy {
public:
    Socks5(boost::asio::io_context &ioc, boost::asio::ip::tcp::endpoint  socks5Endpoint, const boost::asio::ip::address &address, uint16_t port);

    void asyncStart(readyHandler handler) override;

    void asyncRead(char *buf, std::size_t size, readHandler handler) override;

    void asyncWrite(char *buf, std::size_t size, writeHandler handler) override;

private:
    boost::asio::ip::tcp::socket socks5;
    boost::asio::ip::tcp::endpoint socks5Endpoint;
};


#endif //NETCONTROLLER_SOCKS5_H
