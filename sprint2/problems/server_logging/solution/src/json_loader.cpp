#include "json_loader.h"

#include <iostream>
#include <fstream>
#include <string>

namespace json_loader {

    using namespace std::literals;

    model::Road GetRoad(const json::object& roadCords) {
        int64_t coord_x0, coord_y0;

        coord_x0 = roadCords.at("x0"s).as_int64(); 
        coord_y0 = roadCords.at("y0"s).as_int64();

        model::Point point(coord_x0, coord_y0);
        if (roadCords.if_contains("x1"s) != NULL) {
            int64_t coord_x1 = roadCords.at("x1"s).as_int64();
            model::Road road = { model::Road::HORIZONTAL , point, coord_x1};
            return road;
        }
        else {
            int64_t coord_y1 = roadCords.at("y1"s).as_int64();
            model::Road road = { model::Road::VERTICAL , point, coord_y1 };
            return road;
        }
    }

    model::Building GetBuilding(const json::object& buildingCords) {
        int64_t coord_x, coord_y, width, height;

        coord_x = buildingCords.at("x"s).as_int64();
        coord_y = buildingCords.at("y"s).as_int64();
        width = buildingCords.at("w"s).as_int64();
        height = buildingCords.at("h"s).as_int64();

        model::Point point(coord_x, coord_y);
        model::Size size(width, height);
        model::Rectangle rect(point, size);
        model::Building building(rect);
        return building;
    }

    model::Office GetOffice(const json::object& officeCords) {
        std::string id;
        int64_t coord_x, coord_y, offsetX, offsetY;
        coord_x = officeCords.at("x"s).as_int64();
        coord_y = officeCords.at("y"s).as_int64();
        offsetX = officeCords.at("offsetX"s).as_int64();
        offsetY = officeCords.at("offsetY"s).as_int64();
        id = officeCords.at("id"s).as_string();

        model::Office::Id officeID{ id };
        model::Point point(coord_x, coord_y);
        model::Offset offset(offsetX, offsetY);
        model::Office office(officeID, point, offset);
        return office;
    }



model::Game LoadGame(const std::filesystem::path& json_path) {
    // Загрузить содержимое файла json_path, например, в виде строки
    // Распарсить строку как JSON, используя boost::json::parse
    // Загрузить модель игры из файла
    model::Game game;
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
    auto jsonValue = json::parse(jsonString);
    for (const auto& map_ : jsonValue.at("maps"s).as_array())
    {
        std::string ID = map_.as_object().at("id"s).as_string().data();
        std::string mapName = map_.as_object().at("name"s).as_string().data();
        model::Map::Id map_Id{ ID };
        model::Map map(map_Id, mapName);        
        
        for (const auto& road_ : map_.at("roads"s).as_array())
        {
            map.AddRoad(GetRoad(road_.as_object()));          
        }

        for (const auto& building_ : map_.at("buildings"s).as_array())
        {
            map.AddBuilding(GetBuilding(building_.as_object()));
        }

        for (const auto& office_ : map_.at("offices"s).as_array())
        {
            map.AddOffice(GetOffice(office_.as_object()));
        }
    
        game.AddMap(map);
    }
    return game;
}

}  // namespace json_loader
