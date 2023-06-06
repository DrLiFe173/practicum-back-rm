#include "json_loader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>

namespace json_loader {

    using namespace std::literals;

    model::Road GetRoad(const json::object& roadCords) {
        int64_t coord_x0, coord_y0;

        coord_x0 = roadCords.at(ConfigDefs::X0).as_int64();
        coord_y0 = roadCords.at(ConfigDefs::Y0).as_int64();

        model::Point point(coord_x0, coord_y0);
        if (roadCords.if_contains(ConfigDefs::X1) != NULL) {
            int64_t coord_x1 = roadCords.at(ConfigDefs::X1).as_int64();
            model::Road road = { model::Road::HORIZONTAL , point, coord_x1};
            return road;
        }
        else {
            int64_t coord_y1 = roadCords.at(ConfigDefs::Y1).as_int64();
            model::Road road = { model::Road::VERTICAL , point, coord_y1 };
            return road;
        }
    }

    model::Building GetBuilding(const json::object& buildingCords) {
        int64_t coord_x, coord_y, width, height;

        coord_x = buildingCords.at(ConfigDefs::X).as_int64();
        coord_y = buildingCords.at(ConfigDefs::Y).as_int64();
        width = buildingCords.at(ConfigDefs::W).as_int64();
        height = buildingCords.at(ConfigDefs::H).as_int64();

        model::Point point(coord_x, coord_y);
        model::Size size(width, height);
        model::Rectangle rect(point, size);
        model::Building building(rect);
        return building;
    }

    model::Office GetOffice(const json::object& officeCords) {
        std::string id;
        int64_t coord_x, coord_y, offsetX, offsetY;
        coord_x = officeCords.at(ConfigDefs::X).as_int64();
        coord_y = officeCords.at(ConfigDefs::Y).as_int64();
        offsetX = officeCords.at(ConfigDefs::OFFSET_X).as_int64();
        offsetY = officeCords.at(ConfigDefs::OFFSET_Y).as_int64();
        id = officeCords.at(ConfigDefs::ID).as_string();

        model::Office::Id officeID{ id };
        model::Point point(coord_x, coord_y);
        model::Offset offset(offsetX, offsetY);
        model::Office office(officeID, point, offset);
        return office;
    }



void LoadGame(const std::filesystem::path& json_path, model::Game& game) {
    // Загрузить содержимое файла json_path, например, в виде строки
    // Распарсить строку как JSON, используя boost::json::parse
    // Загрузить модель игры из файла
    std::ifstream file(json_path.string());
    std::string jsonString;
    if (file)
    {
        while (file)
        {
            std::string strInput;
            std::getline(file, strInput);
            jsonString.append(strInput);
        }
    }
    else {
        throw std::runtime_error(ServerMessage::JSON_LOAD_FAIL.data());
    }
    auto jsonValue = json::parse(jsonString);

    float default_speed;
    try {
        default_speed = static_cast<float>(jsonValue.at(ConfigDefs::DEF_SPEED).as_double());
        model::RealDimension def_speed{ default_speed };
        game.SetDefaultSpeed(def_speed);
    }
    catch (...) {
        model::RealDimension defaultSpeed{ 1.0f };      // устанавливаем default скорость = 1.0f если поле defaultDogSpeed не определенно в config.json
        game.SetDefaultSpeed(defaultSpeed);
    }
    for (const auto& map_ : jsonValue.at(ConfigDefs::MAPS).as_array())
    {
        float speed;
        try {
            speed = static_cast<float>(map_.as_object().at(ConfigDefs::MAP_SPEED).as_double());
        }
        catch (...) {
            speed = default_speed;                      // устанавливаем default скорость для карты если поле dogSpeed не определенно в config.json
        }
        model::RealDimension map_speed{ speed };

        std::string ID = map_.as_object().at(ConfigDefs::ID).as_string().data();
        std::string mapName = map_.as_object().at(ConfigDefs::NAME).as_string().data();
        model::Map::Id map_Id{ ID };
        model::Map map(map_Id, mapName);
        map.SetMapSpeed(map_speed);
        
        for (const auto& road_ : map_.at(ConfigDefs::ROADS).as_array())
        {
            map.AddRoad(GetRoad(road_.as_object()));          
        }

        for (const auto& building_ : map_.at(ConfigDefs::BUILDINGS).as_array())
        {
            map.AddBuilding(GetBuilding(building_.as_object()));
        }

        for (const auto& office_ : map_.at(ConfigDefs::OFFICES).as_array())
        {
            map.AddOffice(GetOffice(office_.as_object()));
        }        
    
        game.AddMap(map);
    }

    game.SetMapCrossRoads();   
}

}  // namespace json_loader
