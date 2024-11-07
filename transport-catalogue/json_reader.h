#pragma once
#include <sstream>
#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "request_handler.h"

class LoadJson {
public:
    LoadJson(std::istream& input, TransportCatalogue& catalogue);

    void LoadStops(const json::Node& baseRequests);
    void LoadBuses(const json::Node& baseRequests);
    void LoadRenderSettings();

    void GetReply();
    void LoadRequest(const json::Node& statRequests);

    std::string PrintRender();



private:
    json::Document jsonDoc;
    TransportCatalogue& catalogue;
    RequestHandler request;
    MapRender render;

    LoadRender render_set;
};
