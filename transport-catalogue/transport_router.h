#pragma once

#include "transport_catalogue.h"
#include "graph.h"
#include "router.h"

#include <cstdint>
#include <unordered_map>
#include <variant>
#include <memory>
#include <optional>
#include <vector>

class TransportRoute {
private:

    struct EdgeInfo {
        double time_wait;
        int span = 0;
        std::string_view name;
        bool is_bus;

        bool operator<(const EdgeInfo& other) const {
            return time_wait < other.time_wait;
        }

        EdgeInfo operator+(const EdgeInfo& other) const {
            return EdgeInfo{time_wait + other.time_wait, span};
        }

        bool operator>(const EdgeInfo& other) const {
            return time_wait > other.time_wait;
        }
    };

    struct RouteInfo {
        double total_time;
        std::vector<EdgeInfo> route_info;
    };

    struct InOutVertex {
        size_t in_vertex;
        size_t out_vertex;
    };

public:

    TransportRoute(const TransportCatalogue& catalogue);
    void BuildGraph();
    std::optional<RouteInfo> GetRouteInfo(std::string_view from, std::string_view to);


private:

    const TransportCatalogue& catalogue_;
    graph::DirectedWeightedGraph<EdgeInfo> graph_;
    std::unique_ptr<graph::Router<EdgeInfo>> router;

    std::unordered_map<std::string_view, InOutVertex> vertex_stop;
    std::vector<EdgeInfo> edge_info_;

};
