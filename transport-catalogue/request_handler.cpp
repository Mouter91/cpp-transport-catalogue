#include "request_handler.h"

RequestHandler::RequestHandler(TransportCatalogue& cat) : db_(cat) {}

std::optional<BusStat> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
    auto bus_ptr = db_.GetRoute(bus_name);

    if (bus_ptr == nullptr) {
        return std::nullopt;
    }

    std::set<std::string> unique_stations;
    size_t stops_on_route = bus_ptr->route.size();
    double geo_distance = 0.0;
    double distance = 0.0;

    for (const auto& station : bus_ptr->route) {
        unique_stations.insert(station->name_station_);
    }

    for (size_t i = 1; i < stops_on_route; ++i) {
        distance += db_.GetDistance(bus_ptr->route[i - 1]->name_station_, bus_ptr->route[i]->name_station_);
        geo_distance += ComputeDistance(bus_ptr->route[i - 1]->coord_station_,
                                        bus_ptr->route[i]->coord_station_);
    }

    double curvature = distance / geo_distance;

    BusStat response{static_cast<int>(stops_on_route), static_cast<int>(unique_stations.size()), distance, curvature};


    return response;
}

std::optional<std::set<std::string>> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
    auto buses = db_.GetBuses(stop_name);
    std::set<std::string> num_bus;

    if (buses.empty()) {
        auto stations_ptr = db_.GetStations(stop_name);
        if(stations_ptr == nullptr) {
            return std::nullopt;
        } else {
            return num_bus;
        }
    }

    for (const auto& ptr_bus : buses) {
        num_bus.insert(ptr_bus->number_bus_);
    }

    return num_bus;
}


std::set<const Bus*, RequestHandler::BusComparator> RequestHandler::GetAutopark() {
    const auto& buses = db_.GetAllBus();
    std::set<const Bus*, RequestHandler::BusComparator> bus_set;

    for (const auto& bus : buses) {
        bus_set.insert(&bus); // вставляем указатели на элементы
    }

    return bus_set;
}
