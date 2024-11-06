#pragma once

#include "transport_catalogue.h"
#include "domain.h"
#include <optional>
#include <set>


class RequestHandler {
public:

    struct BusComparator {
        bool operator()(const Bus* lhs, const Bus* rhs) const {
            return lhs->number_bus_ < rhs->number_bus_;
        }
    };

    RequestHandler(TransportCatalogue& cat);
    std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    std::optional<std::set<std::string>> GetBusesByStop(const std::string_view& stop_name) const;

    std::set<const Bus*, BusComparator> GetAutopark();

private:

    const TransportCatalogue& db_;

};
