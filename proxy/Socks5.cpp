//
// Created by 70725 on 2020/12/30.
//

#include "Socks5.h"

Socks5::Socks5(boost::asio::io_context &ioc, const boost::asio::ip::tcp::endpoint &socks5Endpoint,
               const boost::asio::ip::address &address, uint16_t port) : Proxy(ioc, address, port) {

}

void Socks5::asyncStart(readyHandler handler) {

}

void Socks5::asyncRead(char *buf, std::size_t size, readHandler handler) {

}

void Socks5::asyncWrite(char *buf, std::size_t size, writeHandler handler) {

}






