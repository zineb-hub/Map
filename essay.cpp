#include "essay.h"

int graphe::lecture_noeuds(const string& nomfichier) {
    ifstream infile(nomfichier);
    if (!infile) {
        cerr << "Unable to open nodes file." << endl;
        return 0;
    }

    string line;
    while (getline(infile, line)) {
        stringstream ss(line);
        string id, attributes;

        getline(ss, id, ',');
        getline(ss, attributes);

        noeud n(id);

        // Parse attributes
        size_t pos_y = attributes.find("'y':");
        size_t pos_x = attributes.find("'x':");
        size_t pos_sc = attributes.find("'street_count':");

        if (pos_y != string::npos) {
            n.latitude = stod(attributes.substr(pos_y + 5, attributes.find(',', pos_y) - pos_y - 5));
        }
        if (pos_x != string::npos) {
            n.longitude = stod(attributes.substr(pos_x + 5, attributes.find(',', pos_x) - pos_x - 5));
        }
        if (pos_sc != string::npos) {
            n.street_count = stoi(attributes.substr(pos_sc + 16, attributes.find('}', pos_sc) - pos_sc - 16));
        }

        lesnoeuds[id] = n;
    }
    nombredenoeuds = lesnoeuds.size();
    return nombredenoeuds;
}

int graphe::lecture_arcs(const string& nomfichier) {
    ifstream infile(nomfichier);
    if (!infile) {
        cerr << "Unable to open arcs file." << endl;
        return 0;
    }

    string line;
    unordered_map<string, int> name_counts;  // To handle duplicate names

    while (getline(infile, line)) {
        stringstream ss(line);
        string o, d, attributes;

        getline(ss, o, ',');
        getline(ss, d, ',');
        getline(ss, attributes);

        // Remove surrounding quotes from attributes
        if (attributes.front() == '"' && attributes.back() == '"') {
            attributes = attributes.substr(1, attributes.size() - 2);
        }

        // Create arc
        arc a;

        a.o = o;
        a.d = d;

        // Parse attributes
        size_t pos_osmid = attributes.find("'osmid':");
        size_t pos_name = attributes.find("'name':");
        size_t pos_oneway = attributes.find("'oneway':");
        size_t pos_reversed = attributes.find("'reversed':");
        size_t pos_length = attributes.find("'length':");
        size_t pos_speed = attributes.find("'speed_kph':");
        size_t pos_travel_time = attributes.find("'travel_time':");
        size_t pos_highway = attributes.find("'highway':");

        if (pos_name != string::npos) {
            size_t start = attributes.find("'", pos_name + 8) + 1;
            size_t end = attributes.find("'", start);
            a.name = attributes.substr(start, end - start);
        } else {
            a.name = o + "-" + d;  // Use node IDs if no name
        }

        // Handle duplicate names
        if (name_counts.find(a.name) != name_counts.end()) {
            name_counts[a.name]++;
            a.id = a.name + to_string(name_counts[a.name]);
        } else {
            name_counts[a.name] = 1;
            a.id = a.name;
        }

        if (pos_oneway != string::npos) {
            string oneway_str = attributes.substr(pos_oneway + 10, 5);
            a.oneway = (oneway_str.find("True") != string::npos);
        }

        if (pos_reversed != string::npos) {
            string reversed_str = attributes.substr(pos_reversed + 12, 5);
            a.reversed = (reversed_str.find("True") != string::npos);
        }

        if (pos_length != string::npos) {
            a.length = stod(attributes.substr(pos_length + 9, attributes.find(',', pos_length) - pos_length - 9));
        }

        if (pos_speed != string::npos) {
            a.speed_kph = stod(attributes.substr(pos_speed + 13, attributes.find(',', pos_speed) - pos_speed - 13));
        }

        if (pos_travel_time != string::npos) {
            a.travel_time = stod(attributes.substr(pos_travel_time + 15, attributes.find('}', pos_travel_time) - pos_travel_time - 15));
        }

        if (pos_highway != string::npos) {
            size_t start = attributes.find("'", pos_highway + 11) + 1;
            size_t end = attributes.find("'", start);
            a.highway = attributes.substr(start, end - start);
        }

        lesarcs[a.id] = a;
    }
    nombredarcs = lesarcs.size();
    return nombredarcs;
}

