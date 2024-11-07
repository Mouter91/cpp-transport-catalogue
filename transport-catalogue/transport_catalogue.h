#pragma once
#include <deque>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cstdint>

#include "geo.h"
#include "domain.h"


struct PairHash {
    std::size_t operator()(const std::pair<std::string_view, std::string_view>& p) const {
        std::hash<const void*> ptr_hash;

        std::size_t h1 = ptr_hash(static_cast<const void*>(p.first.data()));
        std::size_t h2 = ptr_hash(static_cast<const void*>(p.second.data()));

        return h1 ^ (h2 << 1);
    }
};


class TransportCatalogue {
public:
    void AddRouteBus(std::string_view bus_number, const std::vector<std::string_view>& stops, bool is_roundtrip);
    void AddStation(std::string_view stop_name, geo::Coordinates coord);
    void AddToStation(std::string_view, std::string_view, int64_t);

    const Bus* GetRoute(std::string_view) const;
    const std::unordered_set<Bus*>& GetBuses(std::string_view) const;
    const Station* GetStations(std::string_view) const;
    int64_t GetDistance(std::string_view, std::string_view) const;
    const std::deque<Bus>& GetAllBus() const;

private:
    std::unordered_map<std::string_view, std::unordered_set<Bus*>> buses_at_station_;
    std::unordered_map<std::string_view, Station*> station_;
    std::unordered_map<std::string_view, Bus*> route_;
    std::unordered_map<std::pair<std::string_view, std::string_view>, int64_t, PairHash> station_to_;

    std::deque<Station> stop_;
    std::deque<Bus> buses_;
};

