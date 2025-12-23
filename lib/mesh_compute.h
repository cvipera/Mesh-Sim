#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mesh_settings.h"




// --------- FORWARD DECLARATION -----------------------

typedef struct mesh_node mesh_node;

typedef struct mesh_link mesh_link;

typedef struct mesh_path mesh_path; 

typedef struct mesh mesh;

typedef struct mesh_network mesh_network;

typedef struct graph_node graph_node;

typedef struct graph graph;






// ------------------- ENUM --------------------

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





// ------------------- STRUCT -----------------------

/// @brief Structure representing the mesh network. sizeof(mesh) = 160 bytes
struct mesh{
    char name[128];            /// Name of the mesh network
    mesh_node* nodes;          /// Array of nodes in the mesh
    int node_count;            /// Number of nodes in the mesh
    int link_count;            /// Number of links in the mesh
    mesh_network** mnets;      /// Array of mesh networks
    int mnet_count;            /// Number of mesh networks
    int mnet_allocated;        /// Size of the allocated pointer array
    int created_networks;      /// Number of created networks
    mesh_path* paths;          /// Array of paths in the mesh
    int path_count;            /// Number of paths in the mesh
    int simulation_step;       /// Current simulation step
};

///@brief Structure representing a node in the mesh network. sizeof(mesh_node) = 48 bytes
struct mesh_node{
    int id;                       /// Unique identifier for the mesh node
    int network_id;               /// Identifier for the network the node belongs to
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

/// @brief Structure representing a network ( a group of nodes connected together)
struct mesh_network{
    int id;
    int node_count;
    int allocated_space;
    int needed_space;
    int color_r;
    int color_g;
    int color_b;
    mesh_node** nodes;

};

/// @brief Structure representing a path in the mesh network. sizeof(mesh_path) = 24 bytes
struct mesh_path{               /// Name of the path
    int id;                     /// Unique identifier for the mesh path
    int start_node_id;          /// ID of the starting node
    int end_node_id;            /// ID of the ending node
    int length;                 /// Length of the path
    mesh_node* nodes;           /// Array of nodes in the path
};

struct graph_node{
    int in_graph_id;
    mesh_node* mnode;
    int link_count;
    mesh_link** links;
};









// ------------ Mesh Functions ------------

///@brief Initializes a mesh network with the given name.
///@param name The name of the mesh network.
///@return mesh* A pointer to the initialized mesh structure.
mesh* init_mesh(const char* name);

///@brief Frees the resources allocated for the mesh network.
///@param m A double pointer to the mesh structure to be freed.
void free_mesh(mesh* m);



// ------------ Mesh Node Functions ------------

/// @brief Initialize a mesh node with the given id.
/// @param m the adresse of the node to initialize
/// @param id the id of the node
/// @return A pointer to the initialized mesh_node structure
mesh_node* init_mesh_node(mesh_node* m,int id);

/// @brief Check if a target node is within a certain range from a given node.
/// @param a The first mesh node.
/// @param b The second mesh node.
/// @param range The distance range to check.
/// @return True if the second node is within the specified range of the first node, false otherwise.
bool mesh_node_in_range(mesh_node* a, mesh_node* b, float range);

/// @brief List all nodes within a certain range from a given node id.
/// @param nodes An array of mesh_node structures representing the mesh nodes.
/// @param node_count The number of nodes in the array.
/// @param start_id The ID of the starting node.
/// @param range The distance range to search for nodes.
/// @param out_count Pointer to an integer to store the count of nodes found within the specified range can be NULL without error.
/// @return mesh_node* A pointer to an array of mesh_node structures found within the specified range.
mesh_node* get_node_in_range(mesh_node* nodes, int node_count, int start_id, float range,int* out_count);

/// @brief Retrieve a node by its ID from an array of nodes.
/// @param nodes An array of mesh_node structures representing the mesh nodes.
/// @param node_count The number of nodes in the array.
/// @param id The ID of the node to retrieve.
/// @return A pointer to the mesh_node with the specified ID, or NULL if not found.
mesh_node* get_node_by_id(mesh_node* nodes, int node_count, int id);



// ------------ Mesh Link Functions ------------

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
int init_mesh_link(mesh_node* source, mesh_node* destination, mesh* m);

/// @brief Check if a link can be removed between two nodes.
/// @param source The source mesh node.
/// @param destination The destination mesh node.
/// @return True if the link can be removed, false otherwise.
bool mesh_link_removable(mesh_node* source, mesh_node* destination);

/// @brief Remove a link between two nodes.
/// @param m the mesh containing the nodes.
/// @param source The source mesh node.
/// @param destination The destination mesh node.
/// @return The number of output links of the source node after removal
int remove_mesh_link(mesh* m, mesh_node* source, mesh_node* destination);



// ------------ Mesh Path Functions ------------

///@brief Initializes a mesh path with the given parameters.
///@param m A pointer to the mesh_path structure to be initialized.
///@param id The unique identifier for the mesh path.
///@param start_id The ID of the starting node.
///@param end_id The ID of the ending node.
///@return
mesh_path* init_mesh_path(mesh_path* m, int id, int start_id, int end_id);






// ------------ Mesh Network Functions ------------

///@brief Initializes a mesh network structure.
///@param fnode A poitnter to the first node to add to the network
///@param id The unique identifier for the mesh network.
///@return A pointer to the initialized mesh_network structure.
mesh_network* init_mesh_network(mesh_node* fnode, mesh* mesh);

/// @brief Get the index of a mesh network by its ID.
/// @param m The mesh containing the networks.
/// @param id The ID of the mesh network to find.
/// @return The index of the mesh network with the specified ID, or -1 if not found.
int get_mesh_network_index_by_id(mesh* m, int id);


/// @brief  Add a node to the mesh network.
/// @param network A pointer to the mesh_network structure.
/// @param node A pointer to the mesh_node to be added.
/// @return A pointer to the updated mesh_network structure.
mesh_network* add_node_to_mesh_network(mesh_network* network, mesh_node* node);

/// @brief Merge two mesh networks into one.
/// @param net1 The first mesh network.
/// @param net2 The second mesh network.
/// @param mesh A pointer to the mesh containing the nodes.
/// @return A pointer to the merged mesh network.
mesh_network* merge_meshs_networks(mesh_network* net1, mesh_network* net2, mesh* mesh);

///@brief Check if a target node is in the same network as the start node.
///@param mesh An array of mesh_node structures representing the mesh nodes.
///@param start_id The ID of the starting node.
///@param target_id The ID of the target node.
///@return bool True if the target node is reachable from the start node, false otherwise.
bool are_in_same_mesh_network(mesh* mesh, int start_id, int target_id);

/// @brief return a list of pointer to all the nodes in this network
/// @param m A pointer to the mesh
/// @param network_id An int that id a network
/// @param nodes_in_ment_count a pointer to an int, set to the number of nodes in the mnet at the end of the func.
/// @return NULL if invalid network, or a pointer to a list of pointer to node
mesh_node** get_nodes_in_mesh_network(mesh*m, int network_id, int* nodes_in_ment_count);







// ------------ Mesh Save and Debug Functions ------------

///@brief Save or dump the mesh network data to the mesh_data.csv file.
///@param m A pointer to the mesh structure representing the mesh network.
///@param data An enum indicating wich data to save.
///@return int A status code indicating of element save or -1 failure.
int MESH_SAVEDUMP(mesh* m, enum data d);
void mesh_debug_print_node(mesh_node* node);
void mesh_debug_print_link(mesh_link* link);
void mesh_debug_print_path(mesh_path* path);
void mesh_debug_print_mesh(mesh* m);
void mesh_debug_print_node_in_range(mesh_node* nodes, int node_count);
void mesh_debug_print_mesh_data(mesh* m);
void mesh_debug_print_network(mesh_network* network);
void mesh_debug_print_networks(mesh* m);
void mesh_debug_print_network_stats(mesh* m, mesh_network** network);



// ------------------------ TODO  --------------------------------

// TODO FUNCTIONS

///@brief Computes the mesh network based on the provided nodes.
///@param nodes An array of mesh_node structures representing the mesh nodes.
///@param node_count The number of nodes in the array.
///@return int A status code indicating success or failure.
int compute_mesh(mesh_node* nodes);


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
