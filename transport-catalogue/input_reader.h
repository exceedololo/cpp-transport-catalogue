#pragma once
#include "transport_catalogue.h"
#include <utility>
#include <string>
#include <string_view>
#include <istream>
#include <iostream>
#include <vector>

namespace transport_catalogue::detail {
    std::pair<std::string_view, std::string_view> Split(std::string_view, char, int count = 1);

    std::string_view Lstrim(std::string_view);

    std::string_view Rstrim(std::string_view);

    std::string_view TrimString(std::string_view);
}

namespace transport_catalogue::input_reader {
    void ProcessInput(TransportCatalogue& tc, std::istream& is);
    void ProcessInputQueries(TransportCatalogue& tc, std::vector<InputQuery>& queries);

    Stop ProcessQueryAddStop(std::string& query);
    void ProcessQueryAddStopsDistance(TransportCatalogue& tc, std::string& query);
    Bus ProcessQueryAddRoute(TransportCatalogue& tc, std::string& query);
}
