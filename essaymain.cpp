#include "essay.h"

int main() {
    graphe g("arcs.csv", "nodes.csv");
    g.lecture_noeuds("nodes.csv");
    g.lecture_arcs("arcs.csv");
    g.liste_incidence();

    // Example usage
    g.degre(5);  // Display top 5 nodes by degree

    // Find a path between two nodes
    g.chemin("152293", "152294");

    // Find the shortest path between two nodes
    g.pluscourtchemin("152293", "1674101863");

    // Find the shortest itinerary between two streets
    g.itineraire("Avenue du Moulin de Notre-Dame", "Rue de la République");

    return 0;
}