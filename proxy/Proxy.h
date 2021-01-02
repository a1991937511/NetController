//
// Created by 70725 on 2020/12/30.
//

#ifndef NETCONTROLLER_PROXY_H
#define NETCONTROLLER_PROXY_H

#include <functional>
#include <boost/system/system_error.hpp>
#include <boost/asio.hpp>
#include <utility>

typedef std::function<void(std::size_t)> readHandler;
typedef std::function<void(std::size_t)> writeHandler;
typedef std::function<void()> readyHandler;

class Proxy : public std::enable_shared_from_this<Proxy> {
public:
    Proxy(boost::asio::io_context &ioc, boost::asio::ip::address address, uint16_t port)
            : address(std::move(address)), port(port), ioc(ioc) {}

    virtual void asyncStart(readyHandler) = 0;

    virtual void asyncRead(char *buf, std::size_t size, readHandler) = 0;

    virtual void asyncWrite(char *buf, std::size_t size, writeHandler) = 0;

protected:
    boost::asio::io_context &ioc;
    boost::asio::ip::address address;
    uint16_t port;
};


#endif //NETCONTROLLER_PROXY_H
