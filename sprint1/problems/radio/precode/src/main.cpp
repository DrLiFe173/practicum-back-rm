#ifdef WIN32
#include <sdkddkver.h>
#endif

#include <boost/asio.hpp>
#include "audio.h"
#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>

namespace net = boost::asio;
using net::ip::udp;

using namespace std::literals;

static std::string_view ip_address = "127.0.0.1";
static size_t messageSize = 65000;  

void StartServer(const uint16_t port) {
    Player player(ma_format_u8, 1);
    try {
        boost::asio::io_context io_context;

        udp::socket socket(io_context, udp::endpoint(udp::v4(), port));
        
        for (;;) {
            std::vector<char> recv_buf(messageSize);
            udp::endpoint remote_endpoint;

            auto size = socket.receive_from(net::buffer(recv_buf), remote_endpoint);

            size_t recordSize = static_cast<size_t>(recv_buf.size() / player.GetFrameSize());
           
            player.PlayBuffer(recv_buf.data(), recordSize, 1.5s);                 
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;    
    }
}

void StartClient(const uint16_t port) {
    Recorder recorder(ma_format_u8, 1);
    try {
        net::io_context io_context;

        udp::socket socket(io_context, udp::v4());

        boost::system::error_code ec;
        auto endpoint = udp::endpoint(net::ip::make_address(ip_address, ec), port);       

        std::cout << "Message recording is started..." << std::endl;

        auto rec_result = recorder.Record(messageSize, 1.5s);
        std::cout << "Recording is done" << std::endl;

        size_t recordSize = static_cast<size_t>(recorder.GetFrameSize() * rec_result.frames);
        socket.send_to(net::buffer(rec_result.data, recordSize), endpoint);

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;     
    }
}

int main(int argc, char** argv) {
    
    if (argc != 3) {
        std::cout << "Usage: "sv << argv[0] << " client/server"sv << " <port>"sv << std::endl;
        return 1;
    }

    std::stringstream convert{ argv[2] };
    int intPortVal{};
    if (!(convert >> intPortVal))
    {        
        std::cout << "Unable to read <port> argument "sv << argv[2] << std::endl;
        return 1;
    }
    static const uint16_t port = static_cast<uint16_t>(intPortVal);

    if (port < 0 || port > 65535) {
        std::cout << "Incorrect value of <port> argument "sv << port << std::endl;
        return 1;
    }

    const std::string_view radio_type = argv[1];
    if (radio_type == "client"sv) {  
        StartClient(port);
    } else if (radio_type == "server"sv) {
        StartServer(port);
    } else {
        std::cout << radio_type << " is not client/server command!"sv << std::endl;
        return 1;
    }

    return 0;
}
