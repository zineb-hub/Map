#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include "noeud.h"

using namespace std;

class graphe {
private:
    unordered_map<string, noeud> lesnoeuds;
    unordered_map<string, arc> lesarcs;
    int nombredenoeuds;
    int nombredarcs;

public:
    graphe(const string& fichierarcs, const string& fichiernoeuds);
    ~graphe() {}
    int lecture_arcs(const string& nomfichier);
    int lecture_noeuds(const string& nomfichier);
    void liste_incidence(const std::string& source, const std::string& destination, arc& arc_obj);
    void affiche_premiers_arcs() const;
    void affiche_premiers_noeuds() const;

    std::string extract_name(const std::string& attributes);
    double extract_maxspeed(const std::string& attributes);

    void displayNeighbors();

    // The part u need to correct and test

    void degre(int n);
    void quickSort(std::vector<std::pair<std::string, int>>& arr, int low, int high) ;
    int partition(std::vector<std::pair<std::string, int>>& arr, int low, int high);
    int chemin(const std::string& o, const std::string& d);  // DFS path method
    int pluscourtchemin(const std::string& o, const std::string& d);  // BFS shortest path method
    double itineraire(const std::string& origine, const std::string& destination);  // Shortest itinerary by street names

};