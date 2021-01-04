//
// Created by 70725 on 2020/12/12.
//

#ifndef NETCONTROLLER_UTILS_H
#define NETCONTROLLER_UTILS_H

#include <boost/asio.hpp>
#include <openssl/evp.h>
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

    static std::string SHA224(const std::string &message) {
        uint8_t digest[EVP_MAX_MD_SIZE];
        char mdString[(EVP_MAX_MD_SIZE << 1) + 1];
        unsigned int digest_len;
        EVP_MD_CTX *ctx;
        if ((ctx = EVP_MD_CTX_new()) == nullptr) {
            SPDLOG_ERROR("could not create hash context");
            return "";
        }
        if (!EVP_DigestInit_ex(ctx, EVP_sha224(), nullptr)) {
            EVP_MD_CTX_free(ctx);
            SPDLOG_ERROR("could not initialize hash context");
            return "";
        }
        if (!EVP_DigestUpdate(ctx, message.c_str(), message.length())) {
            EVP_MD_CTX_free(ctx);
            SPDLOG_ERROR("could not update hash");
            return "";
        }
        if (!EVP_DigestFinal_ex(ctx, digest, &digest_len)) {
            EVP_MD_CTX_free(ctx);
            SPDLOG_ERROR("could not output hash");
            return "";
        }

        for (unsigned int i = 0; i < digest_len; ++i) {
            sprintf(mdString + (i << 1), "%02x", (unsigned int)digest[i]);
        }
        mdString[digest_len << 1] = '\0';
        EVP_MD_CTX_free(ctx);
        return std::string(mdString);
    }
};


#endif //NETCONTROLLER_UTILS_H
