#pragma once
#define BOOST_BEAST_USE_STD_STRING_VIEW
#include <string_view>
#include <string>
using namespace std::literals;

struct TokenMessage
{
    static inline constexpr std::string_view BEARER     = "Bearer "sv;
};

struct GameDefs
{
    static inline constexpr std::string_view NORTH          = "U"sv;
    static inline constexpr std::string_view SOUTH          = "D"sv;
    static inline constexpr std::string_view EAST           = "R"sv;
    static inline constexpr std::string_view WEST           = "L"sv;

    static inline const uint32_t LOW_DIRECTION_BOUND        = 0;
    static inline const uint32_t UP_DIRECTION_BOUND         = 3;
    static inline const uint32_t TOKEN_SIZE                 = 32;
    static inline const uint32_t TOKEN_SIZE_BAD_GENERATION  = 31;

    static inline const uint32_t ZERO_INT                   = 0;
    static inline const uint32_t TEN                        = 10;

    static inline const double ROAD_OFFSET                  = 0.4;
    static inline const double EMPTY_CROSSROAD              = 0.5;
    static inline const double TICK_DIVIDER                 = 1000.0;

    static inline const double ZERO                         = 0.0;
    static inline const double MINUS_ONE                    = -1.0;
};

struct ConfigDefs
{
    static inline const std::string DEF_SPEED           = "defaultDogSpeed"s;
    static inline const std::string MAP_SPEED           = "dogSpeed"s;
    static inline const std::string MAPS                = "maps"s;
    static inline const std::string ID                  = "id"s;
    static inline const std::string NAME                = "name"s;
    static inline const std::string ROADS               = "roads"s;
    static inline const std::string BUILDINGS           = "buildings"s;
    static inline const std::string OFFICES             = "offices"s;
    static inline const std::string X                   = "x"s;
    static inline const std::string Y                   = "y"s;
    static inline const std::string X0                  = "x0"s;
    static inline const std::string Y0                  = "y0"s;
    static inline const std::string X1                  = "x1"s;
    static inline const std::string Y1                  = "y1"s;
    static inline const std::string W                   = "w"s;
    static inline const std::string H                   = "h"s;
    static inline const std::string OFFSET_X            = "offsetX"s;
    static inline const std::string OFFSET_Y            = "offsetY"s;
};

struct LoggerDefs
{
    static inline const std::string IP                  = "ip"s;
    static inline const std::string RESP_TIME           = "response_time"s;
    static inline const std::string CODE                = "code"s;
    static inline const std::string CONTENT_TYPE        = "content_type"s;
    static inline const std::string RESP_SENT           = "response sent"s;
    static inline const std::string URI                 = "URI"s;
    static inline const std::string METHOD              = "method"s;
    static inline const std::string REQ_RECIEVED        = "request received"s;
    static inline const std::string EXCEPTION           = "exception"s;
    static inline const std::string PORT                = "port"s;
    static inline const std::string ADDRESS             = "address"s;
    static inline const std::string WHERE               = "where"s;
    static inline const std::string ERR                 = "error"s;
    static inline const std::string INFO                = "info"s;
};

struct ServerMessage
{
    static inline constexpr std::string_view START          = "server started"sv;
    static inline constexpr std::string_view EXIT           = "server exited"sv;
    static inline constexpr std::string_view ERROR_ARGS     = "Usage: game_server <game-config-json>"sv;
    static inline constexpr std::string_view OPEN_FILE_FAIL = "Failed to open file"sv;
    static inline constexpr std::string_view JSON_LOAD_FAIL = "Failed to load config JSON file"sv;
};

struct ServerAction
{
    static inline constexpr std::string_view READ       = "read"sv;
    static inline constexpr std::string_view WRITE      = "write"sv;
    static inline constexpr std::string_view ACCEPT     = "accept"sv;
};

