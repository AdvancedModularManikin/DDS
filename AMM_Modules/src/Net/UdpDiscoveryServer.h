#pragma once

#include "stdafx.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using namespace std;
using boost::asio::ip::udp;

class UdpDiscoveryServer {
public:
    UdpDiscoveryServer(boost::asio::io_service &io_service, short port)
            : io_service_(io_service),
              socket_(io_service, udp::endpoint(udp::v4(), port)) {
        // Register async receive-from handler
        socket_.async_receive_from(
                boost::asio::buffer(data_, max_length), sender_endpoint_,
                boost::bind(&UdpDiscoveryServer::handle_receive_from, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
    };

    void handle_receive_from(const boost::system::error_code &error,
                             size_t bytes_recvd);

    void handle_send_to(const boost::system::error_code &error, size_t bytes_sent);

private:
    boost::asio::io_service &io_service_;
    udp::socket socket_;
    udp::endpoint sender_endpoint_;
    enum {
        max_length = 1024
    };
    char data_[max_length];
};
