#include <iostream>
#include <string>
#include <vector>


using namespace std;

struct Point {
    long double x;  // Longitude
    long double y;  // Latitude
};

class arc
{
protected:
    string id;
    string o; // Origin node ID
    string d; // Destination node ID
    string osmid;
    double length;
    double maxspeed;
    double speed_kph;
    double travel_time;
    string name;
    string highway;
    int lanes;
    std::vector<Point> geometry;

    friend class noeud;
    friend class graphe;
public:
arc() : o(""), d(""), osmid(""), length(0.0), maxspeed(0.0), speed_kph(0.0), name(""), highway(""), lanes(0), geometry(), travel_time(0.0) {}
    // Constructor
    arc(
        const std::string& origin,
        const std::string& destination,
        string osmid_id,
        const std::string& name = "",
        const std::string& highway = "",
        double length = 0.0,
        double maxspeed = 0.0,
        double speed_kph = 0.0,
        int lanes = 0,
        const std::vector<Point>& geometry = {}, 
        double time = 0.0
    );
    void affiche() const;
    ~arc();
};