void graphe::liste_incidence() {
    for (auto& arc_pair : lesarcs) {
        arc& a = arc_pair.second;
        // Add outgoing arc to origin node
        if (lesnoeuds.find(a.o) != lesnoeuds.end()) {
            lesnoeuds[a.o].arcs_sortants.push_back(&a);
        }
        // Add incoming arc to destination node
        if (lesnoeuds.find(a.d) != lesnoeuds.end()) {
            lesnoeuds[a.d].arcs_entrants.push_back(&a);
        }

        // If the arc is not one-way, add reverse arc
        if (!a.oneway) {
            // Create reverse arc
            arc reverse_arc = a;
            reverse_arc.o = a.d;
            reverse_arc.d = a.o;
            reverse_arc.id += "_reverse";

            lesarcs[reverse_arc.id] = reverse_arc;

            // Update incidence lists
            lesnoeuds[reverse_arc.o].arcs_sortants.push_back(&lesarcs[reverse_arc.id]);
            lesnoeuds[reverse_arc.d].arcs_entrants.push_back(&lesarcs[reverse_arc.id]);
        }
    }
}

void graphe::reset_visites() {
    for (auto& node_pair : lesnoeuds) {
        node_pair.second.visite = false;
    }
}

void graphe::degre(int n) {
    // Min-heap to store top n nodes
    auto comp = [](const noeud* a, const noeud* b) { return a->degree() > b->degree(); };
    priority_queue<noeud*, vector<noeud*>, decltype(comp)> min_heap(comp);

    for (auto& node_pair : lesnoeuds) {
        noeud* node = &node_pair.second;
        if (min_heap.size() < n) {
            min_heap.push(node);
        } else if (node->degree() > min_heap.top()->degree()) {
            min_heap.pop();
            min_heap.push(node);
        }
    }

    // Extract nodes from heap into a vector
    vector<noeud*> top_nodes;
    while (!min_heap.empty()) {
        top_nodes.push_back(min_heap.top());
        min_heap.pop();
    }

    // Sort nodes by degree in descending order
    sort(top_nodes.begin(), top_nodes.end(), [](const noeud* a, const noeud* b) {
        return a->degree() > b->degree();
    });

    // Display nodes
    for (const auto& node : top_nodes) {
        cout << "Node ID: " << node->id << ", Degree: " << node->degree() << endl;
    }
}

int graphe::chemin(const string& o, const string& d) {
    if (lesnoeuds.find(o) == lesnoeuds.end() || lesnoeuds.find(d) == lesnoeuds.end()) {
        cerr << "Origin or destination node not found." << endl;
        return 0;
    }

    reset_visites();

    stack<noeud*> pile;
    unordered_map<string, string> prev_arc;  // To store the path

    pile.push(&lesnoeuds[o]);

    while (!pile.empty()) {
        noeud* s = pile.top();
        pile.pop();

        if (!s->visite) {
            s->visite = true;

            if (s->id == d) {
                // Path found, reconstruct path
                vector<string> path;
                string current = d;
                while (current != o) {
                    path.push_back(prev_arc[current]);
                    current = lesarcs[prev_arc[current]].o;
                }
                reverse(path.begin(), path.end());

                cout << "Path from " << o << " to " << d << " found:" << endl;
                for (const auto& arc_id : path) {
                    cout << lesarcs[arc_id].name << " (" << arc_id << ")" << endl;
                }

                return path.size();
            }

            for (auto& arc_ptr : s->arcs_sortants) {
                noeud* neighbor = &lesnoeuds[arc_ptr->d];
                if (!neighbor->visite) {
                    pile.push(neighbor);
                    prev_arc[neighbor->id] = arc_ptr->id;
                }
            }
        }
    }

    cout << "No path found from " << o << " to " << d << "." << endl;
    return 0;
}

