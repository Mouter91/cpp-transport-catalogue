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
void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output) {


    auto parse_request = request_redactor::SplitRequest(request);

    if (parse_request.first == "Bus") {
        std::string_view route_name = parse_request.second;
        auto route = transport_catalogue.GetRoute(route_name);
        if (route.empty()) {
            output << request << ": not found" << std::endl;
            return;
        }

        std::set<std::string> unique_stations;
        size_t stops_on_route = route.size();
        double distance = 0.0;

        for (const auto& station : route) {
            unique_stations.insert(station->GetNameStation());
        }

        for (size_t i = 1; i < stops_on_route; ++i) {
            distance += ComputeDistance(route[i - 1]->GetCoordinateStation(),
                                        route[i]->GetCoordinateStation());
        }

        output << request << ": " << stops_on_route << " stops on route, "
               << unique_stations.size() << " unique stops, "
               << distance << " route length" << std::endl;

    } else if (parse_request.first == "Stop") {
        std::string_view station_name = parse_request.second;
        auto buses = transport_catalogue.GetBuses(station_name);

        if (buses.empty()) {
            const auto& stations = transport_catalogue.GetStations();
            std::string station_name_str(station_name);

            auto station_it = stations.find(station_name_str);
            if (station_it == stations.end()) {
                output << request << ": not found" << std::endl;
            } else {
                output << request << ": no buses" << std::endl;
            }
        } else {

            output << request << ": buses ";
            std::set<std::string> num_bus;
            for(const auto& ptr_bus : buses) {
                num_bus.insert(ptr_bus->GetNumberBus());
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

}
}
}
