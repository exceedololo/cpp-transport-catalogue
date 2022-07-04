#include "json_reader.h"
#include "map_renderer.h"
#include <sstream>
#include <optional>
#include <cstdint>
#include <utility>
#include <vector>
#include <string>

namespace json_reader {
    json::Document InputJSON(std::istream& input) {
        json::Document doc = json::Load(input);
        return doc;
    }

    void JSONReader::PackJsonStop(transport_catalogue::TransportCatalogue& transport_catalogue, json::Document& doc) {
        if (doc.GetRoot().AsMap().count("base_requests")) {
            for (const auto& node_map : doc.GetRoot().AsMap().at("base_requests").AsArray()) {
                if (node_map.AsMap().at("type").AsString() == "Stop") {
                    domain::Stop st;
                    st.name = node_map.AsMap().at("name").AsString();
                    st.coords = { node_map.AsMap().at("latitude").AsDouble(),  node_map.AsMap().at("longitude").AsDouble() };
                    transport_catalogue.SetStop(st);
                }
            }
        }
    }
    
    void JSONReader::PackJsonBus(transport_catalogue::TransportCatalogue& transport_catalogue, json::Document& doc) {
        if (doc.GetRoot().AsMap().count("base_requests")) {
            for (const auto& node_map : doc.GetRoot().AsMap().at("base_requests").AsArray()) {
                if (node_map.AsMap().at("type").AsString() == "Bus") {
                    domain::Bus bs;
                    bs.bus_number = node_map.AsMap().at("name").AsString();
                    bs.is_roundtrip = node_map.AsMap().at("is_roundtrip").AsBool();
                    transport_catalogue.SetRoute(bs);
                }
            }
        }
    }
    
    void JSONReader::FillJsonStop(transport_catalogue::TransportCatalogue& transport_catalogue, json::Document& doc) {
        if (doc.GetRoot().AsMap().count("base_requests")) {
            for (const auto& node_map : doc.GetRoot().AsMap().at("base_requests").AsArray()) {
                if (node_map.AsMap().at("type").AsString() == "Stop") {
                    for (const auto& [key, val] : node_map.AsMap().at("road_distances").AsMap()) {
                        transport_catalogue.SetDistance(transport_catalogue.GetStopByName(node_map.AsMap().at("name").AsString()), transport_catalogue.GetStopByName(key), val.AsInt());
                    }
                }
            }
        }
    }
    
    void JSONReader::FillJsonBus(transport_catalogue::TransportCatalogue& transport_catalogue, json::Document& doc) {
        if (doc.GetRoot().AsMap().count("base_requests")) {
            for (const auto& node_map : doc.GetRoot().AsMap().at("base_requests").AsArray()) {      
                if (node_map.AsMap().at("type").AsString() == "Bus") {
                    auto b = node_map.AsMap().at("name").AsString();
                    for (size_t i = 0; i < node_map.AsMap().at("stops").AsArray().size(); ++i) {
                        transport_catalogue.GetRouteByName(b)->stops.push_back(transport_catalogue.GetStopByName(node_map.AsMap().at("stops").AsArray()[i].AsString()));
                    }
                    transport_catalogue.AddStopToBusMap(b);
                    int j = node_map.AsMap().at("stops").AsArray().size();
                    if (node_map.AsMap().at("is_roundtrip").AsBool() == false && j >= 2) {
                        for (j = j - 2; j >= 0; ) {
                            transport_catalogue.GetRouteByName(b)->stops.push_back(transport_catalogue.GetStopByName(node_map.AsMap().at("stops").AsArray()[j].AsString()));
                            --j;
                        }
                    }
                }
            }
        }
    }

    void PrintJSON(transport_catalogue::TransportCatalogue& transport_catalogue, json::Document& doc, const std::string& result_map_render) {
        using namespace std::literals;
        json::Array arr;
        if (doc.GetRoot().AsMap().count("stat_requests")) {
            for (const auto& node_map : doc.GetRoot().AsMap().at("stat_requests").AsArray()) {
                int id_q = node_map.AsMap().at("id").AsInt();
                if (node_map.AsMap().at("type").AsString()[0] == 'B') {
                    std::string tmp = node_map.AsMap().at("name").AsString();
                    if (transport_catalogue.GetRouteByName(tmp) != nullptr) {
                        arr.emplace_back(json::Dict{
                            {"curvature", json::Node(transport_catalogue.GetBusByName(tmp).curvature)},
                            {"request_id", id_q},
                            {"route_length", json::Node(transport_catalogue.GetBusByName(tmp).meters_route_length)},
                            {"stop_count", json::Node(static_cast<int>(transport_catalogue.GetRouteByName(transport_catalogue.GetBusByName(tmp).bus_number_)->stops.size()))},
                            {"unique_stop_count", json::Node(transport_catalogue.GetBusByName(tmp).unique_stops_qty)},
                        });
                    }
                    else {
                        arr.emplace_back(
                            json::Dict{
                            {"request_id", id_q},
                            {"error_message", "not found"s}, });
                    }
                }
                else if (node_map.AsMap().at("type").AsString()[0] == 'S') {
                    std::string tmp = node_map.AsMap().at("name").AsString();
                    if (transport_catalogue.GetStopByName(tmp) != nullptr) {
                        json::Array arr_bus;
                        for (const auto& elem : transport_catalogue.GetBusContainerForStop(tmp).bus_number_) {
                            arr_bus.emplace_back(elem);
                        }
                        arr.emplace_back(json::Dict{
                            {"buses", arr_bus},
                            {"request_id", id_q},
                            });
                    }
                    else {
                        arr.emplace_back(
                            json::Dict{
                            {"request_id", id_q},
                            {"error_message", "not found"s}, });

                    }
                }
                else if (node_map.AsMap().at("type").AsString()[0] == 'M') {
                    arr.emplace_back(json::Dict{
                            {"map", json::Node(result_map_render)},
                            {"request_id", id_q},
                    });
                }
            }
        }
        json::Print(json::Document(arr), std::cout);
    }
}
