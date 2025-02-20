#include "graphe.h"
#include <unordered_set>
#include <regex>

using namespace std;

// Constructeur pour remplir le graphes (les noueds et les arcs)
graphe::graphe(const string &fichierarcs, const string &fichiernoeuds)
{
    lecture_noeuds(fichiernoeuds);
    lecture_arcs(fichierarcs);
    //displayNeighbors();

}

// On lit les noueds du fichier sans ses arcs
int graphe::lecture_noeuds(const std::string& nomfichier) {
    std::ifstream infile(nomfichier);
    if (!infile) {
        std::cerr << "Unable to open nodes file." << std::endl;
        return 0;
    }

    std::string line;
    while (std::getline(infile, line)) {
        std::stringstream ss(line);
        std::string node_id, attributes;

        // Split the line into node_id and attributes
        std::getline(ss, node_id, ',');
        std::getline(ss, attributes);

        // Remove surrounding quotes from attributes
        if (!attributes.empty() && attributes.front() == '"' && attributes.back() == '"') {
            attributes = attributes.substr(1, attributes.size() - 2);
        }

        // Parse attributes
        double latitude = 0.0, longitude = 0.0;
        int street_count = 0;

        auto extract = [&](const std::string& key, auto& value, auto parser) {
            size_t pos = attributes.find("'" + key + "':");
            if (pos != std::string::npos) {
                size_t start = attributes.find_first_of("0123456789.-", pos + key.length() + 3);
                size_t end = attributes.find_first_of(",}", start);
                std::string raw_value = attributes.substr(start, end - start);
                value = parser(raw_value);
            }
        };

        extract("y", latitude, [](const std::string& s) { return std::stod(s); });
        extract("x", longitude, [](const std::string& s) { return std::stod(s); });
        extract("street_count", street_count, [](const std::string& s) { return std::stoi(s); });

        // Collect arcs associated with this node
        // fill them later when reading arcs
        std::vector<arc*> outgoing_arcs;
        std::vector<arc*> incoming_arcs;

        // Create the node using the constructor
        //cout << "node info" << " "<< node_id << " " << latitude << " " << longitude << " " << street_count << endl;
        noeud new_node(node_id, latitude, longitude, street_count, outgoing_arcs, incoming_arcs);

        //new_node.affiche();
        // Insert the node into the graph
        lesnoeuds[node_id] = new_node;
    }

    nombredenoeuds = lesnoeuds.size();
    return nombredenoeuds;
}

