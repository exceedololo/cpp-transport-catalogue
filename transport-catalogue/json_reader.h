#pragma once

#include "transport_catalogue.h"
#include "json.h"

#include <iostream>

namespace json_reader {
	json::Document InputJSON(std::istream& input);
    class JSONReader{
    public:
        void PackJsonStop(transport_catalogue::TransportCatalogue& transport_catalogue, json::Document& doc);
        void PackJsonBus(transport_catalogue::TransportCatalogue& transport_catalogue, json::Document& doc);
        void FillJsonStop(transport_catalogue::TransportCatalogue& transport_catalogue, json::Document& doc);
        void FillJsonBus(transport_catalogue::TransportCatalogue& transport_catalogue, json::Document& doc);
    };
	void PrintJSON(transport_catalogue::TransportCatalogue& transport_catalogue, json::Document& doc, const std::string& result_map_render);
}