/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области
 * (domain) вашего приложения и не зависят от транспортного справочника. Например Автобусные
 * маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */

#include <string_view>
#include <vector>
#include <string>
#include <set>
#include "geo.h"

namespace domain {
     
    struct Stop {
    public:
        std::string name;
        geo::Coordinates coords {0L, 0L};
    };
    
    struct Bus {
        std::string bus_number;
        std::vector<const domain::Stop*> stops;
        bool is_roundtrip;
    };
    
    struct Bus_ {
        std::string_view bus_number_;
        int unique_stops_qty = 0U;
        int stops_num = 0U;
        double geo_route_length = 0L;
        int meters_route_length = 0U;
        double curvature = 0L;
    };
    
    struct Stop_{
        std::string_view stops_name_{};
        std::vector<std::string> bus_number_;
        bool passing;
    };

}