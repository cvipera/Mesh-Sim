#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mesh_settings.h"



typedef struct mesh_node mesh_node;


/// @brief int id, float bandwidth, float latency, float length, mesh_node* source, mesh_node* destination
typedef struct mesh_link mesh_link;


typedef struct mesh_path mesh_path; 


typedef struct mesh mesh; // Forward declaration

typedef enum status {
    DISCONNECTED,
    CONNECTED,
    ACTIVE,
    INACTIVE
}status;

typedef enum type {
    ROUTER,
    END_DEVICE
}type;

typedef enum data {
    NODES,
    LINKS,
    PATHS,
    ALL
}data;


/// @brief Structure representing the mesh network. sizeof(mesh) = 160 bytes
struct mesh{
    char name[128];            /// Name of the mesh network
    mesh_node* nodes;          /// Array of nodes in the mesh
    int node_count;            /// Number of nodes in the mesh
    int link_count;            /// Number of links in the mesh
    mesh_path* paths;          /// Array of paths in the mesh
    int path_count;            /// Number of paths in the mesh
};


///@brief Structure representing a node in the mesh network. sizeof(mesh_node) = 48 bytes
struct mesh_node{
    int id;                       /// Unique identifier for the mesh node
    int input_link_count;         /// Number of input links connected to this node
    int output_link_count;        /// Number of output links connected to this node
    float x, y;                   /// Coordinates of the node in 2D space
    status node_status;           /// Status of the node (e.g., ACTIVE, INACTIVE)
    type node_type;               /// Type of the node (e.g., ROUTER, END_DEVICE)
    mesh_link* output_link;       /// Output link connected to this node
    mesh_link** input_links;      /// Array of input links (up to 8)
};

/// @brief Structure representing a link in the mesh network. sizeof(mesh_link) = 32 bytes
struct mesh_link{
    int id;                      /// Unique identifier for the mesh link
    float bandwidth;             /// Bandwidth of the link in Mbps
    float latency;               /// Latency of the link in ms
    float length;                /// Length of the link in meters
    mesh_node* source;           /// Source node of the link
    mesh_node* destination;      /// Destination node of the link
};


/// @brief Structure representing a path in the mesh network. sizeof(mesh_path) = 24 bytes
struct mesh_path{               /// Name of the path
    int id;                     /// Unique identifier for the mesh path
    int start_node_id;          /// ID of the starting node
    int end_node_id;            /// ID of the ending node
    int length;                 /// Length of the path
    mesh_node* nodes;           /// Array of nodes in the path
};

///@brief Initializes a mesh network with the given name.
///@param name The name of the mesh network.
///@return mesh* A pointer to the initialized mesh structure.
mesh* init_mesh(const char* name);

///@brief Frees the resources allocated for the mesh network.
///@param m A double pointer to the mesh structure to be freed.
void free_mesh(mesh* m);

/// @brief Initialize a mesh node with the given id.
/// @param m the adresse of the node to initialize
/// @param id the id of the node
/// @return A pointer to the initialized mesh_node structure
mesh_node* init_mesh_node(mesh_node* m,int id);



/// @brief Check if a link can be created between two nodes based on distance and existing links.
/// @param source The source mesh node.
/// @param destination The destination mesh node.
/// @return True if a link can be created, false otherwise.
bool mesh_link_allowed(mesh_node* source, mesh_node* destination);



/// @brief create a link between two nodes
/// @param m the adresse of the link to initialize
/// @param id the id of the link
/// @param source the source node of the link
/// @param destination the destination node of the link
/// @return the number of output links of the source node after initialization
int init_mesh_link(int id, mesh_node* source, mesh_node* destination);



mesh_path* init_mesh_path(mesh_path* m, int id, int start_id, int end_id);

///@brief Computes the mesh network based on the provided nodes.
///@param nodes An array of mesh_node structures representing the mesh nodes.
///@param node_count The number of nodes in the array.
///@return int A status code indicating success or failure.
int compute_mesh(mesh_node* nodes, int node_count);


///@brief list all nodes within a certain range from a given node id.
///@param nodes An array of mesh_node structures representing the mesh nodes.
///@param node_count The number of nodes in the array.
///@param start_id The ID of the starting node.
///@param range The distance range to search for nodes.
///@return mesh_node* A pointer to an array of mesh_node structures found within the specified range
mesh_node* get_node_in_range(mesh_node* nodes, int node_count, int start_id, float range);


///@brief Count all nodes within a certain range from a given node id.
///@param nodes An array of mesh_node structures representing the mesh nodes.
///@param node_count The number of nodes in the array.
///@param start_id The ID of the starting node.
///@param range The distance range to search for nodes.
///@return int The count of nodes found within the specified range.
int count_nodes_within_range(mesh_node* nodes, int node_count, int start_id, float range);


///@brief Check if a target node is in the same network as the start node.
///@param nodes An array of mesh_node structures representing the mesh nodes.
///@param node_count The number of nodes in the array.
///@param start_id The ID of the starting node.
///@param target_id The ID of the target node.
///@return bool True if the target node is reachable from the start node, false otherwise.
bool is_node_reachable(mesh_node* nodes, int node_count, int start_id, int target_id);


///@brief Calculate the path length between two nodes in the mesh network.
///@param nodes An array of mesh_node structures representing the mesh nodes.
///@param node_count The number of nodes in the array.
///@param start_id The ID of the starting node.
///@param target_id The ID of the target node.
///@return int The length of the path between the two nodes.
int path_length_between_nodes(mesh_node* nodes, int node_count, int start_id, int target_id);   


///@brief Find the maximum length path from a given start node id.
///@param nodes An array of mesh_node structures representing the mesh nodes.
///@param node_count The number of nodes in the array.
///@param start_id The ID of the starting node.
///@return int The maximum length path from the start node.
int max_length_path_rel(mesh_node* nodes, int node_count, int start_id);


///@brief Find the maximum length path in the entire mesh network.
///@param nodes An array of mesh_node structures representing the mesh nodes.
///@param node_count The number of nodes in the array.
///@return int The maximum length path in the entire mesh network.
int max_length_path_abs(mesh_node* nodes, int node_count);




///@brief Save or dump the mesh network data to the mesh_data.csv file.
///@param m A pointer to the mesh structure representing the mesh network.
///@param data An enum indicating wich data to save.
///@return int A status code indicating of element save or -1 failure.
int MESH_SAVEDUMP(mesh* m, enum data d);



void mesh_debug_print_node(mesh_node* node);
void mesh_debug_print_link(mesh_link* link);
void mesh_debug_print_path(mesh_path* path);
void mesh_debug_print_mesh(mesh* m);