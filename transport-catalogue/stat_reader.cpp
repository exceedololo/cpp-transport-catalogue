#include "stat_reader.h"

namespace transport_catalogue::stat_reader {
    std::ostream& operator<<(std::ostream& os, const Stop* stop) {
        using namespace std::string_literals;
        os << "Stop "s << stop->name << ": "s;
        os << std::fixed << std::setprecision(6);
        os << stop->coords.lat << "s, ";
        os << stop->coords.lng;
        return os;
    }
    std::ostream& operator<<(std::ostream& os, const Route* route) {
        using namespace std::string_literals;
        if (route != nullptr) {
            os << "Bus "s << route->bus_number << ": "s;
            if (route->stops.size()) {
                os << route->stops.size() << " stops on route, "s;
                os << route->unique_stops_qty << " unique stops, "s;
                os << route->meters_route_length << " route length, "s;
                os << std::setprecision(6);
                os << route->curvature << " curvature"s;
            }
            else {
                os << "no stops"s;
            }
        }
        return os;
    }

    std::ostream& ProcessRequests(std::ostream& os, TransportCatalogue& tc, std::istream& is)  {
        std::string line;
        std::getline(is, line);
        size_t request_num = static_cast<size_t>(std::stoul(line));
        for (size_t i = 0; i < request_num; ++i)  {
            std::getline(is, line, '\n');
            auto tmp = detail::Split(line, ' ');
            tmp.first = detail::TrimString(tmp.first);
            tmp.second = detail::TrimString(tmp.second);
            if (tmp.second.empty())  {
                continue;
            }
            using namespace std::literals;
            RequestQuery query;
            if (tmp.first == "Bus"sv) {
                query.type = RequestQueryType::GetRouteByName;
            }
            else if (tmp.first == "Stop"sv)  {
                query.type = RequestQueryType::GetBusesForStop;
            }
            else {
                query.type = RequestQueryType::NoOp;
            }
            query.params = tmp.second;
            ExecuteRequest(std::cout, tc, query);
        }
        return os;
    }

    std::ostream& ExecuteRequest(std::ostream& os, TransportCatalogue& tc, RequestQuery& query) {
        using namespace std::literals;
        if (query.type == RequestQueryType::GetRouteByName) {
            RequestResult result = tc.GetRouteInfo(query.params);
            if (result.code == RequestResultType::Ok) {
                std::stringstream ss;
                ss << result.r_ptr;
                os << ss.str() << std::endl;
            }
            else    {
                std::stringstream ss;
                ss << "Bus "s << std::string(query.params) << ": not found"s;
                os << ss.str() << std::endl;
            }
        } else if (query.type == RequestQueryType::GetBusesForStop)  {
            RequestResult result = tc.GetBusesForStop(query.params);
            if (result.code == RequestResultType::Ok)  {
                std::stringstream ss;
                for (auto& element : result.vector_str)  {
                    ss << " "s << element;
                }
                std::cout << "Stop "s + std::string(query.params) + ": buses"s + ss.str() << std::endl;
            }
            else if (result.code == RequestResultType::NoBuses) {
                os << "Stop "s + std::string(query.params) + ": no buses"s << std::endl;
            } else if (result.code == RequestResultType::StopNotExists) {
                os << "Stop "s + std::string(query.params) + ": not found"s << std::endl;
            }
        } else if (query.type == RequestQueryType::NoOp) {
            return os;
        }
        return os;
    }
}