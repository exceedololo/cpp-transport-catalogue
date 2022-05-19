#include "transport_catalogue.h"

namespace transport_catalogue {

    TransportCatalogue::TransportCatalogue() {}

    TransportCatalogue::~TransportCatalogue() {}

    void TransportCatalogue::SetStop(const Stop& stop) {
        if (all_stops_map_.count(GetStopName(&stop)) == 0) {
            auto& ref = all_stops_data_.emplace_back(stop);
            all_stops_map_.insert({ std::string_view(ref.name), &ref });
        }
    }

    void TransportCatalogue::SetRoute(const Bus& route) {
        if (all_buses_map_.count(route.bus_number) == 0) {
            auto& ref = all_buses_data_.emplace_back(route);
            all_buses_map_.insert({ std::string_view(ref.bus_number), &ref });
            for (size_t i = 0; i < ref.stops.size(); ++i) {
                buses_in_stop_[ref.stops[i]].insert(&ref);
            }

            /*std::vector<const Stop*> tmp = ref.stops;
            std::sort(tmp.begin(), tmp.end());
            auto last = std::unique(tmp.begin(), tmp.end());
            ref.unique_stops_qty = (last != tmp.end() ? std::distance(tmp.begin(), last) : tmp.size());

            int stops_num = static_cast<int>(ref.stops.size());
            if (stops_num > 1) {
                ref.geo_route_length = 0L;
                ref.meters_route_length = 0U;
                for (int i = 0; i < stops_num - 1; ++i) {
                    ref.geo_route_length += ComputeDistance(ref.stops[i]->coords, ref.stops[i + 1]->coords);
                    ref.meters_route_length += GetDistance(ref.stops[i], ref.stops[i + 1]);
                }
                ref.curvature = ref.meters_route_length / ref.geo_route_length;
            }
            else {
                ref.geo_route_length = 0L;
                ref.meters_route_length = 0U;
                ref.curvature = 1L;
            }*/
        }
    }
    std::unordered_set<Bus*> TransportCatalogue::GetVectorOfBusForStop(const Stop* stop) {
        if (buses_in_stop_.find(stop) != buses_in_stop_.end()) {
            return buses_in_stop_.at(stop);
        }
        else
            return {};
    }

    void TransportCatalogue::SetDistance(const Stop* stop_from, const Stop* stop_to, size_t dist) {
        if (stop_from != nullptr && stop_to != nullptr) {
            distances_map_.insert({ { stop_from, stop_to }, dist });
        }
    }

    size_t TransportCatalogue::GetDistance(const Stop* stop_from, const Stop* stop_to) {
        size_t result = GetDistanceDirectly(stop_from, stop_to);
        return (result > 0 ? result : GetDistanceDirectly(stop_to, stop_from));
    }

    size_t TransportCatalogue::GetDistanceDirectly(const Stop* stop_from, const Stop* stop_to) {
        if (distances_map_.count({ stop_from, stop_to }) > 0) {
            return distances_map_.at({ stop_from, stop_to });
        }
        return 0U;
    }

    std::string_view TransportCatalogue::GetStopName(const Stop* stop_ptr) {
        return std::string_view(stop_ptr->name);
    }

    std::string_view TransportCatalogue::GetStopName(const Stop stop) {
        return std::string_view(stop.name);
    }

    std::string_view TransportCatalogue::GetBusName(const Bus* route_ptr) {
        return std::string_view(route_ptr->bus_number);
    }

    std::string_view TransportCatalogue::GetBusName(const Bus route) {
        return std::string_view(route.bus_number);
    }
    const Stop* TransportCatalogue::GetStopByName(std::string_view stop_name) {
        if (all_stops_map_.count(stop_name) == 0) {

            return nullptr;
        }
        return all_stops_map_.at(stop_name);
    }

    Bus* TransportCatalogue::GetRouteByName(std::string_view bus_name) {
        if (all_buses_map_.count(bus_name) == 0) {
            return nullptr;
        }
        else {
            return all_buses_map_.at(bus_name);
        }
    }

    Bus_ TransportCatalogue::GetBus_ByName(std::string_view bus_name) {
        if (all_buses_map_.count(bus_name))  {
            auto& Bus = all_buses_map_[bus_name];
            std::vector<const Stop*> tmp = Bus->stops;
            size_t stops_num = static_cast<int>(Bus->stops.size());
            size_t uniq = 0U;
            std::sort(tmp.begin(), tmp.end());
            auto last = std::unique(tmp.begin(), tmp.end());
            uniq = (last != tmp.end() ? std::distance(tmp.begin(), last) : tmp.size());
            double geo_length = 0L;
            size_t meters_length = 0U;
            double curv = 0L;
            if (stops_num > 1) {
                for (int i = 0; i < stops_num - 1; ++i) {
                    geo_length += ComputeDistance(Bus->stops[i]->coords, Bus->stops[i + 1]->coords);
                    meters_length += GetDistance(Bus->stops[i], Bus->stops[i + 1]);
                }
                curv = meters_length / geo_length;
            }
            return { bus_name, uniq, geo_length, meters_length, curv };
        }
        else {
            return { bus_name, 0, 0.0, 0, 0.0 };
        }
    }

    RequestResult TransportCatalogue::GetRouteInfo(std::string_view bus_name) {
        RequestResult result;
        result.r_ptr = GetBus_ByName(bus_name);
        if (&(result.r_ptr) != nullptr) {
            result.code = RequestResultType::Ok;
        }
        else {
            result.code = RequestResultType::RouteNotExists;
        }
        return result;
    }


    RequestResult TransportCatalogue::GetBusesForStop(std::string_view stop_name) {
        RequestResult result;
        result.s_ptr = GetStopByName(stop_name);

        if (result.s_ptr != nullptr) {
            std::vector<std::string_view> found_buses_sv;
            for (auto& bus : all_buses_map_) {
                auto tmp = std::find_if(bus.second->stops.begin(), bus.second->stops.end(),
                                        [stop_name](const Stop* curr_stop) {
                                            return (curr_stop->name == stop_name);
                                        });
                if (tmp != bus.second->stops.end()) {
                    found_buses_sv.push_back(bus.second->bus_number);
                }
            }
            if (found_buses_sv.size() > 0) {
                result.code = RequestResultType::Ok;
                std::sort(found_buses_sv.begin(), found_buses_sv.end());
                for (auto& element : found_buses_sv) {
                    result.vector_str.emplace_back(std::string(element));
                }
            }
            else {
                result.code = RequestResultType::NoBuses;
            }
        }
        else {
            result.code = RequestResultType::StopNotExists;
        }
        return result;
    }
}
