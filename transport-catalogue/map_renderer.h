#pragma once

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 */

#include "svg.h"
#include "json_reader.h"
#include "geo.h"
//#include "domain.h"


#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>
#include <tuple>



namespace sphere{
	inline const double EPSILON = 1e-6;

	bool IsZero(double value);

	class SphereProjector{
	public:

		SphereProjector() = default;
		// points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
		template <typename PointInputIt>
		SphereProjector(PointInputIt points_begin, PointInputIt points_end,
			double max_width, double max_height, double padding)
			: padding_(padding)	{
			// Если точки поверхности сферы не заданы, вычислять нечего
			if (points_begin == points_end) {
				return;
			}
			// Находим точки с минимальной и максимальной долготой
			const auto [left_it, right_it]
				= std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs)	{
						return lhs.lng < rhs.lng;
					});
			min_lon_ = left_it->lng;
			const double max_lon = right_it->lng;

			// Находим точки с минимальной и максимальной широтой
			const auto [bottom_it, top_it] = std::minmax_element(
				points_begin, points_end,
				[](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
			const double min_lat = bottom_it->lat;
			max_lat_ = top_it->lat;

			// Вычисляем коэффициент масштабирования вдоль координаты x
			std::optional<double> width_zoom;
			if (!IsZero(max_lon - min_lon_)) {
				width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
			}

			// Вычисляем коэффициент масштабирования вдоль координаты y
			std::optional<double> height_zoom;
			if (!IsZero(max_lat_ - min_lat)) {
				height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
			}

			if (width_zoom && height_zoom) {
				// Коэффициенты масштабирования по ширине и высоте ненулевые,
				// берём минимальный из них
				zoom_coeff_ = std::min(*width_zoom, *height_zoom);
			}
			else if (width_zoom) {
				// Коэффициент масштабирования по ширине ненулевой, используем его
				zoom_coeff_ = *width_zoom;
			}
			else if (height_zoom) {
				// Коэффициент масштабирования по высоте ненулевой, используем его
				zoom_coeff_ = *height_zoom;
			}
		}

		// Проецирует широту и долготу в координаты внутри SVG-изображения
		svg::Point operator()(geo::Coordinates coords) const;

	private:
		double padding_;
		double min_lon_ = 0;
		double max_lat_ = 0;
		double zoom_coeff_ = 0;
	};
	
}//namespace

namespace renderer{
	struct SVGBusInfo{
		std::vector<svg::Polyline> svg_bus_route;
		std::vector<svg::Text> svg_name_buses;
		std::map<std::string, svg::Circle> svg_circle_stops;
		std::map<std::string, std::pair<svg::Text, svg::Text>> svg_name_stops;
	};

	struct SVGTextStopInfo{
		std::string name_stop;
		svg::Text svg_name_stop;
		svg::Text svg_name_stop_substr;
	};

	struct SVGCircleStop{
		std::string name_stop;
		svg::Circle svg_stop;
	};

	struct BusSVG{
		svg::Polyline shape_route_bus;
		std::vector<svg::Text> svg_name_bus;
		std::vector<SVGCircleStop> svg_circle_stops;
		std::vector<SVGTextStopInfo> svg_name_stops;
	};
    
    struct MapRenderSettings{
        double width = 0.;
		double height = 0.;
		double padding = 0.;
		double line_width = 0.;
		double stop_radius = 0.;

		double bus_label_font_size = 0.;
		geo::Coordinates bus_label_offset{};

		double stop_label_font_size = 0.;
		geo::Coordinates stop_label_offset{};

		svg::Color underlayer_color{};
		double underlayer_width = 0.;

		std::vector<svg::Color> color_palette{};
    };

	class MapRenderer{
	public:
		explicit MapRenderer(const json::Dict& render_settings, transport_catalogue::TransportCatalogue& transport_catalogue);

		svg::Color AddColor(const json::Node& node);

		svg::Polyline AddRoute(const domain::Bus& bus, const svg::Color& color);

		void FillBusTrain(const domain::Bus& bus, const svg::Color& color);

		void SetSphereProjector(transport_catalogue::TransportCatalogue& transport_catalogue);

		void AddBusSVG(transport_catalogue::TransportCatalogue& transport_catalogue);

		std::string PrintingMap()const;

		svg::Text TextSvgForBus(const svg::Point& pos, const std::string& data);

		svg::Text CreateSVGTextForBus(const svg::Point& pos, const svg::Color& color, const std::string& data);

		svg::Text CreateSVGTextForBus(const svg::Point& pos, const std::string& data);

		std::vector<svg::Text> AddNameBus(const domain::Bus& bus, const svg::Color& color);

		svg::Text TextSvgForStop(const svg::Point& pos, const std::string& data);

		svg::Text CreateSVGTextForStop(const svg::Point& pos, const svg::Color& color, const std::string& data);

		svg::Text CreateSVGTextForStop(const svg::Point& pos, const std::string& data);

		std::vector<SVGTextStopInfo> AddNameStops(const domain::Bus& bus);

		std::vector<SVGCircleStop> AddCircleStops(const domain::Bus& bus);

		SVGBusInfo FillInfo(const std::vector<BusSVG>& buses)const;

	private:
        MapRenderSettings MRSettings_;

		sphere::SphereProjector sphere_;
		std::vector<BusSVG> bus_train;
		
	};

}//namespace