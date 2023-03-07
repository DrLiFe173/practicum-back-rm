#include "request_handler.h"

namespace http_handler {

    void RequestHandler::SetResponceStatus(const std::string requestTarget, http::status& status) {
        if (requestTarget.find("/api/v1/") != 0) {
            status = http::status::bad_request;
        }
        else {
            std::string mapId = GetMapIdFromRequestTarget(requestTarget);
            if (mapId == "maps"sv) {
                status = http::status::ok;
            }
            else {
                model::Map::Id id{ mapId };
                auto requested_map = game_.FindMap(id);
                if (requested_map != NULL) {
                    status = http::status::ok;
                }
                else {
                    status = http::status::not_found;
                }
            }
        }
    }

    std::string RequestHandler::GetMapIdFromRequestTarget(const std::string requestTarget) {
        auto find_res = requestTarget.rfind("/");
        std::string mapId = requestTarget.substr(++find_res);
        return mapId;
    }

    void RequestHandler::CreateErrorResponce(std::string& body, const std::string& code, const std::string& message) {
        json::object obj;
        obj["code"] = code;
        obj["message"] = message;
        body = json::serialize(obj);
    }

    void RequestHandler::AddAllMapsInfo(std::string& body) {
        auto maps = game_.GetMaps();
        json::array jsonArr;
        for (auto map : maps) {
            json::object mapObj;
            mapObj["id"] = *map.GetId();
            mapObj["name"] = map.GetName();
            jsonArr.push_back(mapObj);
        }
        body = json::serialize(jsonArr);
    }

    void RequestHandler::AddRoadsInfo(const model::Map* map, json::object& object) {
        json::array jsonArr;
        for (auto road : map->GetRoads()) {
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
        object["roads"] = jsonArr;
    }

    void RequestHandler::AddBuildingsInfo(const model::Map* map, json::object& object) {
        json::array jsonArr;
        for (auto building : map->GetBuildings()) {
            model::Rectangle bound = building.GetBounds();
            json::object tempObj;
            tempObj["x"] = bound.position.x;
            tempObj["y"] = bound.position.y;
            tempObj["w"] = bound.size.width;
            tempObj["h"] = bound.size.height;
            jsonArr.push_back(tempObj);
        }
        object["buildings"] = jsonArr;
    }

    void RequestHandler::AddOfficesInfo(const model::Map* map, json::object& object) {
        json::array jsonArr;
        for (auto office : map->GetOffices()) {
            json::object tempObj;
            tempObj["id"] = *office.GetId();
            tempObj["x"] = office.GetPosition().x;
            tempObj["y"] = office.GetPosition().y;
            tempObj["offsetX"] = office.GetOffset().dx;
            tempObj["offsetY"] = office.GetOffset().dy;
            jsonArr.push_back(tempObj);
        }
        object["offices"] = jsonArr;
    }

    void RequestHandler::AddMapInfo(std::string& body, std::string& mapId) {
        model::Map::Id id{ mapId };
        auto requested_map = game_.FindMap(id);
        if (requested_map != NULL) {
            json::object obj;
            obj["id"] = *requested_map->GetId();
            obj["name"] = requested_map->GetName();

            AddRoadsInfo(requested_map, obj);
            AddBuildingsInfo(requested_map, obj);
            AddOfficesInfo(requested_map, obj);

            body = json::serialize(obj);
        }
    }

    void RequestHandler::MakeStringBody(const http::status status, std::string& body, std::string& mapId) {
        if (status == http::status::bad_request) {
            CreateErrorResponce(body, "badRequest", "Bad request");
        }
        else if (status == http::status::not_found) {
            CreateErrorResponce(body, "mapNotFound", "Map not found");
        }
        else if (status == http::status::ok) {
            if (mapId == "maps"sv) {
                AddAllMapsInfo(body);
            }
            else {
                AddMapInfo(body, mapId);
            }
        }
    }

    StringResponse RequestHandler::MakeStringResponce(const std::string requestTarget, unsigned http_version, bool isKeepAlive) {

        http::status status;
        std::string body;
        SetResponceStatus(requestTarget, status);
        std::string mapId = GetMapIdFromRequestTarget(requestTarget);
        MakeStringBody(status, body, mapId);

        StringResponse response(status, http_version);
        response.set(http::field::content_type, ContentType::APP_JSON);
        response.body() = body;
        response.content_length(body.size());
        response.keep_alive(isKeepAlive);

        return response;
    }

}  // namespace http_handler
