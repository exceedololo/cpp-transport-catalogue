#pragma once
#include "transport_catalogue.h"    
#include <utility>          
#include <string>
#include <string_view>      
#include <istream>          
#include <iostream>         
#include <vector>

namespace transport_catalogue::detail{
    std::pair<std::string_view, std::string_view> Split(std::string_view, char, int count = 1);

    std::string_view Lstrim(std::string_view);

    std::string_view Rstrim(std::string_view);

    std::string_view TrimString(std::string_view);
}

namespace transport_catalogue::input_reader{
    void ProcessInput(TransportCatalogue&, std::istream&);
    void ProcessInputQueries(TransportCatalogue&, std::vector<InputQuery>&);

    Stop ProcessQueryAddStop(std::string&);
    void ProcessQueryAddStopsDistance(TransportCatalogue&, std::string&);
    Bus ProcessQueryAddRoute(TransportCatalogue&, std::string&);
}
