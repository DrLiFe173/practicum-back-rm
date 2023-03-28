#include "api_handler.h"

namespace http_handler {

    std::optional<std::string> ApiRequestHandler::TryExtractToken(const std::string_view& request)
    {
        if (!request.empty() && request.starts_with(TokenMessage::BEARER)) {
            std::string tokenTmp{ request };
            std::string token = tokenTmp.substr(TokenMessage::BEARER.size());
            if (!token.empty() && token.size() == 32) {
                return token;
            }  
            else {
                return std::nullopt;
            }
        }
        else {
            return std::nullopt;
        }
    }

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
            mapObj[ConfigDefs::ID] = *map.GetId();
            mapObj[ConfigDefs::NAME] = map.GetName();
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
            tempObj[ConfigDefs::X] = bound.position.x;
            tempObj[ConfigDefs::Y] = bound.position.y;
            tempObj[ConfigDefs::W] = bound.size.width;
            tempObj[ConfigDefs::H] = bound.size.height;
            jsonArr.push_back(tempObj);
        }
        object[ConfigDefs::BUILDINGS] = jsonArr;
    }

    void ApiRequestHandler::AddOfficesInfo(const model::Map* map, json::object& object) {
        json::array jsonArr;
        for (auto office : map->GetOffices()) {
            json::object tempObj;
            tempObj[ConfigDefs::ID] = *office.GetId();
            tempObj[ConfigDefs::X] = office.GetPosition().x;
            tempObj[ConfigDefs::Y] = office.GetPosition().y;
            tempObj[ConfigDefs::OFFSET_X] = office.GetOffset().dx;
            tempObj[ConfigDefs::OFFSET_Y] = office.GetOffset().dy;
            jsonArr.push_back(tempObj);
        }
        object[ConfigDefs::OFFICES] = jsonArr;
    }

    void ApiRequestHandler::AddMapInfo(std::string& body, std::string& mapId) {
        model::Map::Id id{ mapId };
        auto requested_map = game_.FindMap(id);
        if (requested_map != NULL) {
            json::object obj;
            obj[ConfigDefs::ID] = *requested_map->GetId();
            obj[ConfigDefs::NAME] = requested_map->GetName();

            AddRoadsInfo(requested_map, obj);
            AddBuildingsInfo(requested_map, obj);
            AddOfficesInfo(requested_map, obj);

            body = json::serialize(obj);
        }
    }

    StringResponse ApiRequestHandler::ProceedJoinGameRequest(std::string& body)
    {
        try {
            auto jsonValue = json::parse(body);            
            std::string userName = jsonValue.as_object().at(JsonField::USER_NAME).as_string().data();
            if (userName.size() != 0) {
                std::string mapId = jsonValue.as_object().at(JsonField::MAP_ID).as_string().data();
                model::Map::Id id{ mapId };
                auto requested_map = game_.FindMap(id);
                if (requested_map != nullptr) {
                    std::string token = game_.GetPlayerTokens().GenerateToken();
                    auto existedSession = game_.FindGameSession(id);
                    std::uint64_t playerId;
                    if (existedSession != nullptr) {
                        playerId = existedSession->GetLastDogIndex();
                        model::Road* road = existedSession->GetMapFromSession()->GetStartRoad();
                        model::Dog dog(userName, road);
                        existedSession->AddDog(dog);
                        model::Player player(existedSession, playerId);
                        game_.GetPlayerTokens().AddPlayer(token, player);
                    }
                    else {
                        model::GameSession session(requested_map);
                        playerId = session.GetLastDogIndex();
                        model::Road* road = session.GetMapFromSession()->GetStartRoad();
                        model::Dog dog(userName, road);
                        session.AddDog(dog);
                        game_.AddGameSession(session);
                        model::Player player(game_.FindGameSession(id), playerId);
                        game_.GetPlayerTokens().AddPlayer(token, player);
                    }
                    return Response::MakeJoinGame(token, playerId);
                }
                else {
                    return Response::MakeJSON(http::status::not_found, ErrorCode::MAP_NOT_FOUND, ErrorMessage::MAP_NOT_FOUND);
                }
            }
            else {
                return Response::MakeJSON(http::status::bad_request, ErrorCode::INVALID_ARGUMENT, ErrorMessage::INVALID_NAME);
            }
        }
        catch (...)
        {
            return Response::MakeBadRequestInvalidArgument(ErrorMessage::JOIN_GAME_PARSE_ERROR);
        }
    }

    StringResponse ApiRequestHandler::ProceedPlayerListRequest(std::string_view& tokenValue)
    {
        return ExecuteAuthorized(tokenValue, [&](std::string& token) {
            std::map<std::string, std::string> players;
            game_.GetPlayerTokens().FindPlayersBy(token, players);
            return Response::MakePlayersByToken(players);
        });
    }

    StringResponse ApiRequestHandler::ProceedGameStateRequest(std::string_view& tokenValue)
    {      
        return ExecuteAuthorized(tokenValue, [&](std::string& token) {
            std::map<std::string, boost::json::object> dogs;
            game_.GetPlayerTokens().FindDogsBy(token, dogs);
            return Response::MakeDogssByToken(dogs);            
        });
    }

    StringResponse ApiRequestHandler::ProceedPlayerActionRequest(std::string_view& tokenValue, std::string& body)
    {
        return ExecuteAuthorized(tokenValue, [&](std::string& token) {
            try {
                auto jsonValue = json::parse(body);
                std::string direction = jsonValue.as_object().at(JsonField::MOVE).as_string().data();
                auto player = game_.GetPlayerTokens().FindPlayerByToken(token);
                player.ChangeDogSpeed(static_cast<model::Direction> (model::DirectionInterpretator::DirectionStringToInt(direction)));
                return Response::MakePlayerActionByToken();
            }
            catch (...)
            {
                return Response::MakeBadRequestInvalidArgument(ErrorMessage::GAME_ACTION_PARSE_ERROR);
            }
        });
    }

    StringResponse ApiRequestHandler::ProceedGameTickRequest(std::string_view& tokenValue, std::string& body)
    {
        try {
            auto jsonValue = json::parse(body);
            const int64_t tick = jsonValue.as_object().at(JsonField::TIME_DELTA).as_int64();
            game_.UpdateGameSessions(tick);
            return Response::MakePlayerActionByToken();
        }
        catch (...)
        {
            return Response::MakeBadRequestInvalidArgument(ErrorMessage::GAME_ACTION_PARSE_ERROR);
        }
    }

}// namespace http_handler