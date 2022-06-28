#pragma once

#include "transport_catalogue.h"
#include "json.h"

#include <iostream>

namespace json_reader {
	json::Document InputJSON(std::istream& input);
	void PackJSON(transport_catalogue::TransportCatalogue& transport_catalogue, json::Document& doc);
	void PrintJSON(transport_catalogue::TransportCatalogue& transport_catalogue, json::Document& doc, std::string result_map_render);
}