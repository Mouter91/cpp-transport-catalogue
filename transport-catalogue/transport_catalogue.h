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

    Station(std::string_view, coordinates_station::Coordinates);

    std::string name_station_;
    coordinates_station::Coordinates coord_station_;
};

struct Bus {

    std::string number_bus_;
    std::vector<Station*> route;
};

struct PairHash {
    std::size_t operator()(const std::pair<std::string_view, std::string_view>& p) const {
        std::hash<const void*> ptr_hash;

        std::size_t h1 = ptr_hash(static_cast<const void*>(p.first.data()));
        std::size_t h2 = ptr_hash(static_cast<const void*>(p.second.data()));

        return h1 ^ (h2 << 1);
    }
};


}

class TransportCatalogue {
public:
    void AddRouteBus(std::string_view bus_number, const std::vector<std::string_view>& stops);
    void AddStation(std::string_view stop_name, coordinates_station::Coordinates coord);
    void AddToStation(std::string_view, std::string_view, int64_t);

    const routing::Bus* GetRoute(std::string_view) const;
    const std::unordered_set<routing::Bus*>& GetBuses(std::string_view) const;
    const routing::Station* GetStations(std::string_view) const;
    const int64_t GetDistance(std::string_view, std::string_view) const;

private:
    std::unordered_map<std::string_view, std::unordered_set<routing::Bus*>> buses_at_station_;
    std::unordered_map<std::string_view, routing::Station*> station_;
    std::unordered_map<std::string_view, routing::Bus*> route_;
    std::unordered_map<std::pair<std::string_view, std::string_view>, int64_t, routing::PairHash> station_to_;

    std::deque<routing::Station> stop_;
    std::deque<routing::Bus> buses_;
};
}
