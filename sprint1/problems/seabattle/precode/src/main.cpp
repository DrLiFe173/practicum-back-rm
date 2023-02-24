#ifdef WIN32
#include <sdkddkver.h>
#endif

#include "seabattle.h"

#include <atomic>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <string_view>

namespace net = boost::asio;
using net::ip::tcp;
using namespace std::literals;

static const std::string_view IP_ADDRESS = "127.0.0.1";

void PrintFieldPair(const SeabattleField& left, const SeabattleField& right) {
    auto left_pad = "  "s;
    auto delimeter = "    "s;
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
    for (size_t i = 0; i < SeabattleField::field_size; ++i) {
        std::cout << left_pad;
        left.PrintLine(std::cout, i);
        std::cout << delimeter;
        right.PrintLine(std::cout, i);
        std::cout << std::endl;
    }
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
}

template <size_t sz>
static std::optional<std::string> ReadExact(tcp::socket& socket) {
    boost::array<char, sz> buf;
    boost::system::error_code ec;

    net::read(socket, net::buffer(buf), net::transfer_exactly(sz), ec);

    if (ec) {
        return std::nullopt;
    }

    return {{buf.data(), sz}};
}

static bool WriteExact(tcp::socket& socket, std::string_view data) {
    boost::system::error_code ec;

    net::write(socket, net::buffer(data), net::transfer_exactly(data.size()), ec);

    return !ec;
}

class SeabattleAgent {
public:
    SeabattleAgent(const SeabattleField& field)
        : my_field_(field) {
    }

    void StartGame(tcp::socket& socket, bool my_initiative) {
            
        PrintFields();

        while (!IsGameEnded()) {
            if (my_initiative) {
                std::string command;
                std::getline(std::cin, command, '\n');
                command.append("\n");

                SendMove(socket, command);

                auto shootComand = ParseMove(command.erase(2));

                SeabattleField::ShotResult shootResult = ReadResult(socket);

                switch (shootResult) {
                case SeabattleField::ShotResult::MISS:
                    std::cout << "Miss!"sv << std::endl;
                    other_field_.MarkMiss(shootComand.value().second, shootComand.value().first);
                    my_initiative = false;
                    break;
                case SeabattleField::ShotResult::HIT:
                    std::cout << "Hit!"sv << std::endl;
                    other_field_.MarkHit(shootComand.value().second, shootComand.value().first);
                    break;
                case SeabattleField::ShotResult::KILL:
                    std::cout << "Kill!"sv << std::endl;
                    other_field_.MarkKill(shootComand.value().second, shootComand.value().first);
                    break;
                default:
                    std::cout << "Incorrect return message "sv << static_cast<int>(shootResult) << std::endl;
                }
               
            }
            else {
                std::string opponentMove = ReadMove(socket);
                std::cout << "Shot to "sv << opponentMove;

                auto shootResult = ParseMove(opponentMove.erase(2));
                const SeabattleField::ShotResult result = my_field_.Shoot(shootResult.value().second, shootResult.value().first);
               
                if (result == SeabattleField::ShotResult::MISS) {
                    my_initiative = true;
                }
                 
                SendResult(socket, result);
            }
            PrintFields();
        }
        if (other_field_.IsLoser()) {
            std::cout << "You win!"sv << std::endl;
        }
        else if (my_field_.IsLoser()) {
            std::cout << "You loose!"sv << std::endl;
        }
    }

private:
    static std::optional<std::pair<int, int>> ParseMove(const std::string_view& sv) {
        if (sv.size() != 2) return std::nullopt;

        int p1 = sv[0] - 'A', p2 = sv[1] - '1';

        if (p1 < 0 || p1 > 8) return std::nullopt;
        if (p2 < 0 || p2 > 8) return std::nullopt;

        return {{p1, p2}};
    }

    static std::string MoveToString(std::pair<int, int> move) {
        char buff[] = {static_cast<char>(move.first) + 'A', static_cast<char>(move.second) + '1'};
        return {buff, 2};
    }

    void PrintFields() const {
        PrintFieldPair(my_field_, other_field_);
    }

    bool IsGameEnded() const {
        return my_field_.IsLoser() || other_field_.IsLoser();
    }

    std::string ReadMove(tcp::socket& socket) {
        boost::system::error_code ec;
        net::streambuf read_stream_buf;
        net::read_until(socket, read_stream_buf, '\n', ec);
        std::string client_data{ std::istreambuf_iterator<char>(&read_stream_buf),
                    std::istreambuf_iterator<char>() };
        return client_data;
    }

    SeabattleField::ShotResult ReadResult(tcp::socket& socket) {
        net::streambuf stream_buf;
        boost::system::error_code ec;
        net::read_until(socket, stream_buf, '\n', ec);
        std::string client_data{ std::istreambuf_iterator<char>(&stream_buf),
                    std::istreambuf_iterator<char>() };       

        return static_cast<SeabattleField::ShotResult>(std::stoi(client_data.erase(2)));
    }

    void SendMove(tcp::socket& socket, std::string& command) {
        boost::system::error_code ec;
        socket.write_some(net::buffer(command), ec);
    }

    void SendResult(tcp::socket& socket, const SeabattleField::ShotResult& result) {
        boost::system::error_code ec;
        socket.write_some(net::buffer(std::to_string(static_cast<int>(result)).append("\n")), ec);
    }

private:
    SeabattleField my_field_;
    SeabattleField other_field_;
};

void StartServer(const SeabattleField& field, unsigned short port) {
    SeabattleAgent agent(field);

    net::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(net::ip::make_address(IP_ADDRESS), port));
    //tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
    boost::system::error_code ec;
    tcp::socket socket{ io_context };
    std::cout << "Waiting your opponent first turn ... "sv << std::endl;
    acceptor.accept(socket, ec);

    agent.StartGame(socket, false);
};

void StartClient(const SeabattleField& field, const std::string& ip_str, unsigned short port) {
    SeabattleAgent agent(field);

    boost::system::error_code ec;
    auto endpoint = tcp::endpoint(net::ip::make_address(ip_str, ec), port);
    net::io_context io_context;
    tcp::socket socket{ io_context };
    socket.connect(endpoint, ec);

    agent.StartGame(socket, true);
};

int main(int argc, const char** argv) {
    if (argc != 3 && argc != 4) {
        std::cout << "Usage: program <seed> [<ip>] <port>" << std::endl;
        return 1;
    }

    std::mt19937 engine(std::stoi(argv[1]));
    SeabattleField fieldL = SeabattleField::GetRandomField(engine);

    if (argc == 3) {
        StartServer(fieldL, std::stoi(argv[2]));
    } else if (argc == 4) {
        StartClient(fieldL, argv[2], std::stoi(argv[3]));
    }
}
