#include "transport_catalogue.h"
#include "json_reader.h"
#include "request_handler.h"
#include <iostream>
#include "map_renderer.h"

int main() {
    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массиве "stat_requests", построив JSON-массив
     * с ответами.
     * Вывести в stdout ответы в виде JSON
     */


    transport_catalogue::TransportCatalogue catalog;
    json::Document document = json_reader::InputJSON(std::cin);
    json_reader::PackJSON(catalog, document);
    auto& j = document.GetRoot().AsMap().at("render_settings").AsMap();
    renderer::MapRenderer ren(j, catalog);
    auto str = ren.PrintingMap();
    json_reader::PrintJSON(catalog, document, str);
    
}