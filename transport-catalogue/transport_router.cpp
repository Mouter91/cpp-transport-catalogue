#include "transport_router.h"

    TransportRoute::TransportRoute(const TransportCatalogue& catalogue)
        : catalogue_(catalogue), graph_(catalogue.GetAllStops().size() * 2) {
        BuildGraph();
        router = std::make_unique<graph::Router<EdgeInfo>>(graph_);
    }

    void TransportRoute::AddEdgeWaiting() {
        const auto& stops = catalogue_.GetAllStops();
        const auto* route_settings = catalogue_.GetRouteSetting();

        for (size_t i = 0; i < stops.size(); ++i) {
            size_t start_waiting = i * 2;      // Вершина начала ожидания
            size_t stop_waiting = i * 2 + 1;  // Вершина окончания ожидания

            vertex_stop[stops[i].name_station_] = {start_waiting, stop_waiting};

            graph_.AddEdge({
                start_waiting,  // Вершина начала ожидания
                stop_waiting,   // Вершина окончания ожидания
                EdgeInfo{static_cast<double>(route_settings->bus_wait_time), 0}  // Время ожидания
            });

            edge_info_.push_back({
                static_cast<double>(route_settings->bus_wait_time),
                0,
                stops[i].name_station_,
                false
            });
        }
    }

    void TransportRoute::AddEdgeTravel() {
        const auto& buses = catalogue_.GetAllBus();
        const auto* route_settings = catalogue_.GetRouteSetting();

        for (const auto& bus : buses) {
            const auto& route_bus = bus.route;
            if (route_bus.empty()) continue;

            size_t route_size = route_bus.size();
            for (int i = 0; i < route_size - 1; ++i) {

                const auto [in_now, out_now] = vertex_stop.at(route_bus[i]->name_station_);
                double time_travel = 0;
                for(int j = i + 1; j < route_size; ++j) {

                    const auto [in_next, out_next] = vertex_stop.at(route_bus[j]->name_station_);
                    double distance = catalogue_.GetDistance(route_bus[j - 1]->name_station_, route_bus[j]->name_station_);
                    time_travel += distance * 60 / (route_settings->bus_velocity * 1000);

                    graph_.AddEdge({
                        out_now, in_next, EdgeInfo{time_travel, j - i}
                    });
                    edge_info_.push_back({
                        time_travel, j - i, bus.number_bus_, true
                    });
                }
            }
        }
    }

    void TransportRoute::BuildGraph() {
        AddEdgeWaiting();
        AddEdgeTravel();
    }

    std::optional<TransportRoute::RouteInfo> TransportRoute::GetRouteInfo(std::string_view from, std::string_view to) {
        auto it_from = vertex_stop.find(from);
        auto it_to = vertex_stop.find(to);

        if (it_from == vertex_stop.end() || it_to == vertex_stop.end()) {
            return std::nullopt;
        }

        size_t id_from = it_from->second.in_vertex;
        size_t id_to = it_to->second.in_vertex;

        auto info_route = router->BuildRoute(id_from, id_to);

        if (!info_route.has_value()) {
            return std::nullopt;
        }

        double total_time = info_route->weight.time_wait;
        std::vector<EdgeInfo> result;

        const auto& edge_route = info_route->edges;

        for (size_t i = 0; i < edge_route.size(); ++i) {
            EdgeInfo edge = edge_info_[edge_route[i]];
            result.push_back(edge);
        }

        return RouteInfo{total_time, std::move(result)};
    }

