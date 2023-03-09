#pragma once
#include "http_server.h"
#include "model.h"
#include <iostream>
#include <boost/json.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include <filesystem>
#include <fstream>
#include <variant>

namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;
namespace fs = std::filesystem;
using namespace std::literals;

// Запрос, тело которого представлено в виде строки
using StringRequest = http::request<http::string_body>;
// Ответ, тело которого представлено в виде строки
using StringResponse = http::response<http::string_body>;

using FileResponse = http::response<http::file_body>;

struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML         = "text/html"sv;
    constexpr static std::string_view TEXT_CSS          = "text/css"sv;
    constexpr static std::string_view TEXT_PLAIN        = "text/plain"sv;
    constexpr static std::string_view TEXT_JAVASCRIPT   = "text/javascript"sv;
    constexpr static std::string_view IMAGE_PNG         = "image/png"sv;
    constexpr static std::string_view IMAGE_JPG         = "image/jpeg"sv;
    constexpr static std::string_view IMAGE_GIF         = "image/gif"sv;
    constexpr static std::string_view IMAGE_BMP         = "image/bmp"sv;
    constexpr static std::string_view IMAGE_ICO         = "image/vnd.microsoft.icon"sv;
    constexpr static std::string_view IMAGE_TIF         = "image/tiff"sv;
    constexpr static std::string_view IMAGE_SVG         = "image/svg+xml"sv;
    constexpr static std::string_view AUDIO_MP3         = "audio/mpeg"sv;
    constexpr static std::string_view APP_JSON          = "application/json"sv;
    constexpr static std::string_view APP_XML           = "application/xml"sv;
    constexpr static std::string_view APP_BYTE          = "application/octet-stream"sv;
    
    constexpr static std::string_view HTML              = ".html"sv;
    constexpr static std::string_view HTM               = ".htm"sv;
    constexpr static std::string_view CSS               = ".css"sv;
    constexpr static std::string_view TXT               = ".txt"sv;
    constexpr static std::string_view JAVASCRIPT        = ".js"sv;
    constexpr static std::string_view PNG               = ".png"sv;
    constexpr static std::string_view JPG               = ".jpg"sv;
    constexpr static std::string_view JPE               = ".jpe"sv;
    constexpr static std::string_view JPEG              = ".jpeg"sv;
    constexpr static std::string_view GIF               = ".gif"sv;
    constexpr static std::string_view BMP               = ".bmp"sv;
    constexpr static std::string_view ICO               = ".ico"sv;
    constexpr static std::string_view TIFF              = ".tiff"sv;
    constexpr static std::string_view TIF               = ".tif"sv;
    constexpr static std::string_view SVG               = ".svg"sv;
    constexpr static std::string_view SVGZ              = ".svgz"sv;
    constexpr static std::string_view MP3               = ".mp3"sv;
    constexpr static std::string_view JSON              = ".json"sv;
    constexpr static std::string_view XML               = ".xml"sv;
};

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game)
        : game_{game} {
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    bool IsMapRequest(const std::string requestTarget);

    bool IsApiRequest(const std::string requestTarget);

    bool IsApiVersionCorrect(const std::string requestTarget);

    std::string GetMapIdFromRequestTarget(const std::string requestTarget);

    void SetResponceStatus(const std::string requestTarget, http::status& status);  // remove?

    void CreateErrorResponce(std::string& body, const std::string& code, const std::string& message);

    void AddAllMapsInfo(std::string& body);

    void AddRoadsInfo(const model::Map* map, json::object& object);

    void AddBuildingsInfo(const model::Map* map, json::object& object);

    void AddOfficesInfo(const model::Map* map, json::object& object);

    void AddMapInfo(std::string& body, std::string& mapId);

    void MakeStringBody(const http::status status, std::string& body, std::string& mapId); // remove?

    std::string_view GetGontentType(const std::string& file_name);

    std::string GetFilePath(const std::string& requestTarget);

    std::string GetFileName(const std::string& file_path);

    StringResponse MakeStringResponce(const std::string requestTarget, unsigned http_version, bool isKeepAlive); // remove?

    FileResponse MakeFileResponce(const std::string& file_path, unsigned http_version, bool isKeepAlive);

    StringResponse PrepareStringResponce(const std::string requestTarget, unsigned http_version, bool isKeepAlive);

    std::string UrlDecode(const std::string& input);

    bool IsInFileRootFolder(const std::string& file_path);

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        // Обработать запрос request и отправить ответ, используя send

        std::string target_text(req.target());
        std::string decoded_request = UrlDecode(target_text);

        if (IsApiRequest(decoded_request.c_str())) {
            if (IsMapRequest(decoded_request)) {
                StringResponse response = PrepareStringResponce(decoded_request, req.version(), req.keep_alive());
                send(response);
            }
        }
        else {
            std::string file_path = GetFilePath(decoded_request);
            if (IsFileExist(file_path)) {
                FileResponse response = MakeFileResponce(file_path, req.version(), req.keep_alive());
                send(response);
            }
            else {
                StringResponse response = PrepareStringResponce(decoded_request, req.version(), req.keep_alive());
                send(response);
            }            
        }   
    }

    bool IsFileExist(const std::string& path) {
        return std::filesystem::exists(path);
    }

    void SetRootFolderPath(const char* path) {
        //fileRootFolder = "../";
        //fileRootFolder.append(path);
        fileRootFolder = path;
    }

private:
    model::Game& game_;
    std::string fileRootFolder;
};

}  // namespace http_handler