struct JsonField
{
    static inline const std::string CODE                = "code"s;
    static inline const std::string MESSAGE             = "message"s;
    static inline const std::string AUTH_TOKEN          = "authToken"s;
    static inline const std::string PLAYER_ID           = "playerId"s;
    static inline const std::string PLAYERS             = "players"s;
    static inline const std::string MOVE                = "move"s;
    static inline const std::string USER_NAME           = "userName"s;
    static inline const std::string MAP_ID              = "mapId"s;
    static inline const std::string NAME                = "name"s;
    static inline const std::string TIME_DELTA          = "timeDelta"s;
};

struct ServerParam
{
    static inline constexpr std::string_view ADDR       = "0.0.0.0"sv;
    static inline const uint32_t PORT                   = 8080;
};

struct ErrorCode
{
    static inline constexpr std::string_view BAD_REQUEST        = "badRequest"sv;
    static inline constexpr std::string_view INVALID_METHOD     = "invalidMethod"sv;
    static inline constexpr std::string_view INVALID_ARGUMENT   = "invalidArgument"sv;
    static inline constexpr std::string_view INVALID_TOKEN      = "invalidToken"sv;
    static inline constexpr std::string_view UNKNOWN_TOKEN      = "unknownToken"sv;
    static inline constexpr std::string_view MAP_NOT_FOUND      = "mapNotFound"sv;
    static inline constexpr std::string_view FILE_NOT_FOUND     = "fileNotFound"sv;
};

struct ErrorMessage
{
    static inline constexpr std::string_view BAD_REQUEST            = "Bad request"sv;
    static inline constexpr std::string_view INVALID_ENDPOINT       = "Invalid endpoint"sv;
    static inline constexpr std::string_view POST_IS_EXPECTED       = "Only POST method is expected"sv;
    static inline constexpr std::string_view GET_IS_EXPECTED        = "Only GET method is expected"sv;
    static inline constexpr std::string_view INVALID_TOKEN          = "Authorization header is missing"sv;
    static inline constexpr std::string_view UNKNOWN_TOKEN          = "Player token has not been found"sv;
    static inline constexpr std::string_view MAP_NOT_FOUND          = "Map not found"sv;
    static inline constexpr std::string_view FILE_NOT_FOUND         = "File not found"sv;
    static inline constexpr std::string_view NO_ACCESS              = "No access"sv;
    static inline constexpr std::string_view JOIN_GAME_PARSE_ERROR  = "Join game request parse error"sv;
    static inline constexpr std::string_view GAME_ACTION_PARSE_ERROR= "Failed to parse action"sv;
    static inline constexpr std::string_view INVALID_NAME           = "Invalid name"sv;
    static inline constexpr std::string_view INVALID_METHOD         = "Invalid method"sv;
    static inline constexpr std::string_view INVALID_CONTENT_TYPE   = "Invalid content type"sv;
};

struct MiscDefs
{
    static inline constexpr std::string_view NO_CACHE                   = "no-cache"sv;
};

struct MiscMessage
{
    static inline constexpr std::string_view ALLOWED_POST_METHOD        = "POST"sv;
    static inline constexpr std::string_view ALLOWED_GET_HEAD_METHOD    = "GET, HEAD"sv;
    static inline constexpr std::string_view ALLOWED_GET_METHOD         = "GET"sv;
    static inline constexpr std::string_view ALLOWED_HEAD_METHOD        = "HEAD"sv;
};

struct Endpoint
{
    static inline constexpr std::string_view API                = "/api/"sv;
    static inline constexpr std::string_view VERSION            = "/v1/"sv;
    static inline constexpr std::string_view MAPS               = "/api/v1/maps"sv;
    static inline constexpr std::string_view GAME               = "/api/v1/game/"sv;
    static inline constexpr std::string_view JOIN_GAME          = "/api/v1/game/join"sv;
    static inline constexpr std::string_view PLAYERS_LIST       = "/api/v1/game/players"sv;
    static inline constexpr std::string_view GAME_STATE         = "/api/v1/game/state"sv;
    static inline constexpr std::string_view PLAYER_ACTION      = "/api/v1/game/player/action"sv;
    static inline constexpr std::string_view GAME_TICK          = "/api/v1/game/tick"sv;
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

