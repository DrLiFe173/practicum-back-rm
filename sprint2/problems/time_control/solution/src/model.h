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
using RealDimension = double;

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

    void AddCrossroad(Dimension num, Road* road) {
        crossroads_.emplace(num, road);
    }

    std::map<Dimension, Road*>& GetCrossRoars() {
        return crossroads_;
    }

private:
    Point start_;
    Point end_;
    std::map<Dimension, Road*> crossroads_;  // храним в ключе координату пересечени€ (x дл€ Horizontal дороги, y дл€ Vertical)
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

    void SetCrossroads();

    Road* GetStartRoad() {
        /*size_t size = roads_.size();
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, static_cast<int>(size));
        int road_num = distrib(gen);*/

        int road_num = 0; // Ќа врем€ тестировани€ всегда выставл€ем на 0ю дорогу

        return &(roads_.at(road_num));
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

    Dog(std::string& name, Road* road)
        :name_(name),
        speed_({ 0.0f, 0.0f }),
        direction_(Direction::NORTH),
        road_(road){
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

    void SetDogSpeed(Speed& new_speed);

    void SetRoad(Road* road) {
        road_ = road;
    }    

    void UpdateCoords(const int64_t& tick);

private:
    void SetStartPosition();

    void MakeHorizontalMovement(const double& tick);

    void MakeVerticalMovement(const double& tick);

    std::string name_;
    DogCoords position_;
    Speed speed_;
    int direction_;
    Road* road_;
};

class GameSession {
public:
    GameSession(Map* map)
        :map_(map),
        lastDogIndex_(0) {}

    void AddDog(Dog dog) {
        dogs_.insert({ lastDogIndex_, dog });
        ++lastDogIndex_;
    }

    const std::unordered_map<std::uint64_t, Dog> GetDogsFromSession() {
        return dogs_;
    }

    Map* GetMapFromSession() {
        return map_;
    }


    std::uint64_t GetLastDogIndex() const {
        return lastDogIndex_;
    }

    Dog& GetDogByIndex(std::uint64_t index)  {
        auto test = dogs_.find(index);
        return dogs_.at(index);
    }

    void UpdateGameState(const int64_t& ticks) {
        for (auto &dog : dogs_) {
            dog.second.UpdateCoords(ticks);
        }
    }

private:
    std::unordered_map<std::uint64_t, Dog> dogs_;
    Map* map_;
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

    void ChangeDogSpeed(const Direction& direction);

private:
    GameSession* sesion_;
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
        if (result.size() == 31) {                          // если при генерации токена получаем строку в 31 символ - добавл€ем еще 1 чтобы получить 32
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

    void FindDogsBy(const std::string& token, std::map<std::string, boost::json::object>& dogs);

private:
    std::random_device random_device_;                                                      // „тобы сгенерировать токен, получите из generator1_ и generator2_
    std::mt19937_64 generator1_{ [this] {                                                   // два 64-разр€дных числа и, перевед€ их в hex-строки, склейте в одну.
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;                   // ¬ы можете поэкспериментировать с алгоритмом генерировани€ токенов, 
        return dist(random_device_);                                                         // чтобы сделать их подбор ещЄ более затруднительным
    }() };  
    std::mt19937_64 generator2_{ [this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }() };

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

    Map* FindMap(const Map::Id& id) {
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

    void UpdateGameSessions(const int64_t& ticks) {
        for (auto &session : gameSessions_) {
            session.UpdateGameState(ticks);
        }
    }

    PlayerTokens& GetPlayerTokens() {
        return tokens_;
    }

    void SetDefaultSpeed(RealDimension speed) {
        default_speed_ = speed;
    }

    void SetMapCrossRoads();

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
