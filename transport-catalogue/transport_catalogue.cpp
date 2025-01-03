#include "transport_catalogue.h"

void TransportCatalogue::AddRouteBus(std::string_view bus_number, const std::vector<std::string_view>& route, bool is_roundtrip) {
    std::vector<Station*> tmp;
    tmp.reserve(route.size());

    buses_.emplace_back(Bus{static_cast<std::string>(bus_number), std::move(tmp), is_roundtrip});
    Bus& bus = buses_.back();
    if(is_roundtrip) {
        for (auto& stop_name : route) {
            auto station_ptr = station_.at(stop_name);
            buses_at_station_[station_ptr->name_station_].insert(&bus);
            bus.route.push_back(station_ptr);
        }
    } else {
        for (size_t i = 0; i < route.size(); ++i) {
            auto station_ptr = station_.at(route[i]);
            buses_at_station_[station_ptr->name_station_].insert(&bus);
            bus.route.push_back(station_ptr);
        }
        bus.route.insert(bus.route.end(), bus.route.rbegin() + 1, bus.route.rend());
    }

    route_.emplace(bus.number_bus_, &bus);
}

void TransportCatalogue::AddStation(std::string_view stop_name,
                                    geo::Coordinates coord)
{
    stop_.emplace_back(static_cast<std::string>(stop_name), std::move(coord));
    Station& station = stop_.back();
    station_.emplace(station.name_station_, &station);
}

const Bus* TransportCatalogue::GetRoute(std::string_view bus_number) const {
    auto it = route_.find(bus_number);

    if (it == route_.end()) {
        return nullptr;
    }
    return it->second;
}

void TransportCatalogue::AddToStation(std::string_view from_station, std::string_view to_station, int64_t distance) {

    std::string_view from = station_.at(from_station)->name_station_;
    std::string_view to = station_.at(to_station)->name_station_;

    station_to_.emplace(std::make_pair(from, to), distance);
}

const std::unordered_set<Bus*>& TransportCatalogue::GetBuses(std::string_view name_station) const {

    static const std::unordered_set<Bus*> empty_set;
    auto it = buses_at_station_.find(name_station);

    if (it == buses_at_station_.end()) {
        return empty_set;
    }

    return it->second;
}

const Station* TransportCatalogue::GetStations(std::string_view station) const {
    auto it = station_.find(station);
    if (it == station_.end()) {
        return nullptr;
    }
    return it->second;
}

int64_t TransportCatalogue::GetDistance(std::string_view from, std::string_view to) const {

    auto route_from_to = std::make_pair(from, to);
    auto route_from_to_rev = std::make_pair(to, from);

    auto it = station_to_.find(route_from_to);
    if (it != station_to_.end()) {
        return it->second;
    }

    it = station_to_.find(route_from_to_rev);
    if (it != station_to_.end()) {
        return it->second;
    }

    return 0;
}

void TransportCatalogue::SetRouteSetting(const size_t bus_wait, const size_t bus_velocity) {
    route_settings_ = { bus_wait, bus_velocity };
}
const RouteSetting* TransportCatalogue::GetRouteSetting() const {
    return &route_settings_;
}

const std::deque<Bus>& TransportCatalogue::GetAllBus() const {
    return buses_;
}