int graphe::pluscourtchemin(const string& o, const string& d) {
    if (lesnoeuds.find(o) == lesnoeuds.end() || lesnoeuds.find(d) == lesnoeuds.end()) {
        cerr << "Origin or destination node not found." << endl;
        return 0;
    }

    reset_visites();

    queue<noeud*> file;
    unordered_map<string, string> prev_arc;  // To store the path

    lesnoeuds[o].visite = true;
    file.push(&lesnoeuds[o]);

    while (!file.empty()) {
        noeud* s = file.front();
        file.pop();

        if (s->id == d) {
            // Path found, reconstruct path
            vector<string> path;
            string current = d;
            while (current != o) {
                path.push_back(prev_arc[current]);
                current = lesarcs[prev_arc[current]].o;
            }
            reverse(path.begin(), path.end());

            cout << "Shortest path from " << o << " to " << d << ":" << endl;
            for (const auto& arc_id : path) {
                cout << lesarcs[arc_id].name << " (" << arc_id << ")" << endl;
            }

            return path.size();
        }

        for (auto& arc_ptr : s->arcs_sortants) {
            noeud* neighbor = &lesnoeuds[arc_ptr->d];
            if (!neighbor->visite) {
                neighbor->visite = true;
                file.push(neighbor);
                prev_arc[neighbor->id] = arc_ptr->id;
            }
        }
    }

    cout << "No path found from " << o << " to " << d << "." << endl;
    return 0;
}

int graphe::itineraire(const string& origine, const string& destination) {
    // Build street to nodes mapping
    unordered_map<string, vector<string>> street_to_nodes;

    for (auto& arc_pair : lesarcs) {
        arc& a = arc_pair.second;
        street_to_nodes[a.name].push_back(a.o);
        street_to_nodes[a.name].push_back(a.d);
    }

    // Remove duplicates in node lists
    for (auto& pair : street_to_nodes) {
        sort(pair.second.begin(), pair.second.end());
        pair.second.erase(unique(pair.second.begin(), pair.second.end()), pair.second.end());
    }

    if (street_to_nodes.find(origine) == street_to_nodes.end() ||
        street_to_nodes.find(destination) == street_to_nodes.end()) {
        cerr << "Origin or destination street not found." << endl;
        return 0;
    }

    // Get nodes associated with origin and destination streets
    vector<string> origin_nodes = street_to_nodes[origine];
    vector<string> dest_nodes = street_to_nodes[destination];

    // BFS from all origin nodes to find shortest path to any destination node
    reset_visites();
    queue<noeud*> file;
    unordered_map<string, string> prev_arc;

    for (const auto& node_id : origin_nodes) {
        lesnoeuds[node_id].visite = true;
        file.push(&lesnoeuds[node_id]);
    }

    while (!file.empty()) {
        noeud* s = file.front();
        file.pop();

        if (find(dest_nodes.begin(), dest_nodes.end(), s->id) != dest_nodes.end()) {
            // Path found
            vector<string> path;
            string current = s->id;
            while (prev_arc.find(current) != prev_arc.end()) {
                path.push_back(prev_arc[current]);
                current = lesarcs[prev_arc[current]].o;
            }
            reverse(path.begin(), path.end());

            cout << "Itinerary from street " << origine << " to street " << destination << ":" << endl;
            for (const auto& arc_id : path) {
                cout << lesarcs[arc_id].name << " (" << arc_id << ")" << endl;
            }

            return path.size();
        }

        for (auto& arc_ptr : s->arcs_sortants) {
            noeud* neighbor = &lesnoeuds[arc_ptr->d];
            if (!neighbor->visite) {
                neighbor->visite = true;
                file.push(neighbor);
                prev_arc[neighbor->id] = arc_ptr->id;
            }
        }
    }

    cout << "No itinerary found from street " << origine << " to street " << destination << "." << endl;
    return 0;
}

