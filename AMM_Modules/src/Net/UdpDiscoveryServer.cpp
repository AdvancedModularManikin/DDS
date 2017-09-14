#include "UdpDiscoveryServer.h"



void UdpDiscoveryServer::handle_receive_from(const boost::system::error_code &error,
                                        size_t bytes_recvd) {
    if (!error && bytes_recvd > 0) {
        cout << "\tWe got data from " << sender_endpoint_ << ": " << data_ << endl;
        // @TODO: Verify this is a valid request before sending data back
        socket_.async_send_to(
                boost::asio::buffer(data_, bytes_recvd), sender_endpoint_,
                boost::bind(&UdpDiscoveryServer::handle_send_to, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
    } else {
        socket_.async_receive_from(
                boost::asio::buffer(data_, max_length), sender_endpoint_,
                boost::bind(&UdpDiscoveryServer::handle_receive_from, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
    }
}

void UdpDiscoveryServer::handle_send_to(const boost::system::error_code &error, size_t bytes_sent) {
    cout << "\tSending data to " << sender_endpoint_ << endl;
    // @TODO: Send back some sort of validation
    socket_.async_receive_from(
            boost::asio::buffer(data_, max_length), sender_endpoint_,
            boost::bind(&UdpDiscoveryServer::handle_receive_from, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
}

