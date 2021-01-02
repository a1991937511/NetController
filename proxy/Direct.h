//
// Created by pp on 2021/1/2.
//

#ifndef NETCONTROLLER_DIRECT_H
#define NETCONTROLLER_DIRECT_H

#include "Proxy.h"

class Direct : public Proxy {
public:
    Direct(boost::asio::io_context &ioc, const boost::asio::ip::address &address, uint16_t port);

    virtual ~Direct();

    void asyncStart(readyHandler handler) override;

    void asyncRead(char *buf, std::size_t size, readHandler handler) override;

    void asyncWrite(char *buf, std::size_t size, writeHandler handler) override;

private:
    boost::asio::ip::tcp::socket remoteSocket;
};


#endif //NETCONTROLLER_DIRECT_H
