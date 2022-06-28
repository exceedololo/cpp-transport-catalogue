#include "map_renderer.h"

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
*/


namespace sphere{
	bool IsZero(double value){
		return std::abs(value) < EPSILON;
	}
	svg::Point SphereProjector::operator()(geo::Coordinates coords) const{
		return { (coords.lng - min_lon_) * zoom_coeff_ + padding_,
				(max_lat_ - coords.lat) * zoom_coeff_ + padding_ };
	}
}//namespace sphere



namespace renderer{
	MapRenderer::MapRenderer(const json::Dict& render_settings,  transport_catalogue::TransportCatalogue& transport_catalogue)	{
		MRSettings_.width = render_settings.at("width").AsDouble();
		MRSettings_.height = render_settings.at("height").AsDouble();
		MRSettings_.padding = render_settings.at("padding").AsDouble();
		MRSettings_.line_width = render_settings.at("line_width").AsDouble();
		MRSettings_.stop_radius = render_settings.at("stop_radius").AsDouble();
		MRSettings_.bus_label_font_size = render_settings.at("bus_label_font_size").AsDouble();
		MRSettings_.bus_label_offset.lat = render_settings.at("bus_label_offset").AsArray().front().AsDouble();
		MRSettings_.bus_label_offset.lng = render_settings.at("bus_label_offset").AsArray().back().AsDouble();
		MRSettings_.stop_label_font_size = render_settings.at("stop_label_font_size").AsDouble();
		MRSettings_.stop_label_offset.lat = render_settings.at("stop_label_offset").AsArray().front().AsDouble();
		MRSettings_.stop_label_offset.lng = render_settings.at("stop_label_offset").AsArray().back().AsDouble();
		MRSettings_.underlayer_width = render_settings.at("underlayer_width").AsDouble();
		MRSettings_.underlayer_color = AddColor(render_settings.at("underlayer_color"));
		json::Array palette = render_settings.at("color_palette").AsArray();
		for (auto it = palette.begin(); it != palette.end(); ++it){
			MRSettings_.color_palette.push_back(AddColor(*it));
		}
		AddBusSVG(transport_catalogue);
	}

	svg::Color MapRenderer::AddColor(const json::Node& node){
		svg::Color color;
        if (node.IsArray()){
			if (node.AsArray().size() == 3)	{
				svg::Rgb rgb;
				rgb.red = node.AsArray()[0].AsInt();
				rgb.green = node.AsArray()[1].AsInt();
				rgb.blue = node.AsArray()[2].AsInt();
				color = rgb;
			}
			if (node.AsArray().size() == 4)	{
				svg::Rgba rgba;
				rgba.red = node.AsArray()[0].AsInt();
				rgba.green = node.AsArray()[1].AsInt();
				rgba.blue = node.AsArray()[2].AsInt();
				rgba.opacity = node.AsArray()[3].AsDouble();
				color = rgba;
			}
		}
		if (node.IsString()){
			color = node.AsString();
		}
		return color;	
	}

	svg::Polyline MapRenderer::AddRoute (const transport_catalogue::Bus& bus, const svg::Color& color){
		svg::Polyline route_bus;
		route_bus.SetStrokeColor(color);
		route_bus.SetFillColor("none");
		route_bus.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
		route_bus.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
		route_bus.SetStrokeWidth(MRSettings_.line_width);
		for (auto stop : bus.stops)	{
			svg::Point point = sphere_({ stop->coords.lat, stop->coords.lng });
			route_bus.AddPoint(point);
		}
		return route_bus;
	}
    
	svg::Text MapRenderer::TextSvgForBus(const svg::Point& pos, const std::string& data){
		return svg::Text().SetPosition(pos)
			.SetOffset({ MRSettings_.bus_label_offset.lat, MRSettings_.bus_label_offset.lng })
			.SetFontSize(MRSettings_.bus_label_font_size)
			.SetFontFamily("Verdana")
			.SetFontWeight("bold")
			.SetData(data);
	}

	svg::Text MapRenderer::CreateSVGTextForBus(const svg::Point& pos, const svg::Color& color, const std::string& data)	{
		return TextSvgForBus(pos, data).SetFillColor(color);
	}

	svg::Text MapRenderer::CreateSVGTextForBus(const svg::Point& pos, const std::string& data){
		return TextSvgForBus(pos, data)
			.SetFillColor(MRSettings_.underlayer_color)
			.SetStrokeColor(MRSettings_.underlayer_color)
			.SetStrokeWidth(MRSettings_.underlayer_width)
			.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
			.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
	}

	std::vector<svg::Text> MapRenderer::AddNameBus(const transport_catalogue::Bus& bus, const svg::Color& color){
		std::vector<svg::Text>result;
		result.push_back(CreateSVGTextForBus(sphere_({ bus.stops.front()->coords.lat, bus.stops.front()->coords.lng }), bus.bus_number));
		result.push_back(CreateSVGTextForBus(sphere_({ bus.stops.front()->coords.lat, bus.stops.front()->coords.lng }), color, bus.bus_number));
		if (!bus.is_roundtrip && bus.stops[(bus.stops.size() + 1) / 2 - 1] != bus.stops[0]){
			result.push_back(CreateSVGTextForBus(sphere_({ bus.stops[(bus.stops.size() + 1) / 2 - 1]->coords.lat
				, bus.stops[(bus.stops.size() + 1) / 2 - 1]->coords.lng }), bus.bus_number));
			result.push_back(CreateSVGTextForBus(sphere_({ bus.stops[(bus.stops.size() + 1) / 2 - 1]->coords.lat
				, bus.stops[(bus.stops.size() + 1) / 2 - 1]->coords.lng }), color, bus.bus_number));
		}
		return result;
	}

