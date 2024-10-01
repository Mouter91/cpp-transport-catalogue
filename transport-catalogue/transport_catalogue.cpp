#include "transport_catalogue.h"

namespace transport_catalogue {
namespace routing {
Station::Station(std::string_view name, coordinates_station::Coordinates coord)
    : name_station_(std::move(name)), coord_station_(coord) {}

}

void TransportCatalogue::AddRouteBus(std::string_view bus_number, const std::vector<std::string_view>& route) {
    std::vector<routing::Station*> tmp;
    tmp.reserve(route.size());

    buses_.emplace_back(routing::Bus{static_cast<std::string>(bus_number), std::move(tmp)});
    routing::Bus& bus = buses_.back();

    for (auto& stop_name : route) {
        auto station_ptr = station_.at(stop_name);
        buses_at_station_[station_ptr->name_station_].insert(&bus);
        bus.route.push_back(station_ptr);
    }

    route_.emplace(bus.number_bus_, &bus);
}



void TransportCatalogue::AddStation(std::string_view stop_name, coordinates_station::Coordinates coord) {

    stop_.emplace_back(static_cast<std::string>(stop_name), std::move(coord));
    routing::Station& station = stop_.back();
    station_.emplace(station.name_station_, &station);
}


const routing::Bus* TransportCatalogue::GetRoute(std::string_view bus_number) const {
    auto it = route_.find(bus_number);

    if (it == route_.end()) {
        return nullptr;
    }
    return it->second;
}

const std::unordered_set<routing::Bus*>& TransportCatalogue::GetBuses(std::string_view name_station) const {

    static const std::unordered_set<routing::Bus*> empty_set;
    auto it = buses_at_station_.find(name_station);

    if (it == buses_at_station_.end()) {
        return empty_set;
    }

    return it->second;
}

const routing::Station* TransportCatalogue::GetStations(std::string_view station) const {
    auto it = station_.find(station);
    if (it == station_.end()) {
        return nullptr;
    }
    return it->second;
}
}
