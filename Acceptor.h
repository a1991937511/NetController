//
// Created by 70725 on 2020/12/10.
//

#ifndef NETCONTROLLER_ACCEPTOR_H
#define NETCONTROLLER_ACCEPTOR_H

#include <boost/asio.hpp>

namespace asio = boost::asio;

class Acceptor : public std::enable_shared_from_this<Acceptor> {
public:
    explicit Acceptor(asio::io_context &ioc);

    void start();

private:
    boost::asio::io_context &ioc;
    asio::ip::tcp::acceptor acceptor;

    void asyncAccept();
};


#endif //NETCONTROLLER_ACCEPTOR_H
