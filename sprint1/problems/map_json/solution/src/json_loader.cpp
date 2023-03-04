#pragma once

#include "json_loader.h"
#include <boost/json.hpp>

#include <iostream>
#include <fstream>
#include <string>

namespace json_loader {

    using namespace std::literals;
    namespace json = boost::json;

model::Game LoadGame(const std::filesystem::path& json_path) {
    // Загрузить содержимое файла json_path, например, в виде строки
    // Распарсить строку как JSON, используя boost::json::parse
    // Загрузить модель игры из файла
    model::Game game;
    std::string filePath = "../../data/";
    std::ifstream file(filePath.append(json_path.string()));
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
            auto roadCords = road_.as_object();
            int64_t x0, y0, endCoord;
            bool horizontalRoad = false;
            for (auto iter = roadCords.begin(); iter != roadCords.end(); iter++)
            {
                if (iter->key() == "x0"s) {
                    x0 = iter->value().as_int64();
                }
                else if (iter->key() == "y0"s) {
                    y0 = iter->value().as_int64();
                }
                else if (iter->key() == "x1"s) {
                    endCoord = iter->value().as_int64();
                    horizontalRoad = true;
                }
                else if (iter->key() == "y1"s) {
                    endCoord = iter->value().as_int64();
                }
            };

            model::Point point(x0, y0);
            if (horizontalRoad) {
                model::Road road = { model::Road::HORIZONTAL , point, endCoord };
                map.AddRoad(road);
            }
            else {
                model::Road road = { model::Road::VERTICAL , point, endCoord };
                map.AddRoad(road);
            }           
        }

        for (const auto& building_ : map_.at("buildings"s).as_array())
        {
            auto buildingCords = building_.as_object();
            int64_t x, y, w, h;
            for (auto iter = buildingCords.begin(); iter != buildingCords.end(); iter++)
            {
                if (iter->key() == "x"s) {
                    x = iter->value().as_int64();
                }
                else if (iter->key() == "y"s) {
                    y = iter->value().as_int64();
                }
                else if (iter->key() == "w"s) {
                    w = iter->value().as_int64();
                }
                else if (iter->key() == "h"s) {
                    h = iter->value().as_int64();
                }
            };

            model::Point point(x, y);
            model::Size size(w, h);
            model::Rectangle rect(point, size);
            model::Building building(rect);
            map.AddBuilding(building);
        }

        for (const auto& office_ : map_.at("offices"s).as_array())
        {
            auto officeCords = office_.as_object();
            std::string id;
            int64_t x, y, offsetX, offsetY;
            for (auto iter = officeCords.begin(); iter != officeCords.end(); iter++)
            {
                if (iter->key() == "x"s) {
                    x = iter->value().as_int64();
                }
                else if (iter->key() == "y"s) {
                    y = iter->value().as_int64();
                }
                else if (iter->key() == "offsetX"s) {
                    offsetX = iter->value().as_int64();
                }
                else if (iter->key() == "offsetY"s) {
                    offsetY = iter->value().as_int64();
                }
                else if (iter->key() == "id"s) {
                    id = iter->value().as_string();
                }
            };

            model::Office::Id officeID{ id };
            model::Point point(x, y);
            model::Offset offset(offsetX, offsetY);
            model::Office office(officeID, point, offset);
            map.AddOffice(office);
        }
    
        game.AddMap(map);
    }
    return game;
}

}  // namespace json_loader
