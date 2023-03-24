#pragma once

#include <filesystem>

#include "model.h"
#include "logger.h"
#include <boost/json.hpp>

namespace json_loader {

namespace json = boost::json;

model::Road GetRoad(const json::object& roadCords);

model::Building GetBuilding(const json::object& buildingCords);

model::Office GetOffice(const json::object& officeCords);

void LoadGame(const std::filesystem::path& json_path, model::Game& game);

}  // namespace json_loader
