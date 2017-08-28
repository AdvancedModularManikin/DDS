#include "stdafx.h"
#include "AMM/DDS_Manager.h"

#include "boost/asio.hpp"


using namespace std;

bool closed = false;

boost::asio::io_service io_service;

boost::asio::ip::udp::endpoint broadcast_endpoint(boost::asio::ip::address_v4::broadcast(), 9015);
boost::asio::ip::udp::socket udpSocket(io_service,
                                    boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));

class GenericListener : public ListenerInterface {



public:
    GenericListener() {
        udpSocket.set_option(boost::asio::socket_base::broadcast(true));
    }

    void onNewNodeData(AMM::Physiology::Node n) {
        if (n.nodepath() == "EXIT") {
            cout << "Shutting down simulation based on shutdown node-data from physiology engine." << endl;
            closed = true;
            return;
        }
        std::ostringstream messageOut;
        messageOut << n.nodepath() << "=" << n.dbl();
        boost::shared_ptr<std::string> message(
                new std::string(messageOut.str()));
        udpSocket.send_to(boost::asio::buffer(*message), broadcast_endpoint);
    }

    void onNewCommandData(AMM::PatientAction::BioGears::Command c) {
        std::ostringstream messageOut;
        messageOut << "[ACT]" <<  c.message();
        boost::shared_ptr<std::string> message(
                new std::string(messageOut.str()));
        udpSocket.send_to(boost::asio::buffer(*message), broadcast_endpoint);
    }
};



int main(int argc, char *argv[]) {
    int count = 0;

    auto *mgr = new DDS_Manager();

    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();
    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    auto *pub_listener = new DDS_Listeners::PubListener();

    GenericListener al;
    node_sub_listener->SetUpstream(&al);
    command_sub_listener->SetUpstream(&al);

    Subscriber *node_subscriber = mgr->InitializeNodeSubscriber(node_sub_listener);
    Subscriber *command_subscriber = mgr->InitializeCommandSubscriber(command_sub_listener);
    Publisher *command_publisher = mgr->InitializeCommandPublisher(pub_listener);

    cout << "=== [UnityBridge] Ready ..." << endl;



    while (!closed) {

    }

    cout << "=== [UnityBridge] Simulation stopped." << endl;

    return 0;

}

