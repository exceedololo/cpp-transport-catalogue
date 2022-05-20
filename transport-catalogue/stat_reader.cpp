#include "stat_reader.h"
#include "transport_catalogue.h"

namespace transport_catalogue::stat_reader {
    std::string PrintBus(const Bus_& bus, TransportCatalogue& tc) {
        std::stringstream stream;

        if (tc.GetRouteByName(bus.bus_number_)!=nullptr) {
            stream << "Bus " << bus.bus_number_ << ": " << tc.GetRouteByName(bus.bus_number_)->stops.size()
                   << " stops on route, "
                   << bus.unique_stops_qty << " unique stops, " << bus.meters_route_length << " route length"
                   << ", " << std::setprecision(6) << bus.curvature << " curvature" << std::endl;
        }
        else {
            stream << "Bus " << bus.bus_number_ << ": "
                   << "not found" << std::endl;
        }
        return stream.str();
    }

    std::string PrintStop(const Stop* stop, TransportCatalogue& tc, std::string_view stop_name) {
        std::stringstream os;
        if (stop != nullptr) {
            if (!tc.GetBusContainerForStop(stop).empty()) {
                os << "Stop " << stop_name << ": "
                   << "buses ";
                std::vector<std::string_view> found_buses;
                for (auto& bus : tc.GetBusContainerForStop(stop)) {
                    found_buses.push_back(bus->bus_number);
                }
                std::sort(found_buses.begin(), found_buses.end());
                for (auto& founded: found_buses){
                    os << founded << " ";
                }
                os << std::endl;
            }
            else {
                os << "Stop " << stop->name << ": "
                   << "no buses" << std::endl;
            }
        }
        else {
            os << "Stop " << stop_name << ": "
               << "not found" << std::endl;
        }
        return os.str();
    }
}
