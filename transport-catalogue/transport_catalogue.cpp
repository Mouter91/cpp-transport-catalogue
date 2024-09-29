#include "transport_catalogue.h"

namespace transport_catalogue {
namespace routing {

Station::Station(std::string_view name_station, coordinates_station::Coordinates coordinates)
    : name_station_(std::move(name_station)), coord_station_(coordinates) {}

Station::Station(std::string_view name_station)
    : name_station_(std::move(name_station)), coord_station_{0.0, 0.0} {}

std::string Station::GetNameStation() const {
    return name_station_;
}

void Station::SetNameStation(std::string_view name_station) {
    name_station_ = name_station;
}

coordinates_station::Coordinates Station::GetCoordinateStation() const {
    return coord_station_;
}

void Station::SetCoordinateStation(coordinates_station::Coordinates coordinates) {
    coord_station_ = coordinates;
}

Bus::Bus(std::string_view number_bus): number_bus_(std::move(number_bus)) {}

std::string Bus::GetNumberBus() const {
    return number_bus_;
}

bool Bus::operator<(const Bus& other) const {
    return number_bus_ < other.number_bus_;
}

}

void TransportCatalogue::AddRouteBus(std::string_view bus_number, std::vector<std::string_view> route) {

    std::vector<std::shared_ptr<routing::Station>> tmp;
    tmp.reserve(route.size());

    std::shared_ptr<routing::Bus> bus = std::make_shared<routing::Bus>(bus_number);

    for (auto& stop_name : route) {
        std::string stop_name_str(stop_name);
        auto station_it = stations_.find(stop_name_str);
        std::shared_ptr<routing::Station> station;

        if (station_it == stations_.end()) {
            station = std::make_shared<routing::Station>(stop_name);
            stations_.emplace(stop_name_str, station);
        } else {
            station = station_it->second;
        }

        buses_at_station_[stop_name_str].insert(bus);
        tmp.push_back(station);
    }

    route_.emplace(bus_number, std::move(tmp));
}


void TransportCatalogue::AddStation(std::string_view stop_name, coordinates_station::Coordinates coord) {

    std::string stop_name_str(stop_name);
    auto station_it = stations_.find(stop_name_str);

    if (station_it == stations_.end()) {
        auto station = std::make_shared<routing::Station>(stop_name, coord);
        stations_.emplace(stop_name_str, station);
        buses_at_station_.emplace(stop_name_str, std::unordered_set<std::shared_ptr<routing::Bus>>());
    } else {
        if (station_it->second->GetCoordinateStation() == coordinates_station::Coordinates{0.0, 0.0}) {
            station_it->second->SetCoordinateStation(coord);
        }
    }
}


const std::vector<std::shared_ptr<routing::Station>>& TransportCatalogue::GetRoute(std::string_view bus_number) const {

    static const std::vector<std::shared_ptr<routing::Station>> empty_route;
    auto it = route_.find(bus_number.data());

    if (it == route_.end()) {
        return empty_route;
    }

    return it->second;
}

const std::unordered_set<std::shared_ptr<routing::Bus>>& TransportCatalogue::GetBuses(std::string_view name_station) const {

    static const std::unordered_set<std::shared_ptr<routing::Bus>> empty_set;
    auto it = buses_at_station_.find(name_station.data());

    if (it == buses_at_station_.end()) {
        return empty_set;
    }

    return it->second;
}

const std::unordered_map<std::string, std::shared_ptr<routing::Station>> TransportCatalogue::GetStations() const {
    return stations_;
}
}
