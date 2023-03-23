#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include <sstream>
#include <boost/json.hpp>
#include "tagged.h"

namespace model {

using Dimension = int64_t;
using Coord = Dimension;

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

private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    Roads roads_;
    Buildings buildings_;

    OfficeIdToIndex warehouse_id_to_index_;
    Offices offices_;
};

class Dog {
public:
    Dog() = default;

    Dog(std::string& name)
        :name_(name) {}

    const std::string& GetDogName() {
        return name_;
    }

private:
    std::string name_;
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

    const Map* GetMapFromSession() {
        return map_;
    }

    std::uint64_t GetLastDogIndex() const {
        return lastDogIndex_;
    }

    const Dog& GetDogByIndex(std::uint64_t index) const {
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
     Player(const GameSession* sesion, std::uint64_t& dogId)
        :sesion_(sesion),
         dog_(sesion->GetDogByIndex(dogId)),
         playerId_(dogId){}

    const Dog& GetPlayerDog() {
        return dog_;
    }

    std::uint64_t GetPlayerId() {
        return playerId_;
    }

    const GameSession* GetPlayerSession() {
        return sesion_;
    }

private:
    const GameSession* sesion_;
    const Dog& dog_;
    std::uint64_t playerId_;
};

class PlayerTokens {
public:
    PlayerTokens()
        : lastPlayerIndex_(0) {
    }

    std::string GenerateToken() {
        std::ostringstream ss;
        ss << std::hex << generator1_();
        ss << std::hex << generator2_();
        std::string result = ss.str();
        return result;
    }

    void AddPlayer(const std::string& token, Player player) {
        playerTokens_.insert({ token, player });
        ++lastPlayerIndex_;
    }

    bool IsTokenExist(const std::string& token) {
        return playerTokens_.contains(token);
    }

    void FindPlayersBy(const std::string& token, boost::json::object& json, std::map<std::string, std::string>& players) {        
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
    // ����� ������������� �����, �������� �� generator1_ � generator2_
    // ��� 64-��������� ����� �, �������� �� � hex-������, ������� � ����.
    // �� ������ �������������������� � ���������� ������������� �������,
    // ����� ������� �� ������ ��� ����� ���������������
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

private:
    using MapIdHasher = util::TaggedHasher<Map::Id>;
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;

    std::vector<Map> maps_;
    std::vector<GameSession> gameSessions_;
    MapIdToIndex map_id_to_index_;
    PlayerTokens tokens_;
};

}  // namespace model
