//
// Created by 70725 on 2021/1/3.
//

#ifndef NETCONTROLLER_TORJAN_H
#define NETCONTROLLER_TORJAN_H

#include "Proxy.h"
#include <boost/asio/ssl.hpp>

class Torjan : public Proxy {
public:
    Torjan(boost::asio::io_context &ioc, std::string  torjanAddress, const boost::asio::ip::address &address, uint16_t port);

    virtual ~Torjan();

    void asyncStart(readyHandler handler) override;

    void asyncRead(char *buf, std::size_t size, readHandler handler) override;

    void asyncWrite(char *buf, std::size_t size, writeHandler handler) override;

private:
    boost::asio::ssl::context ssl_context;
    std::string torjanAddress;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket>torjanSocket;
};


#endif //NETCONTROLLER_TORJAN_H
