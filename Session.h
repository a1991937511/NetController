//
// Created by 70725 on 2020/12/10.
//

#ifndef NETCONTROLLER_SESSION_H
#define NETCONTROLLER_SESSION_H

#include <boost/asio.hpp>
#include <utility>
#include "proxy/Proxy.h"
//     w1                                       w2
// L ------>localSocket  localBuf remoteSocket------>R
// L <------localSocket remoteBuf remoteSocket<------R
//     R1                                       R2

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::io_context &ioc, boost::asio::ip::tcp::socket &socket);

    virtual ~Session();

    void start();

private:
    boost::asio::ip::tcp::socket localSocket;
    std::shared_ptr<Proxy> proxy;

    enum {
        LOCAL_BUF_LEN = 1024 * 8,
        REMOTE_BUF_LEN = 1024 * 8
    };
    size_t localBufLen = LOCAL_BUF_LEN;
    size_t remoteBufLen = REMOTE_BUF_LEN;
    char *localBuf{};
    char *remoteBuf{};
    std::pair<std::string, uint16_t> localEndpoint;
    uint64_t writenBytes{};
    uint64_t receivedBytes{};

    void resizeLocalBuf(size_t size);

    void resizeRemoteBuf(size_t size);

    void destroy();

    void localAsyncRead();

    void remoteAsyncRead();
};


#endif //NETCONTROLLER_SESSION_H
