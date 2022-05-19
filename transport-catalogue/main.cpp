// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "geo.h"
#include "input_reader.h"
#include "transport_catalogue.h"
#include "stat_reader.h"
#include <iostream>       

#include <sstream>

int main()
{
    using namespace std::string_literals;
    transport_catalogue::TransportCatalogue tc;
    transport_catalogue::input_reader::ProcessInput(tc, std::cin);
    transport_catalogue::stat_reader::ProcessRequests(std::cout, tc, std::cin);
    return 0;
}