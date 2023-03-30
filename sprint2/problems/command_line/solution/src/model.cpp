#include "model.h"

#include <stdexcept>

namespace model {
using namespace std::literals;

void Dog::MakeHorizontalMovement(const double& tick) {
    auto new_coord = position_.x + speed_.vx * tick;
    RealDimension x_pos_start, x_pos_end;
    RealDimension offset = 0.4;

    if (road_->GetStart().x <= road_->GetEnd().x) {     // условие если дорога по координатам идет в обратную сторону
        x_pos_start = road_->GetStart().x - offset;
        x_pos_end = road_->GetEnd().x + offset;
    }
    else {
        x_pos_start = road_->GetEnd().x - offset;
        x_pos_end = road_->GetStart().x + offset;
    }

    if (direction_ == Direction::WEST) {
        if (position_.x != x_pos_start && !IsZeroSpeed()) {                   // проверить край дороги
            if (new_coord >= (x_pos_start)) {         // проверить возможность движения в пределах дороги
                position_.x = new_coord;
            }
            else {
                position_.x = x_pos_start;
                Speed zero_speed = Speed{ 0.0, 0.0 };
                SetDogSpeed(zero_speed);
            }
        }
        else {
            Speed zero_speed = Speed{ 0.0, 0.0 };
            SetDogSpeed(zero_speed);
        }
    }
    else {
        if (position_.x != x_pos_end && !IsZeroSpeed()) {                   // проверить край дороги
            if (new_coord < (x_pos_end)) {         // проверить возможность движения в пределах дороги
                position_.x = new_coord;
            }
            else {
                position_.x = x_pos_end;
                Speed zero_speed = Speed{ 0.0, 0.0 };
                SetDogSpeed(zero_speed);
            }
        }
        else {
            Speed zero_speed = Speed{ 0.0, 0.0 };
            SetDogSpeed(zero_speed);
        }
    }
}

void Dog::MakeVerticalMovement(const double& tick) {
    auto new_coord = position_.y + speed_.vy * tick;
    RealDimension y_pos_start, y_pos_end;
    const RealDimension offset = 0.4;

    if (road_->GetStart().y <= road_->GetEnd().y) {     // условие если дорога по координатам идет в обратную сторону
        y_pos_start = road_->GetStart().y - offset;
        y_pos_end = road_->GetEnd().y + offset;
    }
    else {
        y_pos_start = road_->GetEnd().y - offset;
        y_pos_end = road_->GetStart().y + offset;
    }

    if (direction_ == Direction::NORTH) {
        if (position_.y != y_pos_start && !IsZeroSpeed()) {                   // проверить край дороги
            if (new_coord >= y_pos_start) {         // проверить возможность движения в пределах дороги
                position_.y = new_coord;
            }
            else {
                position_.y = y_pos_start;
                Speed zero_speed = Speed{ 0.0, 0.0 };
                SetDogSpeed(zero_speed);
            }
        }
        else {
            Speed zero_speed = Speed{ 0.0, 0.0 };
            SetDogSpeed(zero_speed);
        }
    }
    else {
        if (position_.y != y_pos_end && !IsZeroSpeed()) {                   // проверить край дороги
            if (new_coord <= y_pos_end) {         // проверить возможность движения в пределах дороги
                position_.y = new_coord;
            }
            else {
                position_.y = y_pos_end;
                Speed zero_speed = Speed{ 0.0, 0.0 };
                SetDogSpeed(zero_speed);
            }
        }
        else {
            Speed zero_speed = Speed{ 0.0, 0.0 };
            SetDogSpeed(zero_speed);
        }
    }
}

void Dog::UpdateCoords(const int64_t& tick) {
    double tick_d = tick / 1000.0;
    if (direction_ == Direction::NORTH || direction_ == Direction::SOUTH) {
        if (road_->IsVertical()) {                              // проверить соосность направления скорости с дорогой
            MakeVerticalMovement(tick_d);
        }
        else {
            // проверка наличия перекрестка
            double val;
            double dot_val = round(modf(position_.x, &val) * 10) / 10;
            int64_t pos;
            if (dot_val != 0.5) {                                 // т.к. дорога имеет ширину +- 0,4 от числа координаты, то при значении 0,5 игрок точно не войдет на перекресток
                pos = (dot_val <= 0.4) ? static_cast<int64_t>(val) : static_cast<int64_t>(val + 1);
                auto testBool = road_->GetCrossRoars().find(pos) != road_->GetCrossRoars().end();
                if (road_->GetCrossRoars().find(pos) != road_->GetCrossRoars().end()) {
                    road_ = road_->GetCrossRoars().at(pos);
                }
            }
            MakeVerticalMovement(tick_d);
        }
    }
    else if (direction_ == Direction::WEST || direction_ == Direction::EAST) {
        if (road_->IsHorizontal()) {                              // проверить соосность направления скорости с дорогой
            MakeHorizontalMovement(tick_d);
        }
        else {
            // проверка наличия перекрестка
            double val;
            double dot_val = round(modf(position_.y, &val) * 10) / 10;
            int64_t pos;
            if (dot_val != 0.5) {                                 // т.к. дорога имеет ширину +- 0,4 от числа координаты, то при значении 0,5 игрок точно не войдет на перекресток
                pos = (dot_val <= 0.4) ? static_cast<int64_t>(val) : static_cast<int64_t>(val + 1);
                auto testBool = road_->GetCrossRoars().find(pos) != road_->GetCrossRoars().end();
                if (road_->GetCrossRoars().find(pos) != road_->GetCrossRoars().end()) {
                    road_ = road_->GetCrossRoars().at(pos);
                }
            }
            MakeHorizontalMovement(tick_d);
        }
    }
}

void Dog::SetStartPosition() {
    if (Game::IsRandomSpawnEnabled()) {                                                                 // Если игра запущена с флагом --randomize-spawn-points
        std::random_device rd;                                                                          // то генерируем случайную позицию в пределах стартовой дороги
        std::mt19937 gen(rd());
        int64_t start_pos, end_pos, other_coord;
        if (road_->IsHorizontal()) {
            start_pos = road_->GetStart().x;
            end_pos = road_->GetEnd().x;
            other_coord = road_->GetStart().y;
            std::uniform_int_distribution<int64_t> distrib(start_pos, end_pos);
            position_ = { static_cast<float>(distrib(gen)), static_cast<float>(other_coord) };
        }
        else {
            start_pos = road_->GetStart().y;
            end_pos = road_->GetEnd().y;
            other_coord = road_->GetStart().x;
            std::uniform_int_distribution<int64_t> distrib(start_pos, end_pos);
            position_ = { static_cast<float>(other_coord), static_cast<float>(distrib(gen)) };
        }
    }
    else {
        position_ = { static_cast<float>(road_->GetStart().x), static_cast<float>(road_->GetStart().y) }; // иначе высталяем стартовую позицию как {0,0}
    }    
}

void Dog::SetDogSpeed(Speed& new_speed) {
    if (new_speed.vx > 0) {
        speed_.vx = new_speed.vx;
        speed_.vy = 0.0;
        direction_ = Direction::EAST;
    }
    else if (new_speed.vx < 0) {
        speed_.vx = new_speed.vx;
        speed_.vy = 0.0;
        direction_ = Direction::WEST;
    }
    else if (new_speed.vy > 0) {
        speed_.vx = 0.0;
        speed_.vy = new_speed.vy;
        direction_ = Direction::SOUTH;
    }
    else if (new_speed.vy < 0) {
        speed_.vx = 0.0;
        speed_.vy = new_speed.vy;
        direction_ = Direction::NORTH;
    }
    else {
        speed_.vx = 0.0;
        speed_.vy = 0.0;
    }
}

void Player::ChangeDogSpeed(const Direction& direction) {
    auto speed_val = sesion_->GetMapFromSession()->GetMapSpeed();
    if (direction == Direction::UNKNOWN) {
        Speed speed{ 0.0, 0.0 };
        dog_.SetDogSpeed(speed);
    }
    else if (direction == Direction::NORTH) {
        Speed speed{ 0.0, speed_val  * -1.0 };
        dog_.SetDogSpeed(speed);
    }
    else if (direction == Direction::SOUTH) {
        Speed speed{ 0.0, speed_val};
        dog_.SetDogSpeed(speed);
    }
    else if (direction == Direction::WEST) {
        Speed speed{ speed_val * -1.0, 0.0 };
        dog_.SetDogSpeed(speed);
    }
    else if (direction == Direction::EAST) {
        Speed speed{ speed_val , 0.0 };
        dog_.SetDogSpeed(speed);
    }
}

void PlayerTokens::FindDogsBy(const std::string& token, std::map<std::string, boost::json::object>& dogs) {
    if (IsTokenExist(token)) {
        for (auto player : playerTokens_) {
            boost::json::object js;
            Player pl = player.second;
            Dog dog = pl.GetPlayerDog();
            DogCoords pos = dog.GetDogCoords();
            js["pos"] = boost::json::array{ pos.x, pos.y };
            Speed speed = dog.GetDogSpeed();
            js["speed"] = boost::json::array{ speed.vx, speed.vy };
            int dir = dog.GetDogDirection();
            std::string direction = DirectionInterpretator::DirectionIntToString(dir);
            js["dir"] = direction;
            std::string playerId = std::to_string(pl.GetPlayerId());
            dogs.insert({ playerId, js });
        };
    }
}

void Map::SetCrossroads() {
    for (size_t i = 0; i < roads_.size(); i++) {
        for (size_t j = 0; j < roads_.size(); j++) {
            if ((roads_[i].IsHorizontal() && roads_[j].IsVertical()) || (roads_[i].IsVertical() && roads_[j].IsHorizontal())) {
                if (roads_[i].IsHorizontal()) {
                    Dimension x_pos_start, x_pos_end;
                    if (roads_[i].GetStart().x <= roads_[i].GetEnd().x) {  // условие если дорога по координатам идет в обратную сторону
                        x_pos_start = roads_[i].GetStart().x;
                        x_pos_end = roads_[i].GetEnd().x;
                    }
                    else {
                        x_pos_start = roads_[i].GetEnd().x;
                        x_pos_end = roads_[i].GetStart().x;
                    }

                    if ((x_pos_start <= roads_[j].GetStart().x) && (x_pos_end >= roads_[j].GetStart().x)) {
                        Dimension pos = roads_[j].GetStart().x;
                        bool cond = roads_[i].GetCrossRoars().find(pos) == roads_[i].GetCrossRoars().end();
                        if (cond) {           // проверка что дорога не внесена в карту перекрестков
                            roads_[i].AddCrossroad(pos, &roads_[j]);
                        }
                    }
                }
                else {
                    Dimension y_pos_start, y_pos_end;
                    if (roads_[i].GetStart().y <= roads_[i].GetEnd().y) {  // условие если дорога по координатам идет в обратную сторону
                        y_pos_start = roads_[i].GetStart().y;
                        y_pos_end = roads_[i].GetEnd().y;
                    }
                    else {
                        y_pos_start = roads_[i].GetEnd().y;
                        y_pos_end = roads_[i].GetStart().y;
                    }
                    if ((y_pos_start <= roads_[j].GetStart().y) && (y_pos_end >= roads_[j].GetStart().y)) {
                        Dimension pos = roads_[j].GetStart().y;
                        bool cond = roads_[i].GetCrossRoars().find(pos) == roads_[i].GetCrossRoars().end();
                        if (cond) {           // проверка что дорога не внесена в карту перекрестков
                            roads_[i].AddCrossroad(pos, &roads_[j]);
                        }
                    }
                }
            }
        }
    }
}

Road* Map::GetStartRoad() {
    int road_num = 0;                                                               // Выставляем 0ю дорогу
    if (Game::IsRandomSpawnEnabled()) {                                             // Если игра запущена с флагом --randomize-spawn-points
        size_t size = roads_.size();                                                // то случайно выбираем любую из имеющихся дорог
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, static_cast<int>(size));
        int road_num = distrib(gen);
    }
    return &(roads_.at(road_num));
}

void Game::SetMapCrossRoads()
{
    for (size_t i = 0; i < maps_.size(); i++) {
        maps_.at(i).SetCrossroads();
    }
}

void Map::AddOffice(Office office) {
    if (warehouse_id_to_index_.contains(office.GetId())) {
        throw std::invalid_argument("Duplicate warehouse");
    }

    const size_t index = offices_.size();
    Office& o = offices_.emplace_back(std::move(office));
    try {
        warehouse_id_to_index_.emplace(o.GetId(), index);
    } catch (...) {
        // Удаляем офис из вектора, если не удалось вставить в unordered_map
        offices_.pop_back();
        throw;
    }
}

void Game::AddMap(Map map) {
    const size_t index = maps_.size();
    if (auto [it, inserted] = map_id_to_index_.emplace(map.GetId(), index); !inserted) {
        throw std::invalid_argument("Map with id "s + *map.GetId() + " already exists"s);
    } else {
        try {
            maps_.emplace_back(std::move(map));
        } catch (...) {
            map_id_to_index_.erase(it);
            throw;
        }
    }
}

void Game::AddGameSession(GameSession session) {
    gameSessions_.push_back(session); 
}


}  // namespace model