// On lit les arc et leurs infos
int graphe::lecture_arcs(const std::string& nomfichier) {
    std::ifstream infile(nomfichier);
    if (!infile) {
        std::cerr << "Unable to open arcs file." << std::endl;
        return 0;
    }

    std::string line;

    while (std::getline(infile, line)) {
        std::stringstream ss(line);
        std::string origin, destination, attributes;

        // Split the line into origin, destination, and attributes
        std::getline(ss, origin, ',');
        std::getline(ss, destination, ',');
        std::getline(ss, attributes);

        // Remove surrounding quotes from attributes
        if (!attributes.empty() && attributes.front() == '"' && attributes.back() == '"') {
            attributes = attributes.substr(1, attributes.size() - 2);
        }

        // Extract attributes using string parsing
        std::string osmid_id = "";
        bool oneway = false, reversed = false;
        double length = 0.0, maxspeed = 0.0, speed_kph = 0.0;
        std::string name = "", highway = "";
        int lanes = 0;
        std::vector<Point> geometry;
        double travel_time = 0.0;
        name = extract_name(attributes);
        auto extract = [&](const std::string& key, auto& value, auto parser) {
            size_t pos = attributes.find("'" + key + "':");
            if (pos != std::string::npos) {
                size_t start = attributes.find_first_of("0123456789-'\"", pos + key.length() + 3);
                size_t end = attributes.find_first_of(",}", start);
                std::string raw_value = attributes.substr(start, end - start);

                // Remove surrounding single quotes if present
                if (!raw_value.empty() && raw_value.front() == '\'' && raw_value.back() == '\'') {
                    raw_value = raw_value.substr(1, raw_value.size() - 2);
                }

                try {
                    if constexpr (std::is_same_v<std::decay_t<decltype(value)>, std::string>) {
                        value = raw_value; // Directly assign for strings
                    } else {
                        value = parser(raw_value); // Parse numeric values
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing key '" << key << "' with value '" << raw_value << "': " << e.what() << std::endl;
                    value = {}; // Reset to default value
                }
            }
        };

        extract("osmid", osmid_id, [](const std::string& s) { return s; });
        extract("highway", highway, [](const std::string& s) { return s; });
        extract("oneway", oneway, [](const std::string& s) { return s == "True"; });
        extract("reversed", reversed, [](const std::string& s) { return s == "True"; });
        extract("length", length, [](const std::string& s) { return std::stod(s); });
        maxspeed = extract_maxspeed("maxspeed");
        extract("lanes", lanes, [](const std::string& s) { return std::stoi(s); });
        extract("speed_kph", speed_kph, [](const std::string& s) { return std::stod(s); });
        extract("travel_time", travel_time, [](const std::string& s) { return std::stod(s); });

        // Handle oneway and reversed attributes
        if (oneway && reversed) {
            std::swap(origin, destination); // Switch source and destination
        }

        // Create the arc
        arc new_arc(origin, destination, osmid_id, name, highway, length, maxspeed, speed_kph, lanes, geometry, travel_time);
        lesarcs[new_arc.id] = new_arc; // Insert into the graph
        liste_incidence(origin, destination, lesarcs[new_arc.id]);//Pour remplir les list des arcs sotants et entrants des noueds

        if (!oneway) {
            arc reverse_arc(destination, origin, osmid_id + "_reverse", name, highway, length, maxspeed, speed_kph, lanes, geometry, travel_time);
            lesarcs[reverse_arc.id] = reverse_arc;
            liste_incidence(destination, origin, lesarcs[reverse_arc.id]);
        }
    }

    nombredarcs = lesarcs.size();
    return nombredarcs;
}

// Les names sont compliqués à extracter donc j'ai fais une fonction supplementaire
std::string graphe::extract_name(const std::string& attributes) {
    // Find the position of the 'name' key
    size_t pos = attributes.find("'name':");
    if (pos == std::string::npos) {
        // 'name' key not found
        return "";
    }

    // Determine the start of the value
    size_t start = attributes.find_first_of("'\"[", pos + 7); // Skip "'name':"
    if (start == std::string::npos) {
        return "";
    }

    // Case 1: Array of names
    if (attributes[start] == '[') {
        //std::cout<<"stats with []: "<<attributes<<std::endl;
        size_t end = attributes.find("]", start);
        if (end == std::string::npos) {
            return ""; // Malformed attribute
        }

        // Extract the content inside the array
        std::string raw_value = attributes.substr(start + 1, end - start - 1);

        // Process individual names inside the array
        std::stringstream ss(raw_value);
        std::string part, result;
        while (std::getline(ss, part, ',')) {
            // Remove quotes around each name
            part.erase(remove(part.begin(), part.end(), '\''), part.end());
            part.erase(remove(part.begin(), part.end(), '"'), part.end());
            result += part + " et"; // Concatenate with space
        }

        // Trim the final string and return
        result.erase(result.find_last_not_of(" \n\r\t") + 1);
        //std::cout<<"the end] : "<<result<<std::endl;

        return result;
    }

    // Case 2: Single name in double quotes
    if (attributes[start] == '"') {
        size_t end = attributes.find("\"", start + 1);
        if (end == std::string::npos) {
            return ""; // Malformed attribute
        }
        return attributes.substr(start + 1, end - start - 1); // Extract content
    }

    // Case 3: Single name in single quotes
    if (attributes[start] == '\'') {
        size_t end = attributes.find("'", start + 1);
        if (end == std::string::npos) {
            return ""; // Malformed attribute
        }
        return attributes.substr(start + 1, end - start - 1); // Extract content
    }

    return ""; // Default fallback
}

// Les max speed peut etre array donc on a besoin de la fonction suivante pour prendre la valeur maximale
double graphe::extract_maxspeed(const std::string& attributes) {
    string key = "maxspeed";
    size_t pos = attributes.find("'" + key + "':");
    if (pos == std::string::npos) {
        return 0.0; // Default if 'maxspeed' not found
    }

    size_t start = attributes.find_first_of("['\"", pos + key.length() + 3);
    if (start == std::string::npos) {
        return 0.0; // Malformed attribute
    }

    // Case 1: Array of maxspeeds
    if (attributes[start] == '[') {
        size_t end = attributes.find("]", start);
        if (end == std::string::npos) {
            return 0.0; // Malformed array
        }

        // Extract the content inside the array
        std::string raw_values = attributes.substr(start + 1, end - start - 1);

        // Process individual values
        std::stringstream ss(raw_values);
        std::string part;
        double max_value = 0.0;
        while (std::getline(ss, part, ',')) {
            // Remove quotes and trim
            part.erase(remove(part.begin(), part.end(), '\''), part.end());
            part.erase(remove(part.begin(), part.end(), '"'), part.end());
            try {
                double value = std::stod(part);
                if (value > max_value) {
                    max_value = value; // Update max_value if the current value is greater
                }
            } catch (const std::exception& e) {
                std::cerr << "Error parsing maxspeed value: " << part << "\n";
            }
        }

        return max_value; // Return the largest value in the array
    }

    // Case 2: Single maxspeed value
    if (attributes[start] == '\'' || attributes[start] == '"') {
        size_t end = attributes.find(attributes[start], start + 1);
        if (end == std::string::npos) {
            return 0.0; // Malformed attribute
        }
        std::string value = attributes.substr(start + 1, end - start - 1);
        try {
            return std::stod(value);
        } catch (const std::exception& e) {
            std::cerr << "Error parsing maxspeed value: " << value << "\n";
            return 0.0;
        }
    }

    return 0.0; // Default fallback
}

// Dependant de chaque arcs on le met dans les arcs entrant ou sortants de chaque noeud
void graphe::liste_incidence(const std::string& source, const std::string& destination, arc& arc_obj) {
    // Update the outgoing arcs of the source node
    if (lesnoeuds.find(source) != lesnoeuds.end()) {
        lesnoeuds[source].arcs_sortants.push_back(&lesarcs[arc_obj.id]);
    }

    // Update the incoming arcs of the destination node
    if (lesnoeuds.find(destination) != lesnoeuds.end()) {
        lesnoeuds[destination].arcs_entrants.push_back(&lesarcs[arc_obj.id]);
    }
}

void graphe::affiche_premiers_arcs() const {
    if (lesarcs.empty()) {
        std::cout << "No arcs in the graph.\n";
        return;
    }

    std::cout << "\nDisplaying the first 10 arcs of the graph:\n";
    int count = 0;
    for (const auto& pair : lesarcs) {
        if (count >= 10) break; // Limit to 10 rows
        const arc& a = pair.second;
        a.affiche();

        ++count;
    }
}

void graphe::affiche_premiers_noeuds() const {
    if (lesnoeuds.empty()) {
        std::cout << "No nodes in the graph.\n";
        return;
    }

    std::cout << "\nDisplaying the first 10 nodes of the graph:\n";
    int count = 0;
    for (const auto& pair : lesnoeuds) {
        if (count >= 10) break; // Limit to 10 rows
        const noeud& n = pair.second;

        n.affiche();

        ++count;
    }
}

void graphe::displayNeighbors() {
    int count = 0; // Counter to limit output to the first 10 nodes

    // Iterate through all nodes in the graph
    for (const auto& node_pair : lesnoeuds) {
        if (count >= 10) break; // Stop after displaying 10 nodes

        const noeud& current_node = node_pair.second;

        std::cout << "Node ID: " << current_node.id << "\n";
        std::cout << "  Outgoing Neighbors (Sorting):\n";

        // Display all outgoing neighbors (sorting)
        for (const auto* arc_ptr : current_node.arcs_sortants) {
            if (arc_ptr) {
                std::cout << "    -> " << arc_ptr->d << " (via Arc ID: " << arc_ptr->id << ")\n";
            }
        }

        std::cout << "  Incoming Neighbors (Entering):\n";

        // Display all incoming neighbors (entering)
        for (const auto* arc_ptr : current_node.arcs_entrants) {
            if (arc_ptr) {
                std::cout << "    <- " << arc_ptr->o << " (via Arc ID: " << arc_ptr->id << ")\n";
            }
        }

        std::cout << "----------------------------------------\n";

        ++count; // Increment the counter
    }

    if (count == 0) {
        std::cout << "No nodes available in the graph.\n";
    }
}

void graphe::degre(int n) {
    // Check if n is within bounds
    if (n < 1 || n > nombredenoeuds) {
        std::cerr << "Invalid value of n. It should be between 1 and " << nombredenoeuds << ".\n";
        return;
    }

    // Vector to store pairs of node ID and degree
    std::vector<std::pair<std::string, int>> node_degrees;

    // Compute degrees of all nodes
    for (const auto& pair : lesnoeuds) {
        const std::string& node_id = pair.first;
        const noeud& node = pair.second;
        int degree = node.arcs_sortants.size() + node.arcs_entrants.size();
        node_degrees.push_back({node_id, degree});
    }

    // Sort using Quick Sort
    quickSort(node_degrees, 0, node_degrees.size() - 1);

    // Display the top n nodes
    std::cout << "Top " << n << " nodes with the highest degree:\n";
    for (int i = 0; i < n; ++i) {
        const std::string& node_id = node_degrees[i].first;
        int degree = node_degrees[i].second;
        std::cout << "Node ID: " << node_id << ", Degree: " << degree << "\n";
    }
}

// Quick Sort pour trier les paires de node_id et degrés
void graphe::quickSort(std::vector<std::pair<std::string, int>>& arr, int low, int high) {
    if (low < high) {
        // Partition the array and get the pivot index
        int pivotIndex = partition(arr, low, high);

        // Recursively sort the sub-arrays
        quickSort(arr, low, pivotIndex - 1);
        quickSort(arr, pivotIndex + 1, high);
    }
}

// Partition Function pour le quick sort
int graphe::partition(std::vector<std::pair<std::string, int>>& arr, int low, int high) {
    // Choose the last element as the pivot
    auto pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        // Sort in descending order
        if (arr[j].second >= pivot.second) {
            ++i;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

// La fonction pour parcourir le graphe en profondeur 
int graphe::chemin(const std::string& o, const std::string& d) {
    // Check if origin and destination nodes exist in the graph
    if (lesnoeuds.find(o) == lesnoeuds.end()) {
        std::cerr << "Origin node not found.\n";
        return 0;
    }
    if (lesnoeuds.find(d) == lesnoeuds.end()) {
        std::cerr << "Destination node not found.\n";
        return 0;
    }

    // Reset the visited status of all nodes
    for (auto& pair : lesnoeuds) {
        pair.second.visite = false;
    }

    // Stack for DFS
    std::stack<std::string> pile;
    std::unordered_map<std::string, std::string> parent;

    // Start DFS from the origin node
    pile.push(o);

    while (!pile.empty()) {
        std::string current_id = pile.top();
        pile.pop();
        noeud& current_node = lesnoeuds[current_id];

        // If the node is already visited, skip it
        if (!current_node.visite) {
            current_node.visite = true;

            // If we reached the destination, reconstruct and display the path
            if (current_id == d) {
                std::vector<std::string> path_nodes;
                std::vector<std::string> path_arcs;

                std::string node_id = d;
                while (node_id != o) {
                    path_nodes.push_back(node_id);
                    std::string parent_id = parent[node_id];

                    // Find the arc between parent_id and node_id
                    noeud& parent_node = lesnoeuds[parent_id];
                    std::string arc_name = "";
                    for (arc* arc_ptr : parent_node.arcs_sortants) {
                        if (arc_ptr->d == node_id) {
                            arc_name = arc_ptr->id;
                            break;
                        }
                    }
                    path_arcs.push_back(arc_name);

                    node_id = parent_id;
                }
                path_nodes.push_back(o);

                // Reverse the path to get it in the correct order
                std::reverse(path_nodes.begin(), path_nodes.end());
                std::reverse(path_arcs.begin(), path_arcs.end());

                // Display the path
                std::cout << "Path from " << o << " to " << d << ":\n";
                for (size_t i = 0; i < path_arcs.size(); ++i) {
                    std::cout << "From node " << path_nodes[i] << " to node " << path_nodes[i + 1]
                              << " via route: " << path_arcs[i] << "\n";
                }

                // Return the number of arcs in the path
                return static_cast<int>(path_arcs.size());
            }

            // Explore the neighbors of the current node
            for (arc* arc_ptr : current_node.arcs_sortants) {
                std::string neighbor_id = arc_ptr->d;
                if (!lesnoeuds[neighbor_id].visite) {
                    pile.push(neighbor_id);
                    parent[neighbor_id] = current_id;
                }
            }
        } 
    }

    // If no path is found
    std::cout << "No path from " << o << " to " << d << " found.\n";
    return 0;
}

// le plus court chemin en utilisant l'algorithme de largeur
int graphe::pluscourtchemin(const std::string& o, const std::string& d) {
    if (lesnoeuds.find(o) == lesnoeuds.end() || lesnoeuds.find(d) == lesnoeuds.end()) {
        std::cerr << "Origin or destination node not found.\n";
        return 0;
    }

    for (auto& pair : lesnoeuds) pair.second.visite = false;

    std::queue<std::string> file;
    std::unordered_map<std::string, std::string> parent;

    file.push(o);
    lesnoeuds[o].visite = true;

    while (!file.empty()) {
        std::string current_id = file.front();
        file.pop();

        if (current_id == d) {
            // Path reconstruction and display
            std::vector<std::string> path_arcs, path_nodes;
            std::string node_id = d;

            while (node_id != o) {
                path_nodes.push_back(node_id);
                std::string parent_id = parent[node_id];
                for (arc* a : lesnoeuds[parent_id].arcs_sortants) {
                    if (a->d == node_id) {
                        path_arcs.push_back(a->id);
                        break;
                    }
                }
                node_id = parent_id;
            }
            path_nodes.push_back(o);
            std::reverse(path_nodes.begin(), path_nodes.end());
            std::reverse(path_arcs.begin(), path_arcs.end());

            std::cout << "Shortest Path:\n";
            for (size_t i = 0; i < path_arcs.size(); ++i) {
                std::cout << path_nodes[i] << " -> " << path_nodes[i + 1] << " via " << path_arcs[i] << "\n";
            }
            return static_cast<int>(path_arcs.size());
        }

        for (arc* a : lesnoeuds[current_id].arcs_sortants) {
            if (!lesnoeuds[a->d].visite) {
                lesnoeuds[a->d].visite = true;
                parent[a->d] = current_id;
                file.push(a->d);
            }
        }
    }

    std::cout << "No shortest path found.\n";
    return 0;
}

// Le plus court chemin entre deux rues en utilisant la fonction pluscourtchemin
// dés coup on peut pensé du rues comme des noueds, donc on cherchera le plus court chemin entre chaque deux noueds de rues d'origine et destination
double graphe::itineraire(const std::string& origine, const std::string& destination) {
    // Identify all nodes associated with the 'origine' street
    std::unordered_set<std::string> origin_nodes;
    for (const auto& pair : lesarcs) {
        const arc& a = pair.second;
        if (a.id == origine) {
            origin_nodes.insert(a.o);
            origin_nodes.insert(a.d);
        }
    }

    // Identify all nodes associated with the 'destination' street
    std::unordered_set<std::string> destination_nodes;
    for (const auto& pair : lesarcs) {
        const arc& a = pair.second;
        if (a.id == destination) {
            destination_nodes.insert(a.o);
            destination_nodes.insert(a.d);
        }
    }

    // Check if streets exist
    if (origin_nodes.empty()) {
        std::cerr << "Street '" << origine << "' not found.\n";
        return 0;
    }
    if (destination_nodes.empty()) {
        std::cerr << "Street '" << destination << "' not found.\n";
        return 0;
    }

    // Variables to store the best path
    float min_travel_time = INT_MAX;
    int number_arcs = INT_MAX;
    std::vector<std::string> best_path_nodes;
    std::vector<std::string> best_path_arcs;

    // Iterate over all combinations of origin and destination nodes
    for (const std::string& origin_node : origin_nodes) {
        for (const std::string& destination_node : destination_nodes) {
            // Reset visited status for all nodes
            for (auto& pair : lesnoeuds) pair.second.visite = false;

            // BFS setup
            std::queue<std::string> queue;
            std::unordered_map<std::string, std::string> parent;

            queue.push(origin_node);
            lesnoeuds[origin_node].visite = true;

            bool path_found = false;

            while (!queue.empty()) {
                std::string current_id = queue.front();
                queue.pop();

                // If destination node is reached, break
                if (current_id == destination_node) {
                    path_found = true;
                    break;
                }

                // Explore neighbors
                for (arc* a : lesnoeuds[current_id].arcs_sortants) {
                    std::string neighbor_id = a->d;
                    if (!lesnoeuds[neighbor_id].visite) {
                        lesnoeuds[neighbor_id].visite = true;
                        parent[neighbor_id] = current_id;
                        queue.push(neighbor_id);
                    }
                }
            }

            // If a path was found, reconstruct it
            if (path_found) {
                std::vector<std::string> path_nodes;
                std::vector<std::string> path_arcs;

                std::string node_id = destination_node;
                while (node_id != origin_node) {
                    path_nodes.push_back(node_id);
                    std::string parent_id = parent[node_id];

                    // Find the arc from parent_id to node_id
                    for (arc* a : lesnoeuds[parent_id].arcs_sortants) {
                        if (a->d == node_id) {
                            path_arcs.push_back(a->id);
                            break;
                        }
                    }
                    node_id = parent_id;
                }
                path_nodes.push_back(origin_node);

                // Reverse the path to display it correctly
                std::reverse(path_nodes.begin(), path_nodes.end());
                std::reverse(path_arcs.begin(), path_arcs.end());

                
                // Update the best path if this one is shorter

                double total_travel_time = 0.0;
                
                // Calculate the total travel time for the current path
                for (size_t i = 0; i < path_arcs.size(); ++i) {
                    const std::string& arc_id = path_arcs[i];
                    if (lesarcs.find(arc_id) != lesarcs.end()) {
                        total_travel_time += lesarcs[arc_id].travel_time;
                    }
                }
                if (total_travel_time < min_travel_time) {
                    number_arcs = path_arcs.size();
                    min_travel_time = total_travel_time;
                    best_path_nodes = path_nodes;
                    best_path_arcs = path_arcs;
                }
            }
        }
    }

    // Display the best path if found
    if (min_travel_time != INT_MAX) {
        std::cout << "Itinerary from \"" << origine << "\" to \"" << destination << "\":\n";
        for (size_t i = 0; i < best_path_arcs.size(); ++i) {
            std::cout << "From node " << best_path_nodes[i] << " to node " << best_path_nodes[i + 1]
                      << " via street: " << best_path_arcs[i] << "\n";
        }
        std::cout << "Itinerary found with minimum time in " << number_arcs << " streets.\n";
        return min_travel_time;
    }

    // If no path is found
    std::cout << "No itinerary from \"" << origine << "\" to \"" << destination << "\" found.\n";
    return 0;
}