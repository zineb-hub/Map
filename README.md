# Advanced Algorithms and Graphs (AMS)  

---

## Table of Contents

1. [Data Description](#data-description)
   - [1.1 Nodes](#11-nodes)
   - [1.2 Roads](#12-roads)
2. [Objectives](#objectives)
3. [Data Structures](#data-structures)
   - [3.1 Task 1: Arc Implementation](#31-task-1-arc-implementation)
   - [3.2 Task 2: Node Implementation](#32-task-2-node-implementation)
   - [3.3 Task 3: Graph Implementation](#33-task-3-graph-implementation)
4. [Graph Traversal Algorithms](#graph-traversal-algorithms)
   - [4.1 Task 4: Degree Calculation](#41-task-4-degree-calculation)
   - [4.2 Task 5: Path Existence and Display](#42-task-5-path-existence-and-display)
   - [4.3 Task 6: Shortest Path (in Arcs)](#43-task-6-shortest-path)
   - [4.4 Task 7: Street Itinerary by Road Name](#44-task-7-street-itinerary)
5. [Evaluation Modalities](#evaluation-modalities)
6. [Conclusion](#conclusion)
7. [Acknowledgments](#acknowledgments)

---

## Data Description

### 1.1 Nodes  
The file **nodes.csv** contains a set of nodes representing precise geographical locations in the real road network of Avignon and its surroundings. For each node, only the following attributes are considered:
- **id:** Unique identifier.
- **latitude (y)**
- **longitude (x)**
- **street count:** Number of roads passing through the node.

*Other attributes exist but are ignored for this activity.*

### 1.2 Roads  
Each line in the **arcs.csv** file represents a road segment (arc) between two nodes. The following attributes are associated with each arc:
- **osmid:** An identifier for the arc.
- **oneway:** Boolean indicating if the road is one-way.
- **lanes:** Number of lanes.
- **name:** Road name.
- **highway:** Type of road (e.g., motorway, residential, primary, secondary, etc.).
- **reversed:** When a road is one-way (oneway = true), this indicates its direction (if `reversed = false`, the direction is from left to right; otherwise, from right to left). For two-way roads, this attribute is ignored.
- **length:** Road length in meters.
- **geometry:** Sequence of points constituting the road.
- **maxspeed:** Maximum speed on the road (in km/h).
- **speed kph:** If maxspeed is not provided, this value is an average of allowed maximum speeds for that road type or across all arcs.
- **travel time:** Estimated travel time on the road when traveling at the maximum allowed speed (or at the average speed if maxspeed is not available).

*Note:* Not all attributes are provided for every node or arc. A one-way road (a, b) corresponds to a single arc from the origin node to the destination node, whereas a two-way road (oneway = false) corresponds to two arcs (a, b) and (b, a).

---

## Objectives

The goal of this AMS (Advanced Algorithms and Graphs) activity is to store the data from the CSV files into appropriate data structures and to develop algorithms that exploit this data. The provided guidelines offer a partial architecture for these structures—your task is to complete and justify your design choices.

---

## Data Structures

### 3.1 Task 1: Arc Implementation  
Define a graph as a pair of sets (V, E) where V is the set of nodes and E is the set of arcs.  
Implement an **arc** class (in C++ for example) with attributes such as:
- `string id` – the name of the road (if absent, generate one based on the node pair, and append numbers if duplicate names occur).
- `string o` – origin node.
- `string d` – destination node.
- Other arc attributes.

Choose a data structure (e.g., linked list, binary search tree, or hash table) for E that allows fast retrieval by arc id.

### 3.2 Task 2: Node Implementation  
Define a **node** class with attributes:
- `string id`
- Other attributes (e.g., latitude, longitude, street count)
- Two collections: one for outgoing arcs and one for incoming arcs.

Choose an appropriate data structure (e.g., linked list, binary search tree, or hash table) for the node set V, ensuring rapid lookup by node id.

### 3.3 Task 3: Graph Implementation  
Implement a **graph** class that contains:
- **V lesnoeuds**: the set of nodes.
- **E lesarcs**: the set of arcs.
- Integers for the number of nodes and arcs.

Include methods such as:
- `int lecture_arcs(filename)`: Reads the arcs.csv file line by line and inserts arcs into E. Returns the number of arcs.
- `int lecture_noeuds(filename)`: Reads the nodes.csv file line by line and inserts nodes into V. Returns the number of nodes.
- `void liste_incidence()`: Updates each node's incoming and outgoing arc lists by traversing E.

The graph constructor should build V and E by calling these methods.

---

## Graph Traversal Algorithms

### 4.1 Task 4: Degree Calculation  
Implement a method in the graph class:
- `void degre(n)`: Display the top n nodes with the highest degree (where 1 ≤ n ≤ total number of nodes). Justify the algorithm’s complexity.

### 4.2 Task 5: Path Existence and Display  
Implement a method:
- `int chemin(string o, string d)`: Determines if a path exists from the node with id *o* to the node with id *d* using a depth-first search (DFS).  
  - Returns the number of arcs in the path if one exists, or 0 otherwise.
  - Also displays the names of the roads used in the path.
  
*(Note: This requires adding a "visited" attribute to the node class.)*

### 4.3 Task 6: Shortest Path (in Arcs)  
Implement a method:
- `int pluscourtchemin(string o, string d)`: Finds the shortest path (in terms of number of arcs) from node *o* to node *d* using an alternative data structure (e.g., a queue for breadth-first search).  
  - Returns the number of arcs in the shortest path, or 0 if no path exists.
  - Also displays the names of the roads used.

### 4.4 Task 7: Street Itinerary by Road Name  
Implement a method:
- `int itineraire(string origine, string destination)`: Provides the shortest route (in number of streets) from the street with name *origine* to the street with name *destination*, for users who are not familiar with node ids.

---

## Evaluation Modalities

- **Separate Grade:** The AMS is graded separately from the regular lab work.
- **Group Work:** Maximum group size is 2.
- **Oral Evaluation:** After code submission, oral questions may be asked to either member of the group.
- **Individual Mastery:** Each member must master the entire content, as questions may be directed to any participant.
- **Commenting:** All code must be well-commented with justification for design choices.

---

## Conclusion

This specification document provides a clear and detailed vision for the **Advanced Algorithms and Graphs** project. By meeting these functional, technical, and organizational requirements, the project will result in a robust, efficient, and scalable search engine system using advanced graph algorithms.

---

## Acknowledgments

We extend our sincere thanks to everyone who contributed to this project and to all the stakeholders for their support throughout the development process.
