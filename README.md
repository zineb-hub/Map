# 🚀 Advanced Algorithms and Graphs (AMS) - Road Network Analysis Project 🏙️

**📚 Course:** Advanced Algorithms and Graphs - CERI - L2 Informatics  
**👨‍🏫 Instructor:** S. Gueye  
**📅 Semester:** 3  
**👥 Team Members:**  
- **MEFTAH Zineb**  
- **MEDJBER Syphax**  

---

## 📌 Project Overview

This project involves developing a **graph-based model** to represent and analyze real-world road networks in **Avignon and its surroundings**. The dataset consists of **two CSV files** containing geographical and routing data:

- `nodes.csv`: Represents nodes (intersections or specific locations).
- `arcs.csv`: Represents roads connecting these nodes.

The goal is to **store, process, and implement various algorithms** to extract meaningful insights, such as **shortest path calculations, connectivity checks, and node degree analysis**.

---

## 📂 Dataset Description

### 📍 1. Nodes (`nodes.csv`)
Each node corresponds to a specific geographic location with the following attributes:

| Attribute      | Description |
|--------------|------------|
| `id`         | Unique identifier for the node |
| `latitude`   | Latitude coordinate |
| `longitude`  | Longitude coordinate |
| `street_count` | Number of roads connected to this node |

### 🛣️ 2. Roads (`arcs.csv`)
Each row represents a **road segment** connecting two nodes and includes the following details:

| Attribute      | Description |
|--------------|------------|
| `osmid`      | Unique road identifier |
| `oneway`     | Boolean indicating if the road is one-way |
| `lanes`      | Number of lanes |
| `name`       | Name of the road (if available) |
| `highway`    | Type of road (highway, residential, etc.) |
| `reversed`   | Indicates the direction of one-way roads |
| `length`     | Road length in meters |
| `geometry`   | List of points defining the road's shape |
| `maxspeed`   | Maximum speed limit (if available) |
| `speed_kph`  | Estimated average speed (if `maxspeed` is missing) |
| `travel_time`| Estimated travel time based on speed |

🛑 **Important Note:** Not all attributes are always filled, and for missing values, default placeholders will be used.

---

## 🎯 Project Objectives

1. **Store and structure** the data using efficient data structures.
2. **Implement graph algorithms** to analyze the network.
3. **Optimize data retrieval** for efficient querying.
4. **Develop methods** to process and visualize graph properties.
5. **Implement shortest path and connectivity algorithms** to find optimal routes.

---

## 🏗️ Data Structures

### 1️⃣ Graph Representation
A graph is defined as a **set of nodes (V) and edges (E)**. We implement it using the **C++ Standard Template Library (STL)** for efficiency.

### 2️⃣ Node Class (`noeud`)
Represents a node in the graph.

```cpp
class noeud {
    string id;
    double latitude, longitude;
    int street_count;
    vector<string> outgoing_arcs;
    vector<string> incoming_arcs;
public:
    noeud(string id, double lat, double lon, int sc);
    ~noeud();
    void affiche();
};
