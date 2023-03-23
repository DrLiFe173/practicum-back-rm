#include "response.h"
#include "magic_defs.h"

namespace http_handler {
    // Создаёт корректный HttpResponse с заданными параметрами
    StringResponse Response::Make(http::status status,
        std::string_view body,
        std::string_view content_type,
        std::string_view allow_field)
    {
        StringResponse response;
        response.result(status);
        response.set(http::field::content_type, content_type);
        response.set(http::field::cache_control, MiscDefs::NO_CACHE);
        if (allow_field.size()) response.set(http::field::allow, allow_field);
        response.body() = body;
        response.content_length(body.size());
        return response;
    }

    StringResponse Response::MakeJSON(http::status status, std::string_view code, std::string_view message, std::string_view content_type)
    {
        boost::json::value jv = {
            { JsonField::CODE , code },
            { JsonField::MESSAGE, message }
        };

        return Make(status, boost::json::serialize(jv), content_type);
    }

    StringResponse Response::MakeUnauthorizedErrorInvalidToken()
    {
        return MakeJSON(http::status::unauthorized,
            ErrorCode::INVALID_TOKEN,
            ErrorMessage::INVALID_TOKEN);
    }

    StringResponse Response::MakeUnauthorizedErrorUnknownToken()
    {
        return MakeJSON(http::status::unauthorized,
            ErrorCode::UNKNOWN_TOKEN,
            ErrorMessage::UNKNOWN_TOKEN);
    }

    StringResponse Response::MakeBadRequestInvalidArgument(std::string_view message)
    {
        return MakeJSON(http::status::bad_request,
            ErrorCode::INVALID_ARGUMENT,
            message);
    }

    StringResponse Response::MakeMethodNotAllowed(std::string_view message, std::string_view allow)
    {
        boost::json::value jv = {
            { JsonField::CODE , ErrorCode::INVALID_METHOD },
            { JsonField::MESSAGE, message }
        };

        return Response::Make(http::status::method_not_allowed, boost::json::serialize(jv), ContentType::APP_JSON, allow);
    }
    StringResponse Response::MakeJoinGame(std::string_view token, std::uint64_t playerId)
    {
        boost::json::value jv = {
            { JsonField::AUTH_TOKEN , token },
            { JsonField::PLAYER_ID, playerId }
        };

        return Response::Make(http::status::ok, boost::json::serialize(jv), ContentType::APP_JSON);
    }
    StringResponse Response::MakePlayersByToken(std::map<std::string, std::string>& players)
    {
        boost::json::object json;
        for (auto player : players) {
            boost::json::object js;
            js["name"] = player.second;
            json[player.first] = js;
        }
        return Response::Make(http::status::ok, boost::json::serialize(json), ContentType::APP_JSON);
    }
    FileResponse Response::MakeFileResponse(const std::filesystem::path& file_path, unsigned http_version, bool isKeepAlive)
    {
        http::status status = (http::status::ok);
        FileResponse response(status, http_version);

        std::string file_name = file_path.filename().string();

        response.insert(http::field::content_type, GetGontentType(file_name));

        http::file_body::value_type file;
        if (boost::system::error_code ec; file.open(file_path.string().c_str(), beast::file_mode::read, ec), ec) {
            std::string message = "Failed to open file "s;
            message.append(file_path.string());
            std::string place = "request_handler"s;
            Logger::LogServerError(message, place);
        }
        response.body() = std::move(file);
        response.prepare_payload();

        return response;
    }

    std::string_view Response::GetGontentType(const std::string& file_name) {
        auto find_res = file_name.rfind(".");
        std::string file_extension = file_name.substr(find_res);

        if (boost::iequals(file_extension, ContentType::HTM) || boost::iequals(file_extension, ContentType::HTML)) {
            return ContentType::TEXT_HTML;
        }
        else if (boost::iequals(file_extension, ContentType::CSS)) {
            return ContentType::TEXT_CSS;
        }
        else if (boost::iequals(file_extension, ContentType::TXT)) {
            return ContentType::TEXT_PLAIN;
        }
        else if (boost::iequals(file_extension, ContentType::JAVASCRIPT)) {
            return ContentType::TEXT_JAVASCRIPT;
        }
        else if (boost::iequals(file_extension, ContentType::JSON)) {
            return ContentType::APP_JSON;
        }
        else if (boost::iequals(file_extension, ContentType::XML)) {
            return ContentType::APP_XML;
        }
        else if (boost::iequals(file_extension, ContentType::PNG)) {
            return ContentType::IMAGE_PNG;
        }
        else if (boost::iequals(file_extension, ContentType::JPE) || boost::iequals(file_extension, ContentType::JPEG) || boost::iequals(file_extension, ContentType::JPG)) {
            return ContentType::IMAGE_JPG;
        }
        else if (boost::iequals(file_extension, ContentType::GIF)) {
            return ContentType::IMAGE_GIF;
        }
        else if (boost::iequals(file_extension, ContentType::BMP)) {
            return ContentType::IMAGE_BMP;
        }
        else if (boost::iequals(file_extension, ContentType::ICO)) {
            return ContentType::IMAGE_ICO;
        }
        else if (boost::iequals(file_extension, ContentType::TIF) || boost::iequals(file_extension, ContentType::TIFF)) {
            return ContentType::IMAGE_TIF;
        }
        else if (boost::iequals(file_extension, ContentType::SVG) || boost::iequals(file_extension, ContentType::SVGZ)) {
            return ContentType::IMAGE_SVG;
        }
        else if (boost::iequals(file_extension, ContentType::MP3)) {
            return ContentType::AUDIO_MP3;
        }
        else {
            return ContentType::APP_BYTE;
        }
    }
}