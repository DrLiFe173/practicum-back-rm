#pragma once
#include <boost/asio.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <variant>

#include "model.h"
#include "magic_defs.h"
#include "response.h"
#include "url_handler.h"
#include "logger.h"

namespace http_handler {
	namespace beast = boost::beast;
	namespace http = beast::http;
	namespace json = boost::json;

    using namespace std::literals;

	using StringResponse = http::response<http::string_body>;
	class ApiRequestHandler
	{
	public:
		explicit ApiRequestHandler(model::Game& game)
			: game_{ game } {
		}

		bool IsApiRequest(const std::string requestTarget)
		{
			return requestTarget.starts_with("/api/"sv);
		}

		bool IsMapRequest(const std::string requestTarget) {
			return requestTarget.starts_with(Endpoint::MAPS);
		}

		bool IsApiVersionCorrect(const std::string requestTarget) {
			return requestTarget.substr(4, 4) == "/v1/"sv ? true : false;
		}

		bool IsJoinGameRequest(const std::string requestTarget) {
			return requestTarget.starts_with(Endpoint::JOIN_GAME);
		}

		bool IsPlayerListRequest(const std::string requestTarget) {
			return requestTarget.starts_with(Endpoint::PLAYERS_LIST);
		}

        bool IsGameStateRequest(const std::string requestTarget) {
            return requestTarget.starts_with(Endpoint::GAME_STATE);
        }

        bool IsPlayerActionRequest(const std::string requestTarget) {
            return requestTarget.starts_with(Endpoint::PLAYER_ACTION);
        }

        template <typename Fn>
        StringResponse ExecuteAuthorized(const std::string_view& request, Fn&& action) {
            if (auto token = TryExtractToken(request)) {
                if (game_.GetPlayerTokens().IsTokenExist(token.value())) {
                    return action(token.value());
                }
                else {
                    return Response::MakeUnauthorizedErrorUnknownToken();
                }
            }
            else {
                return Response::MakeUnauthorizedErrorInvalidToken();
            }
        }

        std::optional<std::string> TryExtractToken(const std::string_view& request);

        template <typename Body, typename Allocator>
		StringResponse ProceedApiRequest(const http::request<Body, http::basic_fields<Allocator>>& req) {
            std::string body;
            StringResponse response;
            std::string target_text(req.target());
            std::string decoded_request = UrlHandler::UrlDecode(target_text);

            if (IsApiRequest(decoded_request)) {
                if (IsApiVersionCorrect(decoded_request)) {
                    if (IsMapRequest(decoded_request)) {
                        std::string mapId = GetMapIdFromRequestTarget(decoded_request);
                        if (mapId == "maps"sv) {
                            AddAllMapsInfo(body);
                            response = Response::Make(http::status::ok, body, ContentType::APP_JSON);
                        }
                        else {
                            model::Map::Id id{ mapId };
                            auto requested_map = game_.FindMap(id);
                            if (requested_map != NULL) {
                                AddMapInfo(body, mapId);
                                response = Response::Make(http::status::ok, body, ContentType::APP_JSON);
                            }
                            else {
                                response = Response::MakeJSON(http::status::not_found, ErrorCode::MAP_NOT_FOUND, ErrorMessage::MAP_NOT_FOUND);
                            }
                        }
                    }
                    else if (IsJoinGameRequest(decoded_request)) {                                              
                        if (req.method_string() == MiscMessage::ALLOWED_POST_METHOD) {
                            std::string body = req.body();
                            response = ProceedJoinGameRequest(body);
                        }
                        else {
                            response = Response::MakeMethodNotAllowed(ErrorMessage::POST_IS_EXPECTED, MiscMessage::ALLOWED_POST_METHOD);
                        }                            
                    }
                    else if (IsPlayerListRequest(decoded_request)) {
                        if (req.method_string() == MiscMessage::ALLOWED_HEAD_METHOD || req.method_string() == MiscMessage::ALLOWED_GET_METHOD) {
                            auto tokenValue = req.base()[boost::beast::http::field::authorization];                        
                            response = ProceedPlayerListRequest(tokenValue);
                        }
                        else {
                            response = Response::MakeMethodNotAllowed(ErrorMessage::INVALID_METHOD, MiscMessage::ALLOWED_GET_HEAD_METHOD);
                        }                        
                    }
                    else if (IsGameStateRequest(decoded_request)) {
                        if (req.method_string() == MiscMessage::ALLOWED_HEAD_METHOD || req.method_string() == MiscMessage::ALLOWED_GET_METHOD) {
                            auto tokenValue = req.base()[boost::beast::http::field::authorization];
                            response = ProceedGameStateRequest(tokenValue);
                        }
                        else {
                            response = Response::MakeMethodNotAllowed(ErrorMessage::INVALID_METHOD, MiscMessage::ALLOWED_GET_HEAD_METHOD);
                        }                        
                    }
                    else if (IsPlayerActionRequest(decoded_request)) {
                        auto contentType = req.base()[boost::beast::http::field::content_type];
                        if (!contentType.empty() && contentType.starts_with(ContentType::APP_JSON)) {
                            if (req.method_string() == MiscMessage::ALLOWED_POST_METHOD) {
                                auto tokenValue = req.base()[boost::beast::http::field::authorization];
                                std::string body = req.body();
                                response = ProceedPlayerActionRequest(tokenValue, body);
                            }
                            else {
                                response = Response::MakeMethodNotAllowed(ErrorMessage::POST_IS_EXPECTED, MiscMessage::ALLOWED_POST_METHOD);
                            }
                        }
                        else {
                            response = Response::MakeJSON(http::status::bad_request, ErrorCode::INVALID_ARGUMENT, ErrorMessage::INVALID_CONTENT_TYPE);
                        }                       
                    }
                }
                else {
                    response = Response::MakeJSON(http::status::bad_request, ErrorCode::BAD_REQUEST, ErrorMessage::BAD_REQUEST);                    
                }
            }
            return response;
		}

        std::string GetMapIdFromRequestTarget(const std::string requestTarget);

        void AddAllMapsInfo(std::string& body);

        void AddRoadsInfo(const model::Map* map, json::object& object);

        void AddBuildingsInfo(const model::Map* map, json::object& object);

		void AddOfficesInfo(const model::Map* map, json::object& object);

		void AddMapInfo(std::string& body, std::string& mapId);

        StringResponse ProceedJoinGameRequest(std::string& body);

        StringResponse ProceedPlayerListRequest(std::string_view& token);

        StringResponse ProceedGameStateRequest(std::string_view& token);

        StringResponse ProceedPlayerActionRequest(std::string_view& token, std::string& body);

        model::Game& GetGameInstanse() {
            return game_;
        }

	private:
		model::Game& game_;
	};

}// namespace http_handler