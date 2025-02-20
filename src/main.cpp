#include "graphe.h"

int main() {
    // Initialize the graph with data files
    std::string nodes_file = "../data/nodes.csv";
    std::string arcs_file = "../data/arcs.csv";

    graphe g(arcs_file, nodes_file);

    std::cout << "\nGraph initialized successfully.\n";

    // Display first 10 nodes
    g.affiche_premiers_noeuds();

    // Display first 10 arcs
    g.affiche_premiers_arcs();

    //display first 10 nodes with their neighbors
    g.displayNeighbors();

    // Test `chemin` function
    std::cout << "\nTesting `chemin` function:\n";
    std::string origin_node = "152293"; //  node ID
    std::string destination_node = "940356236"; //  node ID
    int chemin_arcs = g.chemin(origin_node, destination_node);
    if (chemin_arcs > 0) {
        std::cout << "Path found with " << chemin_arcs << " arcs.\n";
    } else {
        std::cout << "No path found between nodes.\n";
    }

    // Test `pluscourtchemin` function
    std::cout << "\nTesting `pluscourtchemin` function:\n";
    int shortest_arcs = g.pluscourtchemin(origin_node, destination_node); // Replace with another node ID
    if (shortest_arcs > 0) {
        std::cout << "Shortest path found with " << shortest_arcs << " arcs.\n";
    } else {
        std::cout << "No shortest path found between nodes.\n";
    }

    // Test `itineraire` function
    std::cout << "\nTesting `itineraire` function:\n";
    std::string origine_street = "Impasse Favot"; // Replace with an actual street name
    std::string destination_street = "Boulevard Paul Marièton4"; // Replace with another street name
    double itinerary_time = g.itineraire(origine_street, destination_street);
    if (itinerary_time > 0) {
        std::cout << "Itinerary found with minimum time in " << itinerary_time << " mins.\n";
    } else {
        std::cout << "No itinerary found between streets.\n";
    }

    // Final message
    std::cout << "\nTesting complete. Check the outputs above.\n";

    return 0;
}