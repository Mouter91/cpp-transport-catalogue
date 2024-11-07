#include "json_reader.h"
#include <iostream>


LoadJson::LoadJson(std::istream& input, TransportCatalogue& catalogue): catalogue(catalogue), request(catalogue) {
    try{
        jsonDoc = json::Load(input);
        const auto& root = jsonDoc.GetRoot();

        if (!root.IsMap() || !root.AsMap().count("base_requests")) {
            throw std::logic_error("Invalid JSON structure");
        }

        const auto& baseRequests = root.AsMap().at("base_requests");
        LoadStops(baseRequests);
        LoadBuses(baseRequests);
        LoadRenderSettings();
    } catch (const json::ParsingError& e) {
        std::cerr << "Ошибка парсинга JSON: " << e.what() << std::endl;
    }
}

void LoadJson::LoadStops(const json::Node& baseRequests) {

    for (const auto& node : baseRequests.AsArray()) {
        if (!node.IsMap() || !node.AsMap().count("type") || node.AsMap().at("type").AsString() != "Stop") {
            continue;
        }

        std::string_view name = node.AsMap().at("name").AsString();
        geo::Coordinates coordinates = {node.AsMap().at("latitude").AsDouble(), node.AsMap().at("longitude").AsDouble()};

        catalogue.AddStation(name, coordinates);

    }

    for (const auto& node : baseRequests.AsArray()) {
        if (!node.IsMap() || !node.AsMap().count("type") || node.AsMap().at("type").AsString() != "Stop") {
            continue;
        }

        std::string name = node.AsMap().at("name").AsString();
        const auto& roadDistances = node.AsMap().at("road_distances").AsMap();
        for (const auto& [key, value] : roadDistances) {
            catalogue.AddToStation(name, key, value.AsInt());
        }
    }

}

void LoadJson::LoadBuses(const json::Node& baseRequests) {

    for (const auto& node : baseRequests.AsArray()) {
        if (!node.IsMap() || !node.AsMap().count("type") || node.AsMap().at("type").AsString() != "Bus") {
            continue;
        }

        std::string_view name = node.AsMap().at("name").AsString();
        bool is_roundtrip = node.AsMap().at("is_roundtrip").AsBool();

        std::vector<std::string_view> route;
        const auto& stops = node.AsMap().at("stops").AsArray();
        for (const auto& stopNode : stops) {
            route.push_back(stopNode.AsString());
        }
        catalogue.AddRouteBus(name, route, is_roundtrip);
    }

}


void LoadJson::GetReply() {
    const auto& root = jsonDoc.GetRoot();
    if (!root.IsMap() || !root.AsMap().count("stat_requests")) {
        throw std::logic_error("Invalid JSON structure");
    }
    const auto& statRequests = root.AsMap().at("stat_requests");
    LoadRequest(statRequests);
}

void LoadJson::LoadRequest(const json::Node& statRequests) {

    json::Array responseArray;

    for (const auto& node : statRequests.AsArray()) {
        if (!node.IsMap() || !node.AsMap().count("id") || !node.AsMap().count("type")) {
            throw std::logic_error("Invalid request format");
        }
        json::Dict dict;
        dict["request_id"] = json::Node(node.AsMap().at("id").AsInt());

        std::string_view requestType = node.AsMap().at("type").AsString();

        if(requestType == "Map") {
            dict["map"] =  json::Node(PrintRender());
            auto pt = json::Node(std::move(dict));
            responseArray.push_back(pt);
        }

        if (requestType == "Stop") {
            std::string_view name = node.AsMap().at("name").AsString();
            auto stopStat = request.GetBusesByStop(name);

            if (stopStat.has_value()) {
                json::Array busesArray;
                for (const auto& bus : *stopStat) {
                    busesArray.push_back(json::Node(bus));
                }
                dict["buses"] = json::Node(std::move(busesArray));

            } else {
                dict["error_message"] = json::Node(std::string("not found"));
            }

            responseArray.push_back(json::Node(std::move(dict)));

        } else if (requestType == "Bus") {
            std::string_view name = node.AsMap().at("name").AsString();
            auto bus_stat = request.GetBusStat(name);
            if(bus_stat.has_value()) {
                dict["curvature"] = json::Node(bus_stat->curvature);
                dict["route_length"] = json::Node(bus_stat->distance);
                dict["stop_count"] = json::Node(bus_stat->stops_on_route);
                dict["unique_stop_count"] = json::Node(bus_stat->unique_stations);
            } else {
                dict["error_message"] = json::Node(std::string("not found"));
            }
            responseArray.push_back(json::Node(std::move(dict)));

        }
    }


    json::Node responseNode(std::move(responseArray));
    json::Print(json::Document(responseNode), std::cout);
}

