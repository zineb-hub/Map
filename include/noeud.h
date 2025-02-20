#include <iostream>
#include <string>
#include <vector>
#include "arc.h"

using namespace std;

class graphe;

class noeud {
protected:
    string id;
    long double latitude;
    long double longitude;
    int street_count;
    bool visite;  // For traversal algorithms

    vector<arc*> arcs_sortants;  // Outgoing arcs
    vector<arc*> arcs_entrants;  // Incoming arcs
    
    friend class graphe;

public:
    noeud(
        const std::string& node_id = "",
        double lat = 0.0,
        double lon = 0.0,
        int streets = 0,
        const std::vector<arc*>& outgoing_arcs = {},
        const std::vector<arc*>& incoming_arcs = {}
    );

    ~noeud() {}

    void affiche() const;

};
