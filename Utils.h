//
// Created by 70725 on 2020/12/12.
//

#ifndef NETCONTROLLER_UTILS_H
#define NETCONTROLLER_UTILS_H

#include <boost/asio.hpp>
#include <spdlog/spdlog.h>

#ifndef SO_ORIGINAL_DST
#define SO_ORIGINAL_DST 80
#endif // SO_ORIGINAL_DST
#ifndef IP6T_SO_ORIGINAL_DST
#define IP6T_SO_ORIGINAL_DST 80
#endif // IP6T_SO_ORIGINAL_DST

class Utils {
public:
    static boost::asio::ip::tcp::endpoint getTargetEndpoint(int fd) {
        sockaddr_storage destAddr{};
        socklen_t socklen = sizeof(destAddr);

        auto error = getsockopt(fd, SOL_IPV6, IP6T_SO_ORIGINAL_DST, &destAddr, &socklen);
        if (error) {
            error = getsockopt(fd, SOL_IP, SO_ORIGINAL_DST, &destAddr, &socklen);
            if (error) {
                SPDLOG_ERROR("Get original dst fail! {}", error);
                throw std::bad_exception();
            }
        }

        char ipStr[INET_ADDRSTRLEN];
        auto *sa = (sockaddr_in *) &destAddr;
        inet_ntop(AF_INET, &(sa->sin_addr), ipStr, INET_ADDRSTRLEN);
        auto port = ntohs(sa->sin_port);
        return boost::asio::ip::tcp::endpoint{boost::asio::ip::make_address(ipStr), port};
    }

    template<class T>
    static void setSocketMask(boost::asio::basic_socket<T> &socket, int mask, boost::system::error_code &errorCode) {
        socket.set_option(boost::asio::detail::socket_option::integer<SOL_SOCKET, SO_MARK>(mask), errorCode);
    }
};


#endif //NETCONTROLLER_UTILS_H
