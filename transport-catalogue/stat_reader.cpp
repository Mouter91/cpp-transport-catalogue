#include "stat_reader.h"

namespace transport_catalogue {
namespace request_redactor {

std::pair<std::string_view, std::string_view> SplitRequest(std::string_view request) {

    request.remove_prefix(std::min(request.find_first_not_of(' '), request.size()));
    size_t first_word_end = request.find(' ');

    if (first_word_end == std::string_view::npos) {
        return {request, ""};
    }

    std::string_view first_word = request.substr(0, first_word_end);
    request.remove_prefix(first_word_end + 1);
    request.remove_prefix(std::min(request.find_first_not_of(' '), request.size()));

    std::string_view second_word = request;

    return {first_word, second_word};
}
}
namespace input_output_request {

void PrintStatBus(const TransportCatalogue& transport_catalogue, std::string_view request_bus,
                  std::ostream& output) {

    auto bus_ptr = transport_catalogue.GetRoute(request_bus);

    if (bus_ptr == nullptr) {
        output << "Bus " << request_bus << ": not found" << std::endl;
        return;
    }

    std::set<std::string> unique_stations;
    size_t stops_on_route = bus_ptr->route.size();
    double distance = 0.0;

    for (const auto& station : bus_ptr->route) {
        unique_stations.insert(station->name_station_);
    }

    for (size_t i = 1; i < stops_on_route; ++i) {
        distance += ComputeDistance(bus_ptr->route[i-1]->coord_station_,
                                    bus_ptr->route[i]->coord_station_);
    }

    output << "Bus " << request_bus << ": " << stops_on_route << " stops on route, "
           << unique_stations.size() << " unique stops, "
           << distance << " route length" << std::endl;

}

void PrintStatStop(const TransportCatalogue& transport_catalogue, std::string_view request_stop,
                   std::ostream& output) {

    auto buses = transport_catalogue.GetBuses(request_stop);

    if (buses.empty()) {

        auto stations_ptr = transport_catalogue.GetStations(request_stop);

        if (stations_ptr == nullptr) {
            output << "Stop " << request_stop << ": not found" << std::endl;
        } else {
            output << "Stop " << request_stop << ": no buses" << std::endl;
        }
    } else {

        output << "Stop " << request_stop << ": buses ";
        std::set<std::string> num_bus;
        for(const auto& ptr_bus : buses) {
            num_bus.insert(ptr_bus->number_bus_);
        }

        for(auto it = num_bus.begin(); it != num_bus.end(); ++it) {
            if(it != num_bus.begin()) {
                output << " ";
            }
            output << *it;
        }
        output << std::endl;
    }

}

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output) {


    auto parse_request = request_redactor::SplitRequest(request);

    if (parse_request.first == "Bus") {

        PrintStatBus(transport_catalogue, parse_request.second, output);

    }

    if (parse_request.first == "Stop") {
        PrintStatStop(transport_catalogue, parse_request.second, output);
    }

}
}
}
