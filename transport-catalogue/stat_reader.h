#pragma once

#include <iosfwd>
#include <string_view>
#include <iostream>
#include <set>

#include "transport_catalogue.h"
#include "geo.h"

namespace transport_catalogue {
namespace input_output_request {
void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output);
}
}
