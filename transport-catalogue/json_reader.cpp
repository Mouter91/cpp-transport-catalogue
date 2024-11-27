#include "json_reader.h"
#include <iostream>


LoadJson::LoadJson(std::istream& input, TransportCatalogue& catalogue): catalogue(catalogue), request(catalogue) {
    try{
        jsonDoc = json::Load(input);
        const auto& root = jsonDoc.GetRoot();

        if (!root.IsDict() || !root.AsDict().count("base_requests")) {
            throw std::logic_error("Invalid JSON structure");
        }

        const auto& baseRequests = root.AsDict().at("base_requests");
        LoadStops(baseRequests);
        LoadBuses(baseRequests);
        LoadRenderSettings();
    } catch (const json::ParsingError& e) {
        std::cerr << "Ошибка парсинга JSON: " << e.what() << std::endl;
    }
}

void LoadJson::LoadStops(const json::Node& baseRequests) {

    for (const auto& node : baseRequests.AsArray()) {
        if (!node.IsDict() || !node.AsDict().count("type") || node.AsDict().at("type").AsString() != "Stop") {
            continue;
        }

        std::string_view name = node.AsDict().at("name").AsString();
        geo::Coordinates coordinates = {node.AsDict().at("latitude").AsDouble(), node.AsDict().at("longitude").AsDouble()};

        catalogue.AddStation(name, coordinates);

    }

    for (const auto& node : baseRequests.AsArray()) {
        if (!node.IsDict() || !node.AsDict().count("type") || node.AsDict().at("type").AsString() != "Stop") {
            continue;
        }

        std::string name = node.AsDict().at("name").AsString();
        const auto& roadDistances = node.AsDict().at("road_distances").AsDict();
        for (const auto& [key, value] : roadDistances) {
            catalogue.AddToStation(name, key, value.AsInt());
        }
    }

}

void LoadJson::LoadBuses(const json::Node& baseRequests) {

    for (const auto& node : baseRequests.AsArray()) {
        if (!node.IsDict() || !node.AsDict().count("type") || node.AsDict().at("type").AsString() != "Bus") {
            continue;
        }

        std::string_view name = node.AsDict().at("name").AsString();
        bool is_roundtrip = node.AsDict().at("is_roundtrip").AsBool();

        std::vector<std::string_view> route;
        const auto& stops = node.AsDict().at("stops").AsArray();
        for (const auto& stopNode : stops) {
            route.push_back(stopNode.AsString());
        }
        catalogue.AddRouteBus(name, route, is_roundtrip);
    }

}


void LoadJson::GetReply() {
    const auto& root = jsonDoc.GetRoot();
    if (!root.IsDict() || !root.AsDict().count("stat_requests")) {
        throw std::logic_error("Invalid JSON structure");
    }
    const auto& statRequests = root.AsDict().at("stat_requests");
    LoadRequest(statRequests);
}

void LoadJson::LoadRequest(const json::Node& statRequests) {

    json::Builder builder;
    builder.StartArray();

    for (const auto& node : statRequests.AsArray()) {
        if (!node.IsDict() || !node.AsDict().count("id") || !node.AsDict().count("type")) {
            throw std::logic_error("Invalid request format");
        }

        const auto& dictNode = node.AsDict();
        builder.StartDict();
        builder.Key("request_id").Value(dictNode.at("id").AsInt());

        std::string_view requestType = dictNode.at("type").AsString();

        if(requestType == "Map") {
            builder.Key("map").Value(PrintRender());
        } else if (requestType == "Stop") {
            std::string_view name = dictNode.at("name").AsString();
            auto stopStat = request.GetBusesByStop(name);

            if (stopStat.has_value()) {
                builder.Key("buses").StartArray();
                for (const auto& bus : *stopStat) {
                    builder.Value(bus);
                }
                builder.EndArray();

            } else {
                builder.Key("error_message").Value("not found");
            }

        } else if (requestType == "Bus") {
            std::string_view name = dictNode.at("name").AsString();
            auto busStat = request.GetBusStat(name);
            if(busStat.has_value()) {
                builder.Key("curvature").Value(busStat->curvature);
                builder.Key("route_length").Value(busStat->distance);
                builder.Key("stop_count").Value(busStat->stops_on_route);
                builder.Key("unique_stop_count").Value(busStat->unique_stations);
            } else {
                builder.Key("error_message").Value("not found");
            }
        }
        builder.EndDict();
    }
    builder.EndArray();

    json::Print(json::Document(builder.Build()), std::cout);
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
    if (!root.IsDict() || !root.AsDict().count("render_settings")) {
        throw std::logic_error("Invalid JSON structure");
    }

    const auto& render_settings = root.AsDict().at("render_settings");

    render_set.width = render_settings.AsDict().at("width").AsDouble();
    render_set.height = render_settings.AsDict().at("height").AsDouble();
    render_set.padding = render_settings.AsDict().at("padding").AsDouble();

    render_set.line_width = render_settings.AsDict().at("line_width").AsDouble();
    render_set.stop_radius = render_settings.AsDict().at("stop_radius").AsDouble();

    render_set.bus_label_font_size = render_settings.AsDict().at("bus_label_font_size").AsDouble();

    std::vector<double> bus_label_offset;
    for (const auto& value : render_settings.AsDict().at("bus_label_offset").AsArray()) {
        bus_label_offset.push_back(value.AsDouble());
    }

    render_set.bus_label_offset = svg::Point(bus_label_offset[0],bus_label_offset[1]);

    render_set.stop_label_font_size = render_settings.AsDict().at("stop_label_font_size").AsDouble();
    std::vector<double> stop_label_offset;
    for (const auto& value : render_settings.AsDict().at("stop_label_offset").AsArray()) {
        stop_label_offset.push_back(value.AsDouble());
    }

    render_set.stop_label_offset = svg::Point(stop_label_offset[0], stop_label_offset[1]);

    const auto& underlayer_color_node = render_settings.AsDict().at("underlayer_color");
    render_set.underlayer_color = ParseColor(underlayer_color_node);

    render_set.underlayer_width = render_settings.AsDict().at("underlayer_width").AsDouble();

    const auto& color_array = render_settings.AsDict().at("color_palette").AsArray();
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
