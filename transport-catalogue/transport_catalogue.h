#pragma once
#include "geo.h"
#include "domain.h"

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

    class PairPointersHasher {
    public:
        std::size_t operator()(const std::pair<const domain::Stop*, const domain::Stop*> pair_of_pointers) const noexcept; 

    private:
        std::hash<const void*> hasher_;
    };


    class TransportCatalogue {
    public:
        TransportCatalogue();
        ~TransportCatalogue();

        void SetStop(const domain::Stop&);
        void SetRoute(const domain::Bus&);
        void AddStopToBusMap(const std::string_view route);
        const domain::Stop* GetStopByName(std::string_view);
        domain::Bus* GetRouteByName(std::string_view);     

        domain::Stop_ GetBusContainerForStop(std::string_view);
        domain::Bus_ GetBusByName(std::string_view);
        
        void SetDistance(const domain::Stop*, const domain::Stop*, size_t);
        size_t GetDistance(const domain::Stop*, const domain::Stop*);
        size_t GetDistanceDirectly(const domain::Stop*, const domain::Stop*);

        std::deque<domain::Bus> GetAllBusesData() {
            return all_buses_data_;
        }
        std::deque<domain::Stop> GetAllStopsData() {
            return all_stops_data_;
        }
    private:
        std::deque<domain::Stop> all_stops_data_;
        std::unordered_map<std::string_view, const domain::Stop*> all_stops_map_;
        std::deque<domain::Bus> all_buses_data_;
        std::unordered_map<std::string_view, domain::Bus*> all_buses_map_;
        
        std::unordered_map<std::pair<const domain::Stop*, const domain::Stop*>, size_t, PairPointersHasher> distances_map_;
        std::unordered_map<const domain::Stop*, std::unordered_set<domain::Bus*>> buses_in_stop_;

        std::string_view GetStopName(const domain::Stop* stop_ptr);
        std::string_view GetStopName(const domain::Stop stop);
        std::string_view GetBusName(const domain::Bus* route_ptr);
        std::string_view GetBusName(const domain::Bus route);
    };
}