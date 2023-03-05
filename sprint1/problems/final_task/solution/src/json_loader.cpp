#pragma once

#include "json_loader.h"

#include <iostream>
#include <fstream>
#include <string>

namespace json_loader {

    using namespace std::literals;

    json::value GetObjectValue(const std::string& key, const json::object& object) {
        json::value value;
        for (auto iter = object.begin(); iter != object.end(); iter++)
        {
            if (iter->key() == key)
            {
                value = iter->value();
                break;
            }
        }
        return value;
    }

    model::Road GetRoad(const json::object& roadCords) {
        json::value coord_x0, coord_y0, coord_x1, coord_y1;

        coord_x0 = GetObjectValue("x0"s, roadCords);
        coord_y0 = GetObjectValue("y0"s, roadCords);
        coord_x1 = GetObjectValue("x1"s, roadCords);
        coord_y1 = GetObjectValue("y1"s, roadCords);

        model::Point point(coord_x0.as_int64(), coord_y0.as_int64());
        if (coord_y1.is_null()) {
            model::Road road = { model::Road::HORIZONTAL , point, coord_x1.as_int64()};
            return road;
        }
        else {
            model::Road road = { model::Road::VERTICAL , point, coord_y1.as_int64() };
            return road;
        }
    }

    model::Building GetBuilding(const json::object& buildingCords) {
        json::value coord_x, coord_y, width, height;

        coord_x = GetObjectValue("x"s, buildingCords);
        coord_y = GetObjectValue("y"s, buildingCords);
        width = GetObjectValue("w"s, buildingCords);
        height = GetObjectValue("h"s, buildingCords);

        model::Point point(coord_x.as_int64(), coord_y.as_int64());
        model::Size size(width.as_int64(), height.as_int64());
        model::Rectangle rect(point, size);
        model::Building building(rect);
        return building;
    }

    model::Office GetOffice(const json::object& officeCords) {
        std::string id;
        json::value coord_x, coord_y, offsetX, offsetY;
        coord_x = GetObjectValue("x"s, officeCords);
        coord_y = GetObjectValue("y"s, officeCords);
        offsetX = GetObjectValue("offsetX"s, officeCords);
        offsetY = GetObjectValue("offsetY"s, officeCords);
        id = GetObjectValue("id"s, officeCords).as_string();

        model::Office::Id officeID{ id };
        model::Point point(coord_x.as_int64(), coord_y.as_int64());
        model::Offset offset(offsetX.as_int64(), offsetY.as_int64());
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
    if (!file)
    {
        std::cout << "Can't open JSON file "sv << json_path << std::endl;
    }
    else {
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
