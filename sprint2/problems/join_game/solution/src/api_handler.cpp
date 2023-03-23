#include "api_handler.h"

namespace http_handler {	

    std::string ApiRequestHandler::GetMapIdFromRequestTarget(const std::string requestTarget) {
        auto find_res = requestTarget.rfind("/");
        std::string mapId = requestTarget.substr(++find_res);
        return mapId;
    }

    void ApiRequestHandler::AddAllMapsInfo(std::string& body) {
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

    void ApiRequestHandler::AddRoadsInfo(const model::Map* map, json::object& object) {
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

    void ApiRequestHandler::AddBuildingsInfo(const model::Map* map, json::object& object) {
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

    void ApiRequestHandler::AddOfficesInfo(const model::Map* map, json::object& object) {
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

    void ApiRequestHandler::AddMapInfo(std::string& body, std::string& mapId) {
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

    StringResponse ApiRequestHandler::ProceedJoinGameRequest(json::value& jsonValue)
    {
        try {
            std::string mapId = jsonValue.as_object().at("mapId"s).as_string().data();
            std::string userName = jsonValue.as_object().at("userName"s).as_string().data();
            model::Map::Id id{ mapId };
            auto requested_map = game_.FindMap(id);
            if (requested_map != nullptr) {
                if (userName.size() != 0) {
                    std::string token = game_.GetPlayerTokens().GenerateToken();
                    auto existedSession = game_.FindGameSession(id);
                    std::uint64_t playerId;
                    model::Dog dog(userName);
                    if (existedSession != nullptr) {                        
                        playerId = existedSession->GetLastDogIndex();
                        existedSession->AddDog(dog);
                        model::Player player(existedSession, playerId);
                        game_.GetPlayerTokens().AddPlayer(token, player);
                    }
                    else {
                        model::GameSession session(requested_map);                        
                        playerId = session.GetLastDogIndex();
                        session.AddDog(dog);
                        game_.AddGameSession(session);
                        model::Player player(game_.FindGameSession(id), playerId);
                        game_.GetPlayerTokens().AddPlayer(token, player);
                    }                               

                    return Response::MakeJoinGame(token, playerId);
                }
                else {
                    return Response::MakeJSON(http::status::not_found, ErrorCode::INVALID_ARGUMENT, ErrorMessage::INVALID_NAME);
                }
            }
            else {
                return Response::MakeJSON(http::status::not_found, ErrorCode::MAP_NOT_FOUND, ErrorMessage::MAP_NOT_FOUND);
            }
        }
        catch (...)
        {
            return Response::MakeBadRequestInvalidArgument(ErrorMessage::PARSE_ERROR);
        }
    }

    StringResponse ApiRequestHandler::ProceedPlayerListRequest(std::string_view& tokenValue)
    {
        if (!tokenValue.empty())
        {
            std::string tokenTmp{ tokenValue };
            std::string token = tokenTmp.substr(TokenMessage::BEARER.size());
            
            if (game_.GetPlayerTokens().IsTokenExist(token)) {
                boost::json::object json;
                std::map<std::string, std::string> players;
                game_.GetPlayerTokens().FindPlayersBy(token, json, players);
                return Response::MakePlayersByToken(players);
            }
            else {
                return Response::MakeJSON(http::status::unauthorized, ErrorCode::UNKNOWN_TOKEN, ErrorMessage::UNKNOWN_TOKEN);
            }
        }
        else {
            return Response::MakeJSON(http::status::unauthorized, ErrorCode::INVALID_TOKEN, ErrorMessage::INVALID_TOKEN);
        }
    }

}// namespace http_handler