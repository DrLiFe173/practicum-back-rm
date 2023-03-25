#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include <sstream>
#include <boost/json.hpp>
#include <boost/format.hpp>
#include <math.h>

#include "tagged.h"
#include "magic_defs.h"


namespace model {

using Dimension = int64_t;
using Coord = Dimension;
using RealDimension = float;

enum Direction {
    UNKNOWN = -1,
    NORTH   = 0,
    SOUTH   = 1,
    EAST    = 2,
    WEST    = 3
};

class DirectionInterpretator {
public:
    DirectionInterpretator() = delete;

    static std::string DirectionIntToString(int direction) {
        std::string dir = "";
        if (direction >= 0 && direction < 4) {
            if (direction == Direction::NORTH) {
                dir = GameDefs::NORTH;
            }
            else if (direction == Direction::SOUTH) {
                dir = GameDefs::SOUTH;
            }
            else if (direction == Direction::EAST) {
                dir = GameDefs::EAST;
            }
            else if (direction == Direction::WEST) {
                dir = GameDefs::WEST;
            }
        }
        return dir;
    }

    static int DirectionStringToInt(std::string& direction) {
        int dir = Direction::UNKNOWN;
        if (!direction.empty()) {
            if (direction == GameDefs::NORTH) {
                dir = Direction::NORTH;
            }
            else if (direction == GameDefs::SOUTH) {
                dir = Direction::SOUTH;
            }
            else if (direction == GameDefs::EAST) {
                dir = Direction::EAST;
            }
            else if (direction == GameDefs::WEST) {
                dir = Direction::WEST;
            }
        }
        return dir;
    }
};

struct Speed {
    RealDimension vx, vy;
};

struct DogCoords {
    RealDimension x, y;
};

struct Point {
    Coord x, y;
};

struct Size {
    Dimension width, height;
};

struct Rectangle {
    Point position;
    Size size;
};

struct Offset {
    Dimension dx, dy;
};

class Road {
    struct HorizontalTag {
        explicit HorizontalTag() = default;
    };

    struct VerticalTag {
        explicit VerticalTag() = default;
    };

public:
    constexpr static HorizontalTag HORIZONTAL{};
    constexpr static VerticalTag VERTICAL{};

    Road(HorizontalTag, Point start, Coord end_x) noexcept
        : start_{start}
        , end_{end_x, start.y} {
    }

    Road(VerticalTag, Point start, Coord end_y) noexcept
        : start_{start}
        , end_{start.x, end_y} {
    }

    bool IsHorizontal() const noexcept {
        return start_.y == end_.y;
    }

    bool IsVertical() const noexcept {
        return start_.x == end_.x;
    }

    Point GetStart() const noexcept {
        return start_;
    }

    Point GetEnd() const noexcept {
        return end_;
    }

private:
    Point start_;
    Point end_;
};

class Building {
public:
    explicit Building(Rectangle bounds) noexcept
        : bounds_{bounds} {
    }

    const Rectangle& GetBounds() const noexcept {
        return bounds_;
    }

private:
    Rectangle bounds_;
};

class Office {
public:
    using Id = util::Tagged<std::string, Office>;

    Office(Id id, Point position, Offset offset) noexcept
        : id_{std::move(id)}
        , position_{position}
        , offset_{offset} {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    Point GetPosition() const noexcept {
        return position_;
    }

    Offset GetOffset() const noexcept {
        return offset_;
    }

private:
    Id id_;
    Point position_;
    Offset offset_;
};

class Map {
public:
    using Id = util::Tagged<std::string, Map>;
    using Roads = std::vector<Road>;
    using Buildings = std::vector<Building>;
    using Offices = std::vector<Office>;

    Map(Id id, std::string name) noexcept
        : id_(std::move(id))
        , name_(std::move(name)) {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

    const Buildings& GetBuildings() const noexcept {
        return buildings_;
    }

    const Roads& GetRoads() const noexcept {
        return roads_;
    }

    const Offices& GetOffices() const noexcept {
        return offices_;
    }

    void AddRoad(const Road& road) {
        roads_.emplace_back(road);
    }

    void AddBuilding(const Building& building) {
        buildings_.emplace_back(building);
    }

    void AddOffice(Office office);

    void SetMapSpeed(RealDimension speed){
        map_speed_ = speed;
    }

    const RealDimension GetMapSpeed() const {
        return map_speed_;
    }

private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    Roads roads_;
    Buildings buildings_;

    OfficeIdToIndex warehouse_id_to_index_;
    Offices offices_;
    RealDimension map_speed_;
};

class Dog {
public:
    Dog() = default;

    Dog(std::string& name)
        :name_(name),
        speed_({ 0.0f, 0.0f }),
        direction_(Direction::NORTH){
        SetStartPosition();
    }

    const std::string& GetDogName() {
        return name_;
    }

    Speed GetDogSpeed() {
        return speed_;
    }

    DogCoords GetDogCoords() {
        return position_;
    }

    int GetDogDirection() {
        return direction_;
    }

    void SetDogSpeed(Speed& new_speed) {
        speed_.vx = new_speed.vx;
        speed_.vy = new_speed.vy;
    }

private:
    void SetStartPosition() {
        std::random_device rd;  
        std::mt19937 gen(rd()); 
        std::uniform_int_distribution<> distrib(0, 10);
        position_ = { static_cast<float>(distrib(gen)), static_cast<float>(distrib(gen)) };
    }

    std::string name_;
    DogCoords position_;
    Speed speed_;
    int direction_;
};

class GameSession {
public:
    GameSession(const Map* map)
        :map_(map),
        lastDogIndex_(0) {}

