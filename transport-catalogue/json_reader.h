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

    void GetRenderRoute(svg::Document&);
    void GetRenderNameRoute(svg::Document&);
    void GetRenderSimStop(svg::Document&);
    std::string PrintRender();



private:
    json::Document jsonDoc;
    TransportCatalogue& catalogue;
    RequestHandler request;

    LoadRender render_set;
    std::set<BusRender> render_map;
};
