// ���������� ������������ ���� <sdkddkver.h> � ������� Windows,
// ����� �������� �������������� � ����������� ������ Platform SDK,
// ����� ���������� ������������ ����� ���������� Boost.Asio
#ifdef WIN32
#include <sdkddkver.h>
#endif

// Boost.Beast ����� ������������ std::string_view ������ boost::string_view
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <thread>
#include <iostream> 

namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::literals;
namespace beast = boost::beast;
namespace http = beast::http;
// ������, ���� �������� ������������ � ���� ������
using StringRequest = http::request<http::string_body>;
// �����, ���� �������� ������������ � ���� ������
using StringResponse = http::response<http::string_body>;


// ��������� ContentType ����� ������� ��������� ��� ��������,
// �������� �������� HTTP-��������� Content-Type
struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    // ��� ������������� ������ ContentType ����� �������� � ������ ���� ��������
};

// ������ StringResponse � ��������� �����������
StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                    bool keep_alive,
                                    std::string_view content_type = ContentType::TEXT_HTML) {
    StringResponse response(status, http_version);
    response.set(http::field::content_type, content_type);
    response.body() = body;
    response.content_length(body.size());
    response.keep_alive(keep_alive);
    return response;
}

StringResponse HandleRequest(StringRequest&& req) {
    const auto text_response = [&req](http::status status, std::string_view text) {
        return MakeStringResponse(status, text, req.version(), req.keep_alive());
    };

    // ����� ����� ���������� ������ � ������������ �����, �� ���� ������ ��������: Hello
    return text_response(http::status::ok, "<strong>Hello</strong>"sv);
}

std::optional<StringRequest> ReadRequest(tcp::socket& socket, beast::flat_buffer& buffer) {
    beast::error_code ec;
    StringRequest req;
    // ��������� �� socket ������ req, ��������� buffer ��� �������� ������.
    // � ec ������� ������� ��� ������.
    http::read(socket, buffer, req, ec);

    if (ec == http::error::end_of_stream) {
        return std::nullopt;
    }

    if (ec) {
        throw std::runtime_error("Failed to read request: "s.append(ec.message()));
    }

    return req;
}

void DumpRequest(const StringRequest& req) {
    std::cout << req.method_string() << ' ' << req.target() << std::endl;
    // ������� ��������� �������
    for (const auto& header : req) {
        std::cout << "  "sv << header.name_string() << ": "sv << header.value() << std::endl;
    }
}

template <typename RequestHandler>
void HandleConnection(tcp::socket& socket, RequestHandler&& handle_request) {
    try {
        // ����� ��� ������ ������ � ������ ������� ������.
        beast::flat_buffer buffer;

        // ���������� ��������� ��������, ���� ������ �� ����������
        while (auto request = ReadRequest(socket, buffer)) {
            // ������������ ������ � ��������� ����� �������
            DumpRequest(*request);
            // ���������� ��������� ������� ������� handle_request
            StringResponse response = handle_request(*std::move(request));
            http::write(socket, response);
            if (response.need_eof()) {
                break;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    beast::error_code ec;
    // ��������� ���������� �������� ������ ����� �����
    socket.shutdown(tcp::socket::shutdown_send, ec);
}

int main() {
    // �������� ��� ���������� ���������� � ����������� �������� �����/������
    net::io_context ioc;
    const auto address = net::ip::make_address("0.0.0.0");
    constexpr unsigned short port = 8080;

    // ������, ����������� ��������� tcp-����������� � ������
    tcp::acceptor acceptor(ioc, { address, port });

    while (true) {
        tcp::socket socket(ioc);
        acceptor.accept(socket);

        // ��������� ��������� �������������� � �������� � ��������� ������
        std::thread t(
            // ������-������� ����� ����������� � ��������� ������
            [](tcp::socket socket) {
                // �������� HandleConnection, ��������� �� �������-���������� �������
                HandleConnection(socket, HandleRequest);
            },
            std::move(socket)); // ����� ������ �����������, �� ����� �����������

        // ����� ������ detach ����� ��������� ����������� ���������� �� ������� t
        t.detach();

    }



    /*std::cout << "Waiting for socket connection"sv << std::endl;
    tcp::socket socket(ioc);
    acceptor.accept(socket);
    std::cout << "Connection received"sv << std::endl;

    constexpr std::string_view response
        = "HTTP/1.1 200 OK\r\n"sv
        "Content-Type: text/plain\r\n\r\n"sv
        "Hello"sv;
    net::write(socket, net::buffer(response));*/
}