    void AddDog(Dog dog) {
        dogs_.insert({ lastDogIndex_, dog });
        ++lastDogIndex_;
    }

    const std::unordered_map<std::uint64_t, Dog> GetDogsFromSession() {
        return dogs_;
    }

    const Map* GetMapFromSession() const {
        return map_;
    }

    std::uint64_t GetLastDogIndex() const {
        return lastDogIndex_;
    }

    Dog& GetDogByIndex(std::uint64_t index)  {
        auto test = dogs_.find(index);
        return dogs_.at(index);
    }

private:
    std::unordered_map<std::uint64_t, Dog> dogs_;
    const Map* map_;
    std::uint64_t lastDogIndex_;
};

class Player {
public:
     Player(GameSession* sesion, std::uint64_t& dogId)
        :sesion_(sesion),
         dog_(sesion->GetDogByIndex(dogId)),
         playerId_(dogId){}

    Dog& GetPlayerDog() {
        return dog_;
    }

    std::uint64_t GetPlayerId() {
        return playerId_;
    }

    const GameSession* GetPlayerSession() {
        return sesion_;
    }

    void ChangeDogSpeed(const Direction& direction) {
        auto speed_val = sesion_->GetMapFromSession()->GetMapSpeed();
        if (direction == Direction::UNKNOWN) {
            Speed speed{ 0.0f, 0.0f };
            dog_.SetDogSpeed(speed);
        }
        else if (direction == Direction::NORTH) {
            Speed speed{ 0.0f, speed_val * -1.0f };
            dog_.SetDogSpeed(speed);
        }
        else if (direction == Direction::SOUTH) {
            Speed speed{ 0.0f, speed_val };
            dog_.SetDogSpeed(speed);
        }
        else if (direction == Direction::WEST) {
            Speed speed{ speed_val * -1.0f, 0.0f };
            dog_.SetDogSpeed(speed);
        }
        else if (direction == Direction::EAST) {
            Speed speed{ speed_val , 0.0f };
            dog_.SetDogSpeed(speed);
        }
    }

private:
    const GameSession* sesion_;
    Dog& dog_;
    std::uint64_t playerId_;
};

class PlayerTokens {
public:
    PlayerTokens()
        : lastPlayerIndex_(0) {
    }

    std::string GenerateToken() {
        auto i1 = generator1_();
        auto i2 = generator2_();
        std::string result = (boost::format("%x") % i1).str();
        result.append((boost::format("%x") % i2).str());
        if (result.size() == 31) {                          // если при генерации токена получаем строку в 31 символ - добавляем еще 1 чтобы получить 32
                result.append("0");
        }
        return result;
    }

    void AddPlayer(const std::string& token, Player player) {
        playerTokens_.insert({ token, player });
        ++lastPlayerIndex_;
    }

    bool IsTokenExist(const std::string& token) {
        return playerTokens_.contains(token);
    }

    void FindPlayersBy(const std::string& token, std::map<std::string, std::string>& players) {        
        if (IsTokenExist(token)) {
            for (auto player : playerTokens_) {
                Player pl = player.second;
                Dog dog = pl.GetPlayerDog();
                std::string name = dog.GetDogName();
                std::string playerId = std::to_string(pl.GetPlayerId());
                players.insert({ playerId, name});
            };
        }
    }

    Player& FindPlayerByToken(const std::string& token) {
        return playerTokens_.at(token);
    }

    void FindDogsBy(const std::string& token, std::map<std::string, boost::json::object>& dogs) {
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

private:
    std::random_device random_device_;
    std::mt19937_64 generator1_{ [this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }() };
    std::mt19937_64 generator2_{ [this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }() };
    // Чтобы сгенерировать токен, получите из generator1_ и generator2_
    // два 64-разрядных числа и, переведя их в hex-строки, склейте в одну.
    // Вы можете поэкспериментировать с алгоритмом генерирования токенов,
    // чтобы сделать их подбор ещё более затруднительным
    std::unordered_map<std::string, Player> playerTokens_;
    std::uint64_t lastPlayerIndex_;
};

class Game {
public:
    Game() = default;

    using Maps = std::vector<Map>;
    using GameSessions = std::vector<GameSession>;

    void AddMap(Map map);

    void AddGameSession(GameSession session);

    const Maps& GetMaps() const noexcept {
        return maps_;
    }

    const GameSessions& GetGameSessions() const noexcept {
        return gameSessions_;
    }

    const Map* FindMap(const Map::Id& id) const noexcept {
        if (auto it = map_id_to_index_.find(id); it != map_id_to_index_.end()) {
            return &maps_.at(it->second);
        }
        return nullptr;
    }

    GameSession* FindGameSession(const Map::Id& id) noexcept {
        for (auto i = 0; i < gameSessions_.size(); i++) {   
            auto session = gameSessions_.at(i);
            if (session.GetMapFromSession()->GetId() == id) {
                return &gameSessions_.at(i);
            }
        }
        return nullptr;
    }

    PlayerTokens& GetPlayerTokens() {
        return tokens_;
    }

    void SetDefaultSpeed(RealDimension speed) {
        default_speed_ = speed;
    }

private:
    using MapIdHasher = util::TaggedHasher<Map::Id>;
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;

    std::vector<Map> maps_;
    std::vector<GameSession> gameSessions_;
    MapIdToIndex map_id_to_index_;
    PlayerTokens tokens_;
    RealDimension default_speed_;
};

}  // namespace model
