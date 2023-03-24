#pragma once
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <string_view>
#include <string>
#include <variant>
#include <filesystem>

#include <boost/algorithm/string.hpp>
#include <boost/json.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include "magic_defs.h"
#include "logger.h"

namespace http_handler {

    namespace beast = boost::beast;
    namespace http = beast::http;

    using StringResponse = http::response<http::string_body>;

    using FileResponse = http::response<http::file_body>;

    class Response
    {
    public:
        Response() = delete;

        // Создаёт корректный HttpResponse с заданными параметрами
        static StringResponse Make(http::status status,
            std::string_view body,
            std::string_view content_type = ContentType::APP_JSON,
            std::string_view allow_field = "");
        static StringResponse MakeJSON(http::status status, std::string_view code, std::string_view message, std::string_view content_type = ContentType::APP_JSON);

        static StringResponse MakeUnauthorizedErrorInvalidToken();
        static StringResponse MakeUnauthorizedErrorUnknownToken();

        static StringResponse MakeBadRequestInvalidArgument(std::string_view message);
        static StringResponse MakeMethodNotAllowed(std::string_view message, std::string_view allow);

        static StringResponse MakeJoinGame(std::string_view token, std::uint64_t playerId);
        static StringResponse MakePlayersByToken(std::map<std::string, std::string>& players);
        static StringResponse MakeDogssByToken(std::map<std::string, boost::json::object>& dogs);

        static FileResponse MakeFileResponse(const std::filesystem::path& file_path, unsigned http_version, bool isKeepAlive);

        static std::string_view GetGontentType(const std::string& file_name);
    };
}