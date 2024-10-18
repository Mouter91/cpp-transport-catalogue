#pragma once
#include <deque>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

#include "geo.h"

namespace transport_catalogue {
namespace routing {

struct Station {

    Station(std::string_view, coordinates_station::Coordinates, std::unordered_map<std::string, int64_t>);

    std::string name_station_;
    coordinates_station::Coordinates coord_station_;
    std::unordered_map<std::string, int64_t> to_station_;
};

struct Bus {

    std::string number_bus_;
    std::vector<Station*> route;
};
}

class TransportCatalogue {
public:
    void AddRouteBus(std::string_view bus_number, const std::vector<std::string_view>& stops);
    void AddStation(std::string_view stop_name, coordinates_station::Coordinates coord, std::unordered_map<std::string, int64_t> &dis_next_station);

    const routing::Bus* GetRoute(std::string_view) const;
    const std::unordered_set<routing::Bus*>& GetBuses(std::string_view) const;
    const routing::Station* GetStations(std::string_view) const;

private:
    std::unordered_map<std::string_view, std::unordered_set<routing::Bus*>> buses_at_station_;
    std::unordered_map<std::string_view, routing::Station*> station_;
    std::unordered_map<std::string_view, routing::Bus*> route_;

    std::deque<routing::Station> stop_;
    std::deque<routing::Bus> buses_;
};
}
