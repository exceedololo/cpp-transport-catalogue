#pragma once
#include "geo.h"
#include <deque>
#include <string>
#include <string_view>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <cctype>
#include <functional>

namespace transport_catalogue {

    struct Stop {
    public:
        std::string name;
        geo::Coordinates coords {0L, 0L};
    };

    struct Bus {
        std::string bus_number;
        std::vector<const Stop*> stops;
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


    class PairPointersHasher {
    public:
        std::size_t operator()(const std::pair<const Stop*, const Stop*> pair_of_pointers) const noexcept {
            auto ptr1 = static_cast<const void*>(pair_of_pointers.first);
            auto ptr2 = static_cast<const void*>(pair_of_pointers.second);
            return hasher_(ptr1) * 37 + hasher_(ptr2);
        }

    private:
        std::hash<const void*> hasher_;
    };


    class TransportCatalogue {
    public:
        TransportCatalogue();
        ~TransportCatalogue();

        void SetStop(const Stop&);
        void SetRoute(const Bus&);
        void AddStopToBusMap(const std::string_view route);
        const Stop* GetStopByName(std::string_view);
        Bus* GetRouteByName(std::string_view);     

        //std::unordered_set<Bus*> GetBusContainerForStop(const Stop* stop);
        Stop_ GetBusContainerForStop(std::string_view);
        Bus_ GetBus_ByName(std::string_view);
        
        void SetDistance(const Stop*, const Stop*, size_t);
        size_t GetDistance(const Stop*, const Stop*);
        size_t GetDistanceDirectly(const Stop*, const Stop*);

        std::deque<Bus> GetAllBusesData() {
            return all_buses_data_;
        }
        std::deque<Stop> GetAllStopsData() {
            return all_stops_data_;
        }
    private:
        std::deque<Stop> all_stops_data_;
        std::unordered_map<std::string_view, const Stop*> all_stops_map_;
        std::deque<Bus> all_buses_data_;
        std::unordered_map<std::string_view, Bus*> all_buses_map_;
        
        std::unordered_map<std::pair<const Stop*, const Stop*>, size_t, PairPointersHasher> distances_map_;
        std::unordered_map<const Stop*, std::unordered_set<Bus*>> buses_in_stop_;

        std::string_view GetStopName(const Stop* stop_ptr);
        std::string_view GetStopName(const Stop stop);
        std::string_view GetBusName(const Bus* route_ptr);
        std::string_view GetBusName(const Bus route);
    };
}