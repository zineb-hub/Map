#include "noeud.h"
#include <iostream>


noeud::noeud(
    const std::string& node_id,
    double lat,
    double lon,
    int streets,
    const std::vector<arc*>& outgoing_arcs,
    const std::vector<arc*>& incoming_arcs) :
    id(node_id),
    latitude(lat),
    longitude(lon),
    street_count(streets),
    visite(false), // Initialize to false
    arcs_sortants(outgoing_arcs),
    arcs_entrants(incoming_arcs) {}


void noeud::affiche() const {
    std::cout << "Node Details:\n";
    std::cout << "----------------------------------------\n";
    std::cout << "ID            : " << id << "\n";
    std::cout << "Latitude      : " << latitude << "\n";
    std::cout << "Longitude     : " << longitude << "\n";
    std::cout << "Street Count  : " << street_count << "\n";
    std::cout << "Visited       : " << (visite ? "True" : "False") << "\n";

    std::cout << "Outgoing Arcs : " << arcs_sortants.size() << " arcs\n";
    for (const auto* arc_ptr : arcs_sortants) {
        if (arc_ptr) std::cout << "  - " << arc_ptr->id << "\n";
    }

    std::cout << "Incoming Arcs : " << arcs_entrants.size() << " arcs\n";
    for (const auto* arc_ptr : arcs_entrants) {
        if (arc_ptr) std::cout << "  - " << arc_ptr->id << "\n";
    }

    std::cout << "----------------------------------------\n";
}