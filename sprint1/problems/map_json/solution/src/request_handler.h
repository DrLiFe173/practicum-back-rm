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

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        // Обработать запрос request и отправить ответ, используя send
        http::status response_status;
        std::string response_message;
        std::string target_text(req.target());

        if (target_text.find("/api/") != 0) {                       // условие если нет /api/ части - 400 badRequest
            response_status = http::status::bad_request;
            json::object obj;
            obj["code"] = "badRequest";
            obj["message"] = "Bad request";
            response_message = json::serialize(obj);
        }
        else {                                                      // условие когда можем искать карту искать
            auto find_res = target_text.rfind("/");
            auto mapId = target_text.substr(++find_res);
            if (mapId == "maps"sv) {                                // получаем список всех карт
                response_status = http::status::ok;
                auto maps = game_.GetMaps();
                json::array jsonArr;
                for (auto map : maps) {
                    json::object mapObj;
                    mapObj["id"] = *map.GetId();
                    mapObj["name"] = map.GetName();                    
                    jsonArr.push_back(mapObj);
                }
                response_message = json::serialize(jsonArr);
            }
            else {
                model::Map::Id id{ mapId };
                auto requested_map = game_.FindMap(id);
                if (requested_map != NULL) {
                    response_status = http::status::ok;
                    json::object obj;
                    obj["id"] = *requested_map->GetId();
                    obj["name"] = requested_map->GetName();

                    json::array jsonArr;
                    for (auto road : requested_map->GetRoads()) {
                        model::Point start = road.GetStart();
                        json::object tempObj;
                        tempObj["x0"] = start.x;
                        tempObj["y0"] = start.y;
                        if (road.IsHorizontal()) {
                            tempObj["x1"] = road.GetEnd().x;
                        }
                        else {
                            tempObj["y1"] = road.GetEnd().y;
                        }
                        jsonArr.push_back(tempObj);
                    }
                    obj["roads"] = jsonArr;
                    jsonArr.clear();

                    for (auto building : requested_map->GetBuildings()) {
                        model::Rectangle bound = building.GetBounds();
                        json::object tempObj;
                        tempObj["x"] = bound.position.x;
                        tempObj["y"] = bound.position.y;
                        tempObj["w"] = bound.size.width;
                        tempObj["h"] = bound.size.height;
                        jsonArr.push_back(tempObj);
                    }
                    obj["buildings"] = jsonArr;
                    jsonArr.clear();

                    for (auto office : requested_map->GetOffices()) {
                        json::object tempObj;
                        tempObj["id"] = *office.GetId();
                        tempObj["x"] = office.GetPosition().x;
                        tempObj["y"] = office.GetPosition().y;
                        tempObj["offsetX"] = office.GetOffset().dx;
                        tempObj["offsetY"] = office.GetOffset().dy;
                        jsonArr.push_back(tempObj);
                    }
                    obj["offices"] = jsonArr;
                    jsonArr.clear();

                    response_message = json::serialize(obj);
                }
                else {                                                   // условие если не найдена карта - 404 mapNotFound 
                    response_status = http::status::not_found;
                    json::object obj;
                    obj["code"] = "mapNotFound";
                    obj["message"] = "Map not found";
                    response_message = json::serialize(obj);
                }
            }
        }
        StringResponse response(response_status, req.version());
        response.set(http::field::content_type, ContentType::APP_JSON);
        response.body() = response_message;
        response.content_length(response_message.size());
        response.keep_alive(req.keep_alive());
        send(response);
    }

private:
    model::Game& game_;
};

}  // namespace http_handler
