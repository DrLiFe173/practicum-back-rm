#pragma once
#define BOOST_BEAST_USE_STD_STRING_VIEW
#include <string_view>
#include <string>
using namespace std::literals;

struct TokenMessage
{
    static inline constexpr std::string_view BEARER = "Bearer "sv;
};

struct ServerMessage
{
    static inline constexpr std::string_view START = "Server has started..."sv;
    static inline constexpr std::string_view EXIT = "server exited"sv;
    static inline constexpr std::string_view ERROR_ARGS = "Usage: game_server <game-config-json>"sv;
};

struct ServerAction
{
    static inline constexpr std::string_view READ = "read"sv;
    static inline constexpr std::string_view WRITE = "write"sv;
    static inline constexpr std::string_view ACCEPT = "accept"sv;
};

struct JsonField
{
    static inline const std::string CODE = "code"s;
    static inline const std::string MESSAGE = "message"s;
    static inline const std::string AUTH_TOKEN = "authToken"s;
    static inline const std::string PLAYER_ID = "playerId"s;
};

struct ServerParam
{
    static inline constexpr std::string_view ADDR = "0.0.0.0"sv;
    static inline const uint32_t PORT = 8080;
};

struct ErrorCode
{
    static inline constexpr std::string_view BAD_REQUEST = "badRequest"sv;
    static inline constexpr std::string_view INVALID_METHOD = "invalidMethod"sv;
    static inline constexpr std::string_view INVALID_ARGUMENT = "invalidArgument"sv;
    static inline constexpr std::string_view INVALID_TOKEN = "invalidToken"sv;
    static inline constexpr std::string_view UNKNOWN_TOKEN = "unknownToken"sv;
    static inline constexpr std::string_view MAP_NOT_FOUND = "mapNotFound"sv;
    static inline constexpr std::string_view FILE_NOT_FOUND = "fileNotFound"sv;
};

struct ErrorMessage
{
    static inline constexpr std::string_view BAD_REQUEST = "Bad request"sv;
    static inline constexpr std::string_view INVALID_ENDPOINT = "Invalid endpoint"sv;
    static inline constexpr std::string_view POST_IS_EXPECTED = "Only POST method is expected"sv;
    static inline constexpr std::string_view GET_IS_EXPECTED = "Only GET method is expected"sv;
    static inline constexpr std::string_view INVALID_TOKEN = "Authorization header is missing"sv;
    static inline constexpr std::string_view UNKNOWN_TOKEN = "Player token has not been found"sv;
    static inline constexpr std::string_view MAP_NOT_FOUND = "Map not found"sv;
    static inline constexpr std::string_view FILE_NOT_FOUND = "File not found"sv;
    static inline constexpr std::string_view NO_ACCESS = "No access"sv;
    static inline constexpr std::string_view PARSE_ERROR = "Join game request parse error"sv;
    static inline constexpr std::string_view INVALID_NAME = "Invalid name"sv;
    static inline constexpr std::string_view INVALID_METHOD = "Invalid method"sv;
};

struct MiscDefs
{
    static inline constexpr std::string_view NO_CACHE = "no-cache"sv;
};

struct MiscMessage
{
    static inline constexpr std::string_view ALLOWED_POST_METHOD = "POST"sv;
    static inline constexpr std::string_view ALLOWED_GET_HEAD_METHOD = "GET, HEAD"sv;
    static inline constexpr std::string_view ALLOWED_GET_METHOD = "GET"sv;
    static inline constexpr std::string_view ALLOWED_HEAD_METHOD = "HEAD"sv;
};

struct Endpoint
{
    static inline constexpr std::string_view API = "/api/"sv;
    static inline constexpr std::string_view MAPS = "/api/v1/maps"sv;
    static inline constexpr std::string_view GAME = "/api/v1/game/"sv;
    static inline constexpr std::string_view JOIN_GAME = "/api/v1/game/join"sv;
    static inline constexpr std::string_view PLAYERS_LIST = "/api/v1/game/players"sv;
};

struct ContentType 
{
    static inline constexpr std::string_view TEXT_HTML         = "text/html"sv;
    static inline constexpr std::string_view TEXT_CSS          = "text/css"sv;
    static inline constexpr std::string_view TEXT_PLAIN        = "text/plain"sv;
    static inline constexpr std::string_view TEXT_JAVASCRIPT   = "text/javascript"sv;
    static inline constexpr std::string_view IMAGE_PNG         = "image/png"sv;
    static inline constexpr std::string_view IMAGE_JPG         = "image/jpeg"sv;
    static inline constexpr std::string_view IMAGE_GIF         = "image/gif"sv;
    static inline constexpr std::string_view IMAGE_BMP         = "image/bmp"sv;
    static inline constexpr std::string_view IMAGE_ICO         = "image/vnd.microsoft.icon"sv;
    static inline constexpr std::string_view IMAGE_TIF         = "image/tiff"sv;
    static inline constexpr std::string_view IMAGE_SVG         = "image/svg+xml"sv;
    static inline constexpr std::string_view AUDIO_MP3         = "audio/mpeg"sv;
    static inline constexpr std::string_view APP_JSON          = "application/json"sv;
    static inline constexpr std::string_view APP_XML           = "application/xml"sv;
    static inline constexpr std::string_view APP_BYTE          = "application/octet-stream"sv;

    static inline constexpr std::string_view HTML              = ".html"sv;
    static inline constexpr std::string_view HTM               = ".htm"sv;
    static inline constexpr std::string_view CSS               = ".css"sv;
    static inline constexpr std::string_view TXT               = ".txt"sv;
    static inline constexpr std::string_view JAVASCRIPT        = ".js"sv;
    static inline constexpr std::string_view PNG               = ".png"sv;
    static inline constexpr std::string_view JPG               = ".jpg"sv;
    static inline constexpr std::string_view JPE               = ".jpe"sv;
    static inline constexpr std::string_view JPEG              = ".jpeg"sv;
    static inline constexpr std::string_view GIF               = ".gif"sv;
    static inline constexpr std::string_view BMP               = ".bmp"sv;
    static inline constexpr std::string_view ICO               = ".ico"sv;
    static inline constexpr std::string_view TIFF              = ".tiff"sv;
    static inline constexpr std::string_view TIF               = ".tif"sv;
    static inline constexpr std::string_view SVG               = ".svg"sv;
    static inline constexpr std::string_view SVGZ              = ".svgz"sv;
    static inline constexpr std::string_view MP3               = ".mp3"sv;
    static inline constexpr std::string_view JSON              = ".json"sv;
    static inline constexpr std::string_view XML               = ".xml"sv;
};

