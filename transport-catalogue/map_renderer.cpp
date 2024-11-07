#include "map_renderer.h"
#include "request_handler.h"


void MapRender::SetRender(LoadRender &set) {
    render_set = set;
}

void MapRender::GetRenderRoute(svg::Document& route_render, RequestHandler& request) {
    size_t count = 0;
    auto allbus = request.GetAutopark();
    std::vector<geo::Coordinates> all_coords;


    for (const auto& one_bus : allbus) {
        auto route = one_bus->route;
        for(auto station : route) {
            all_coords.push_back(station->coord_station_);
        }
    }

    SphereProjector coord_bus_sphere(all_coords.begin(), all_coords.end(),
                                     render_set.width, render_set.height, render_set.padding);
    for(auto& one_bus : allbus) {
        if(one_bus->route.empty()) {
            continue;
        }
        std::vector<StationRender> route_station;
        svg::Polyline bus;
        for(auto station : one_bus->route) {
            bus.AddPoint(coord_bus_sphere(station->coord_station_));

            StationRender stop (station->name_station_, coord_bus_sphere(station->coord_station_));
            route_station.push_back(stop);
        }
        if(count >= render_set.color_palette.size()) {
            count = 0;
        }

        render_map.emplace(BusRender(one_bus->number_bus_,route_station, count, one_bus->is_roundtrip));

        bus.SetFillColor(svg::NoneColor).SetStrokeColor(render_set.color_palette[count++]).
            SetStrokeWidth(render_set.line_width).SetStrokeLineCap(svg::StrokeLineCap::ROUND).
            SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        route_render.Add(std::move(bus));
    }
}


void MapRender::GetRenderNameRoute(svg::Document& route_render) {

    for(const auto& bus : render_map) {
        if(bus.route_.empty()) {
            continue;
        }
        svg::Text fill;
        svg::Text legend;
        if(bus.roundtrip || bus.route_[0].name_station == bus.route_[bus.route_.size() / 2].name_station) {
            auto end_station = bus.route_[0];
            fill.SetFillColor(render_set.underlayer_color).
                SetStrokeColor(render_set.underlayer_color).
                SetStrokeWidth(render_set.underlayer_width).
                SetStrokeLineCap(svg::StrokeLineCap::ROUND).
                SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).
                SetPosition(end_station.coord).
                SetOffset(render_set.bus_label_offset).
                SetFontSize(render_set.bus_label_font_size).
                SetFontFamily("Verdana").
                SetFontWeight("bold").
                SetData(static_cast<std::string>(bus.bus_number));

            legend.SetFillColor(render_set.color_palette[bus.color]).
                SetPosition(end_station.coord).
                SetOffset(render_set.bus_label_offset).
                SetFontSize(render_set.bus_label_font_size).
                SetFontFamily("Verdana").
                SetFontWeight("bold").
                SetData(static_cast<std::string>(bus.bus_number));

            route_render.Add(std::move(fill));
            route_render.Add(std::move(legend));

        } else {
            auto end_station = bus.route_[0];
            auto end_station_2 = bus.route_[bus.route_.size() / 2];

            fill.SetFillColor(render_set.underlayer_color).
                SetStrokeColor(render_set.underlayer_color).
                SetStrokeWidth(render_set.underlayer_width).
                SetStrokeLineCap(svg::StrokeLineCap::ROUND).
                SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).
                SetPosition(end_station.coord).
                SetOffset(render_set.bus_label_offset).
                SetFontSize(render_set.bus_label_font_size).
                SetFontFamily("Verdana").
                SetFontWeight("bold").
                SetData(static_cast<std::string>(bus.bus_number));

            legend.SetFillColor(render_set.color_palette[bus.color]).
                SetPosition(end_station.coord).
                SetOffset(render_set.bus_label_offset).
                SetFontSize(render_set.bus_label_font_size).
                SetFontFamily("Verdana").
                SetFontWeight("bold").
                SetData(static_cast<std::string>(bus.bus_number));

            route_render.Add(std::move(fill));
            route_render.Add(std::move(legend));

            svg::Text fill;
            svg::Text legend;

            fill.SetFillColor(render_set.underlayer_color).
                SetStrokeColor(render_set.underlayer_color).
                SetStrokeWidth(render_set.underlayer_width).
                SetStrokeLineCap(svg::StrokeLineCap::ROUND).
                SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).
                SetPosition(end_station_2.coord).
                SetOffset(render_set.bus_label_offset).
                SetFontSize(render_set.bus_label_font_size).
                SetFontFamily("Verdana").
                SetFontWeight("bold").
                SetData(static_cast<std::string>(bus.bus_number));

            legend.SetFillColor(render_set.color_palette[bus.color]).
                SetPosition(end_station_2.coord).
                SetOffset(render_set.bus_label_offset).
                SetFontSize(render_set.bus_label_font_size).
                SetFontFamily("Verdana").
                SetFontWeight("bold").
                SetData(static_cast<std::string>(bus.bus_number));


            route_render.Add(std::move(fill));
            route_render.Add(std::move(legend));
        }

    }
}

void MapRender::GetRenderSimStop(svg::Document& route_render) {
    auto station_comparator = [](const StationRender& a, const StationRender& b) {
        return a.name_station < b.name_station;
    };

    std::set<StationRender, decltype(station_comparator)> station_set(station_comparator);

    for (const auto& bus : render_map) {
        if (bus.route_.empty()) {
            continue;
        }

        for (const auto& station : bus.route_) {
            station_set.insert(station);
        }
    }

    for (const auto& station_circle : station_set) {

        svg::Circle stop;
        stop.SetCenter(station_circle.coord)
            .SetRadius(render_set.stop_radius)
            .SetFillColor("white");

        route_render.Add(std::move(stop)); // Добавляем круг в документ
    }

    for(const auto& station : station_set) {
        svg::Text name_st;
        svg::Text fill;
        fill.SetFillColor(render_set.underlayer_color).
            SetStrokeColor(render_set.underlayer_color).
            SetStrokeWidth(render_set.underlayer_width).
            SetStrokeLineCap(svg::StrokeLineCap::ROUND).
            SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).
            SetPosition(station.coord).
            SetOffset(render_set.stop_label_offset).
            SetFontSize(render_set.stop_label_font_size).
            SetFontFamily("Verdana").
            SetData(static_cast<std::string>(station.name_station));
        name_st.SetFillColor("black").
            SetPosition(station.coord).
            SetOffset(render_set.stop_label_offset).
            SetFontSize(render_set.stop_label_font_size).
            SetFontFamily("Verdana").
            SetData(static_cast<std::string>(station.name_station));
        route_render.Add(std::move(fill));
        route_render.Add(std::move(name_st));
    }
}
