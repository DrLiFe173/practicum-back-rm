#pragma once
#include "http_server.h"
#include "model.h"
#include <iostream>
#include <boost/json.hpp>

namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;
using namespace std::literals;

// Запрос, тело которого представлено в виде строки
using StringRequest = http::request<http::string_body>;
// Ответ, тело которого представлено в виде строки
using StringResponse = http::response<http::string_body>;

struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    constexpr static std::string_view APP_JSON = "application/json"sv;
    // При необходимости внутрь ContentType можно добавить и другие типы контента
};

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game)
        : game_{game} {
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    std::string GetMapIdFromRequestTarget(const std::string requestTarget);

    void SetResponceStatus(const std::string requestTarget, http::status& status);

    void CreateErrorResponce(std::string& body, const std::string& code, const std::string& message);

    void AddAllMapsInfo(std::string& body);

    void AddRoadsInfo(const model::Map* map, json::object& object);

    void AddBuildingsInfo(const model::Map* map, json::object& object);

    void AddOfficesInfo(const model::Map* map, json::object& object);

    void AddMapInfo(std::string& body, std::string& mapId);

    void MakeStringBody(const http::status status, std::string& body, std::string& mapId);

    StringResponse MakeStringResponce(const std::string requestTarget, unsigned http_version, bool isKeepAlive);

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        // Обработать запрос request и отправить ответ, используя send

        std::string target_text(req.target());
        StringResponse response = MakeStringResponce(target_text, req.version(), req.keep_alive());
        send(response);
    }

private:
    model::Game& game_;
};

}  // namespace http_handler