	svg::Text MapRenderer::TextSvgForStop(const svg::Point& pos, const std::string& data){
		return svg::Text().SetPosition(pos)
			.SetOffset({ MRSettings_.stop_label_offset.lat, MRSettings_.stop_label_offset.lng })
			.SetFontSize(MRSettings_.stop_label_font_size)
			.SetFontFamily("Verdana")
			.SetData(data);
	}

	svg::Text MapRenderer::CreateSVGTextForStop(const svg::Point& pos, const svg::Color& color, const std::string& data){
		return TextSvgForStop(pos, data).SetFillColor(color);
	}

	svg::Text MapRenderer::CreateSVGTextForStop(const svg::Point& pos, const std::string& data)	{
		return TextSvgForStop(pos, data)
			.SetFillColor(MRSettings_.underlayer_color)
			.SetStrokeColor(MRSettings_.underlayer_color)
			.SetStrokeWidth(MRSettings_.underlayer_width)
			.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
			.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
	}

	std::vector<SVGTextStopInfo> MapRenderer::AddNameStops(const transport_catalogue::Bus& bus){
		std::vector<SVGTextStopInfo> result;
		for (size_t i = 0; i < bus.stops.size() - 1; ++i){
			result.push_back({ bus.stops[i]->name
				, CreateSVGTextForStop(sphere_({ bus.stops[i]->coords.lat, bus.stops[i]->coords.lng }), "black", bus.stops[i]->name)
				, CreateSVGTextForStop(sphere_({ bus.stops[i]->coords.lat, bus.stops[i]->coords.lng }),bus.stops[i]->name) });
		}
		return result;
	}

	std::vector<SVGCircleStop> MapRenderer::AddCircleStops(const transport_catalogue::Bus& bus){
		std::vector<SVGCircleStop> result;
		for (size_t i = 0; i < bus.stops.size() - 1; ++i){
			svg::Circle circle;
			circle.SetCenter(sphere_({ bus.stops[i]->coords.lat, bus.stops[i]->coords.lng }));
			circle.SetRadius(MRSettings_.stop_radius);
			circle.SetFillColor("white");
			result.push_back({ bus.stops[i]->name ,circle });
		}
		return result;
	}

	void MapRenderer::AddBusSVG(transport_catalogue::TransportCatalogue& transport_catalogue){
		SetSphereProjector(transport_catalogue);
		std::vector<transport_catalogue::Bus>buses;
		for (const auto& bus : transport_catalogue.GetAllBusesData()){
			buses.push_back(bus);
		}
		sort(buses.begin(), buses.end(), [](const transport_catalogue::Bus& lhs, const transport_catalogue::Bus& rhs){
				return lhs.bus_number < rhs.bus_number;
		});
		int j = 0;
		for (size_t i = 0; i < buses.size(); ++i){
			if (buses[i].stops.size()){
				FillBusTrain(buses[i], MRSettings_.color_palette[j]);
				if (j == static_cast<int>(MRSettings_.color_palette.size()) - 1){
					j = -1;
				}
				++j;
			}
		}
	}

	void MapRenderer::FillBusTrain(const transport_catalogue::Bus& bus, const svg::Color& color){
		bus_train.push_back({ AddRoute(bus,color) ,AddNameBus(bus, color) ,AddCircleStops(bus) ,AddNameStops(bus) });
	}

	void MapRenderer::SetSphereProjector(transport_catalogue::TransportCatalogue& transport_catalogue){
		std::vector<geo::Coordinates> min_max;
		for (const auto& bus : transport_catalogue.GetAllBusesData()){
			for (const auto& stop : bus.stops){
				if (std::find(min_max.begin(), min_max.end(), stop->coords) == min_max.end()){
					min_max.push_back(stop->coords);
				}
			}
			sphere_ = sphere::SphereProjector(min_max.begin(), min_max.end(), MRSettings_.width, MRSettings_.height, MRSettings_.padding);
		}
	}

	inline SVGBusInfo MapRenderer::FillInfo (const std::vector<BusSVG>& buses) const {
		SVGBusInfo temp;
		for (const auto& bus : buses){
			if (bus.svg_name_stops.size()){
				temp.svg_bus_route.push_back(bus.shape_route_bus);
				for (const auto& n_bus : bus.svg_name_bus){
					temp.svg_name_buses.push_back(n_bus);
				}
				for (const auto& circle_stop : bus.svg_circle_stops){
					temp.svg_circle_stops.insert({ circle_stop.name_stop, circle_stop.svg_stop });
				}
				for (const auto& name_stop : bus.svg_name_stops){
					temp.svg_name_stops.insert({ name_stop.name_stop ,{name_stop.svg_name_stop, name_stop.svg_name_stop_substr } });
				}
			}
		}
		return temp;
	}

	std::string MapRenderer::PrintingMap()const{

		svg::Document doc;
		for (const auto& line : FillInfo(bus_train).svg_bus_route){
			doc.Add(line);
		}
		for (const auto& text : FillInfo(bus_train).svg_name_buses){
			doc.Add(text);
		}
		for (const auto& circle : FillInfo(bus_train).svg_circle_stops){
			doc.Add(circle.second);
		}
		for (const auto& stop : FillInfo(bus_train).svg_name_stops){
			doc.Add(stop.second.second);
			doc.Add(stop.second.first);
		}
		
		std::stringstream map;
		doc.Render(map);
		return map.str();
		}

}//namespace renderer

