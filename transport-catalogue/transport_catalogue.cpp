#include "transport_catalogue.h"

namespace transport_catalogue {
    
    std::size_t PairPointersHasher::operator()(const std::pair<const domain::Stop*, const domain::Stop*> pair_of_pointers) const noexcept {
            auto ptr1 = static_cast<const void*>(pair_of_pointers.first);
            auto ptr2 = static_cast<const void*>(pair_of_pointers.second);
            return hasher_(ptr1) * 37 + hasher_(ptr2);
    }

    TransportCatalogue::TransportCatalogue() {}

    TransportCatalogue::~TransportCatalogue() {}

    void TransportCatalogue::SetStop(const domain::Stop& stop) {
        if (all_stops_map_.count(GetStopName(&stop)) == 0) {
            auto& ref = all_stops_data_.emplace_back(stop);
            all_stops_map_.insert({ std::string_view(ref.name), &ref });
        }
    }

    void TransportCatalogue::SetRoute(const domain::Bus& route) {
        if (all_buses_map_.count(route.bus_number) == 0) {
            auto& ref = all_buses_data_.emplace_back(route);
            all_buses_map_.insert({ std::string_view(ref.bus_number), &ref });
            for (size_t i = 0; i < ref.stops.size(); ++i) {
                buses_in_stop_[ref.stops[i]].insert(&ref);
            }
        }
    }

    void TransportCatalogue::AddStopToBusMap(const std::string_view route) {
        auto bus = GetRouteByName(route);
        for (size_t i = 0; i < bus->stops.size(); ++i) {
            buses_in_stop_[bus->stops[i]].insert(bus);
        }
    }

    void TransportCatalogue::SetDistance(const domain::Stop* stop_from, const domain::Stop* stop_to, size_t dist) {
        if (stop_from != nullptr && stop_to != nullptr) {
            distances_map_.insert({ { stop_from, stop_to }, dist });
        }
    }

    size_t TransportCatalogue::GetDistance(const domain::Stop* stop_from, const domain::Stop* stop_to) {
        size_t result = GetDistanceDirectly(stop_from, stop_to);
        return (result > 0 ? result : GetDistanceDirectly(stop_to, stop_from));
    }

    size_t TransportCatalogue::GetDistanceDirectly(const domain::Stop* stop_from, const domain::Stop* stop_to) {
        if (distances_map_.count({ stop_from, stop_to }) > 0) {
            return distances_map_.at({ stop_from, stop_to });
        }
        return 0U;
    }

    std::string_view TransportCatalogue::GetStopName(const domain::Stop* stop_ptr) {
        return std::string_view(stop_ptr->name);
    }

    std::string_view TransportCatalogue::GetStopName(const domain::Stop stop) {
        return std::string_view(stop.name);
    }

    std::string_view TransportCatalogue::GetBusName(const domain::Bus* route_ptr) {
        return std::string_view(route_ptr->bus_number);
    }

    std::string_view TransportCatalogue::GetBusName(const domain::Bus route) {
        return std::string_view(route.bus_number);
    }
    const domain::Stop* TransportCatalogue::GetStopByName(std::string_view stop_name) {
        if (all_stops_map_.count(stop_name) == 0) {

            return nullptr;
        }
        return all_stops_map_.at(stop_name);
    }

    domain::Bus* TransportCatalogue::GetRouteByName(std::string_view bus_name) {
        if (all_buses_map_.count(bus_name) == 0) {
            return nullptr;
        }
        else {
            return all_buses_map_.at(bus_name);
        }
    }

    domain::Bus_ TransportCatalogue::GetBusByName(std::string_view bus_name) {
        if (all_buses_map_.count(bus_name)){
            auto& Bus = all_buses_map_[bus_name];
            std::vector<const domain::Stop*> tmp = Bus->stops;
            int stops_num = static_cast<int>(Bus->stops.size());
            int uniq = 0U;
            std::sort(tmp.begin(), tmp.end());
            auto last = std::unique(tmp.begin(), tmp.end());
            uniq = (last != tmp.end() ? std::distance(tmp.begin(), last) : tmp.size());
            double geo_length = 0L;
            int meters_length = 0U;
            double curv = 0L;
            if (stops_num > 1){
                for (int i = 0; i < stops_num - 1; ++i){
                    geo_length += ComputeDistance(Bus->stops[i]->coords, Bus->stops[i + 1]->coords);
                    meters_length += GetDistance(Bus->stops[i], Bus->stops[i + 1]);
                }
                curv = meters_length / geo_length;
            }
            return { bus_name, uniq, stops_num, geo_length, meters_length, curv };
        }
        else {
            return {bus_name, 0, 0, 0.0, 0, 0.0};
        }
    }


    domain::Stop_ TransportCatalogue::GetBusContainerForStop(std::string_view stop_name) {
        auto* result = GetStopByName(stop_name);
        domain::Stop_ stoppp;
        stoppp.stops_name_ = stop_name;
        if (result != nullptr) {
                if (buses_in_stop_.count(result)) {
                auto set_buses = buses_in_stop_.at(result);

                for (const auto& bus : set_buses)
                {
                    stoppp.stops_name_ = stop_name;
                    stoppp.bus_number_.push_back(bus->bus_number);
                }
                std::sort(stoppp.bus_number_.begin(), stoppp.bus_number_.end());
                return stoppp;
            }
            return stoppp;
        }
        stoppp.passing = true;
        return stoppp;
    }
}