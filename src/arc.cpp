#include "arc.h"
#include <iostream>
#include <sstream>

static std::unordered_map<std::string, int> name_counts; // Static variable for tracking duplicate names
static std::unordered_map<std::string, double> highway_speed_sum;  // Total speed_kph per highway type
static std::unordered_map<std::string, int> highway_count;  // Count of arcs per highway type
static double global_speed_sum = 0.0;         // Total speed_kph for all arcs
static int global_arc_count = 0; // Count of all arcs

arc::arc(
    const std::string& origin,
    const std::string& destination,
    string osmid_id,
    const std::string& arc_name,
    const std::string& arc_highway,
    double arc_length,
    double arc_maxspeed,
    double arc_speed_kph,
    int arc_lanes,
    const std::vector<Point>& arc_geometry, 
    double time
) :
    o(origin),
    d(destination),
    osmid(osmid_id),
    length(arc_length),
    maxspeed(arc_maxspeed),
    speed_kph(arc_speed_kph),
    name(arc_name.empty() ? (origin + destination) : arc_name),
    highway(arc_highway),
    lanes(arc_lanes),
    geometry(arc_geometry), 
    travel_time(time)
{
    if (speed_kph == 0.0){
        // Handle speed_kph
        if (!highway.empty() && highway_speed_sum.find(highway) != highway_speed_sum.end()) {
            // Use the average speed for this highway type
            speed_kph = highway_speed_sum[highway] / highway_count[highway];
        } else  {
            // Use the global average speed
            speed_kph = (global_arc_count > 0) ? global_speed_sum / global_arc_count : 50.0; // Default 50 kph
        }
    }

    // Update global and highway-specific averages
    global_speed_sum += speed_kph;
    global_arc_count++;

    if (!highway.empty()) {
        highway_speed_sum[highway] += speed_kph;
        highway_count[highway]++;
    }
    
    // Handle travel_time
    if (time) {
        travel_time = time;  // Travel time in seconds
    } else {
        travel_time = 0.0; // Default value for missing data
    }

    // Handle naming, si repétition on les numerote 
    if (name_counts.find(name) != name_counts.end()) {
        name_counts[name]++;
        id = name + std::to_string(name_counts[name]);
    } else {
        name_counts[name] = 1;
        id = name;
    }
}



void arc::affiche() const {
    std::cout << "Arc Details:\n";
    std::cout << "----------------------------------------\n";
    std::cout << "ID           : " << id << "\n";
    std::cout << "Origin       : " << o << "\n";
    std::cout << "Destination  : " << d << "\n";
    std::cout << "Length       : " << length << " meters\n";
    std::cout << "Max Speed    : " << (maxspeed > 0 ? std::to_string(maxspeed) + " km/h" : "Not provided") << "\n";
    std::cout << "Speed (kph)  : " << speed_kph << " km/h\n";
    std::cout << "Travel Time  : " << (travel_time > 0 ? std::to_string(travel_time) + " seconds" : "Not calculated") << "\n";
    std::cout << "Name         : " << (!name.empty() ? name : "Unnamed") << "\n";
    std::cout << "Highway Type : " << (!highway.empty() ? highway : "Unknown") << "\n";
    std::cout << "Lanes        : " << (lanes > 0 ? std::to_string(lanes) : "Not specified") << "\n";

    std::cout << "Geometry     : ";
    if (!geometry.empty()) {
        for (const auto& point : geometry) {
            std::cout << "(" << std::fixed << "      " << point.x << ", " << point.y << ") ";
        }
        std::cout << "\n";
    } else {
        std::cout << "No geometry data provided.\n";
    }
    std::cout << "----------------------------------------\n";
}

arc::~arc() {}
