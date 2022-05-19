#pragma once
#include "transport_catalogue.h"
#include "input_reader.h"
#include <utility>
#include <string>
#include <string_view>
#include <sstream>
#include <iomanip>
#include <istream>
#include <iostream>
namespace transport_catalogue::stat_reader {
    std::string PrintBus(const Bus_& bus, TransportCatalogue& tc);
    std::string PrintStop(const Stop*, TransportCatalogue& tc, std::string_view stop_name);
    template<typename T, typename U>
    void ProcessRequests(T& is, U& os, TransportCatalogue& tc);
}
template<typename T, typename U>
void transport_catalogue::stat_reader::ProcessRequests(T& is, U& os, transport_catalogue::TransportCatalogue& tc) {
    std::string num;
    std::getline(is, num);
    for (size_t i = 0; i < std::stoi(num); ++i) {
        using namespace std::literals;
        std::string line;
        std::getline(is, line);
        auto tmp = detail::Split(line, ' ');
        tmp.first = detail::TrimString(tmp.first);
        tmp.second = detail::TrimString(tmp.second);
        if (tmp.first == "Stop"sv) {
            const auto stop_info = tc.GetBusesForStop(tmp.second);
            os << PrintStop(stop_info.s_ptr, tc, tmp.second);
        }
        if (tmp.first == "Bus"sv) {
            const auto bus_info = tc.GetRouteInfo(tmp.second);
            os << PrintBus(bus_info.r_ptr, tc);
        }
    }
}