svg::Color ParseColor(const json::Node& color_node) {
    if (color_node.IsString()) {
        return svg::Color(color_node.AsString());
    } else if (color_node.IsArray()) {
        const auto& color_array = color_node.AsArray();
        if (color_array.size() == 3) {

            uint8_t r = color_array[0].AsInt();
            uint8_t g = color_array[1].AsInt();
            uint8_t b = color_array[2].AsInt();
            return svg::Rgb{r, g, b};
        } else if (color_array.size() == 4) {

            uint8_t r = color_array[0].AsInt();
            uint8_t g = color_array[1].AsInt();
            uint8_t b = color_array[2].AsInt();
            double a = color_array[3].AsDouble();
            return svg::Rgba{r, g, b, a};
        }
    }
    throw std::logic_error("Invalid color format");
}

void LoadJson::LoadRenderSettings() {
    const auto& root = jsonDoc.GetRoot();
    if (!root.IsMap() || !root.AsMap().count("render_settings")) {
        throw std::logic_error("Invalid JSON structure");
    }

    const auto& render_settings = root.AsMap().at("render_settings");

    render_set.width = render_settings.AsMap().at("width").AsDouble();
    render_set.height = render_settings.AsMap().at("height").AsDouble();
    render_set.padding = render_settings.AsMap().at("padding").AsDouble();

    render_set.line_width = render_settings.AsMap().at("line_width").AsDouble();
    render_set.stop_radius = render_settings.AsMap().at("stop_radius").AsDouble();

    render_set.bus_label_font_size = render_settings.AsMap().at("bus_label_font_size").AsDouble();

    std::vector<double> bus_label_offset;
    for (const auto& value : render_settings.AsMap().at("bus_label_offset").AsArray()) {
        bus_label_offset.push_back(value.AsDouble());
    }

    render_set.bus_label_offset = svg::Point(bus_label_offset[0],bus_label_offset[1]);

    render_set.stop_label_font_size = render_settings.AsMap().at("stop_label_font_size").AsDouble();
    std::vector<double> stop_label_offset;
    for (const auto& value : render_settings.AsMap().at("stop_label_offset").AsArray()) {
        stop_label_offset.push_back(value.AsDouble());
    }

    render_set.stop_label_offset = svg::Point(stop_label_offset[0], stop_label_offset[1]);

    const auto& underlayer_color_node = render_settings.AsMap().at("underlayer_color");
    render_set.underlayer_color = ParseColor(underlayer_color_node);

    render_set.underlayer_width = render_settings.AsMap().at("underlayer_width").AsDouble();

    const auto& color_array = render_settings.AsMap().at("color_palette").AsArray();
    std::vector<svg::Color> color_palette;

    for (const auto& color_item : color_array) {
        color_palette.emplace_back(ParseColor(color_item));
    }
    render_set.color_palette = std::move(color_palette);
}


std::string LoadJson::PrintRender() {
    svg::Document route_render;

    std::ostringstream map;
    render.SetRender(render_set);
    render.GetRenderRoute(route_render, request);
    render.GetRenderNameRoute(route_render);
    render.GetRenderSimStop(route_render);

    route_render.Render(map);
    std::string result = map.str();
    return result;
}
