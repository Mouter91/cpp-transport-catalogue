#pragma once
#include <optional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

#include "geo.h"

namespace transport_catalogue {
namespace routing {

class Station {
public:
    Station(std::string_view name_station, coordinates_station::Coordinates coordinates);
    Station(std::string_view name_station);

    std::string GetNameStation() const;
    void SetNameStation(std::string_view name_station);

    coordinates_station::Coordinates GetCoordinateStation() const;
    void SetCoordinateStation(coordinates_station::Coordinates coordinates);

private:
    std::string name_station_;
    coordinates_station::Coordinates coord_station_{0.0, 0.0};
};

class Bus {

public:
    Bus(std::string_view);
    std::string GetNumberBus() const;
    bool operator<(const Bus& other) const;

private:
    std::string number_bus_;
};
}

class TransportCatalogue {

public:
    void AddRouteBus(std::string_view, std::vector<std::string_view>);
    void AddStation(std::string_view, coordinates_station::Coordinates);

    const std::vector<std::shared_ptr<routing::Station>>& GetRoute(std::string_view) const;
    const std::unordered_map<std::string, std::shared_ptr<routing::Station>> GetStations() const;
    const std::unordered_set<std::shared_ptr<routing::Bus>>& GetBuses(std::string_view) const;

private:
    std::unordered_map<std::string, std::unordered_set<std::shared_ptr<routing::Bus>>> buses_at_station_;
    std::unordered_map<std::string, std::vector<std::shared_ptr<routing::Station>>> route_;
    std::unordered_map<std::string, std::shared_ptr<routing::Station>> stations_;
};
}
