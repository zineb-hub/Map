#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>

using namespace std;

class arc {
public:
    string id;
    string o;  // Origin node ID
    string d;  // Destination node ID

    // Additional attributes
    bool oneway;
    bool reversed;
    double length;
    double speed_kph;
    double travel_time;
    string name;
    string highway;

    arc() {}
    arc(const string& arc_id) : id(arc_id) {}
    ~arc() {}

    void affiche() {
        cout << "Arc ID: " << id << ", From: " << o << ", To: " << d << ", Name: " << name << endl;
    }
};

class noeud {
public:
    string id;
    double latitude;
    double longitude;
    int street_count;
    bool visite;  // For traversal algorithms

    vector<arc*> arcs_sortants;  // Outgoing arcs
    vector<arc*> arcs_entrants;  // Incoming arcs

    noeud() : visite(false) {}
    noeud(const string& node_id) : id(node_id), visite(false) {}
    ~noeud() {}

    void affiche() {
        cout << "Node ID: " << id << ", Latitude: " << latitude << ", Longitude: " << longitude << endl;
    }

    int degree() const {
        return arcs_sortants.size() + arcs_entrants.size();
    }
};

class graphe {
public:
    unordered_map<string, noeud> lesnoeuds;
    unordered_map<string, arc> lesarcs;
    int nombredenoeuds;
    int nombredarcs;

    graphe(const string& fichierarcs, const string& fichiernoeuds);
    ~graphe() {}

    void affiche();
    int lecture_arcs(const string& nomfichier);
    int lecture_noeuds(const string& nomfichier);
    void liste_incidence();
    void reset_visites();
    void degre(int n);
    int chemin(const string& o, const string& d);
    int pluscourtchemin(const string& o, const string& d);
    int itineraire(const string& origine, const string& destination);
};

