#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "geo.h"

struct Station {

    Station(const std::string& name, geo::Coordinates coord = {0.0, 0.0}) : name_station_(name), coord_station_(coord) {}

    std::string name_station_;
    geo::Coordinates coord_station_{0.0, 0.0};
};

struct Bus {

    std::string number_bus_;
    std::vector<Station*> route;
    bool is_roundtrip;
};


struct BusStat {
    int stops_on_route;
    int unique_stations;
    double distance;
    double curvature;
};
