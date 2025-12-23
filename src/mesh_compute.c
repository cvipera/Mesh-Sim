#include "../lib/mesh_compute.h"
#include "../lib/utils.h"
#include <assert.h>



// ------------ Mesh Functions ------------

mesh* init_mesh(const char* name) {
    mesh* m = (mesh*)malloc(sizeof(mesh));
    strncpy(m->name, name, sizeof(m->name) - 1);
    m->name[sizeof(m->name) - 1] = '\0';
    m->node_count = MESH_NODES_COUNT;
    m->nodes = (mesh_node*)malloc(sizeof(mesh_node) * m->node_count);
    m->link_count = 0;
    m->path_count = 0;
    m->paths = NULL;
    for (int i = 0; i < m->node_count; i++) {
        init_mesh_node(&m->nodes[i], i);
    }

    return m;
} 

void free_mesh(mesh* m) {
    if (m->nodes) {
        free(m->nodes);
        m->nodes = NULL;
    }
    if (m->paths) {
        free(m->paths);
        m->paths = NULL;
    }
    m->node_count = 0;
    m->link_count = 0;
    m->path_count = 0;
}





// ------------ Mesh Node Functions ------------

mesh_node* init_mesh_node (mesh_node* node, int id) {
    node->id = id;
    node->input_link_count = 0;
    node->output_link_count = 0;
    node->network_id = -1; // Not part of any network initially
    node->node_status = DISCONNECTED; // Initially disconnected
    node->node_type = MESH_ROUTERS_COUNT > id ? ROUTER : END_DEVICE; // First N nodes are routers
    node->x = random() % (int)MESH_SIZE_X;
    node->y = random() % (int)MESH_SIZE_Y;
    node->output_link = calloc(2, sizeof(mesh_link)); // No output links initially
    node->input_links = calloc(8, sizeof(mesh_link*)); // No input links initially
    return node;
}

bool mesh_node_in_range(mesh_node* a, mesh_node* b, float range) {
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    if (dx > range || dy > range || dx < -range || dy < -range) {
        return false; // Quick rejection
    }
    float distance = sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
    return distance <= range;
}

mesh_node* get_node_in_range(mesh_node* nodes, int node_count, int start_id, float range,int* out_count) {
    // count nodes in range
    if (start_id < 0 || start_id >= node_count) {
        return NULL; // Invalid start_id
    }
    
    int count = 0;
    for (int i = 0; i < node_count; i++) {
        if (nodes[i].id != start_id && mesh_node_in_range(&nodes[start_id], &nodes[i], range)) {
            count++;   
            // set the bit i of the bit mask
            bit_set(bitmask, i);
        }else {
            // clear the bit i of the bit mask
            bit_clear(bitmask, i);
        }
    }
    if (count == 0) {
        return NULL; // No nodes in range
    }
    // allocate array for nodes in range
    mesh_node* in_range_nodes = (mesh_node*)malloc(sizeof(mesh_node) * count);
    int index = 0;
    for (int i = 0; i < node_count; i++) {
        if (bit_test(bitmask, i)) {
            in_range_nodes[index++] = nodes[i];
        }
    }
    if (out_count) {
        *out_count = count;
    }
    return in_range_nodes;
}

mesh_node* get_node_by_id(mesh_node* nodes, int node_count, int id) {
    for (int i = 0; i < node_count; i++) {
        if (nodes[i].id == id) {
            return &nodes[i];
        }
    }
    return NULL; // Not found
}



// ------------ Mesh Link Functions ------------

bool mesh_link_allowed(mesh_node* source, mesh_node* destination) {
    if(destination->node_type == END_DEVICE) {
        return false; // Cannot link to end devices
    }
    if(source->node_type == END_DEVICE && source->output_link_count >= 1) {
        return false; // End devices can have only one output link
    }
    if(source->output_link_count >= 2) {
        return false; // Source has max output links
    }
    if(destination->input_link_count >= 8) {
        return false; // Destination has max input links
    }
    if (source->id == destination->id) {
        return false; // No self-links
    }
    float distance = sqrt(pow(source->x - destination->x, 2) + pow(source->y - destination->y, 2));
    if(distance > MESH_MAX_ROUTER_DISTANCE) {
        return false; // Exceeds max link distance
    }
    if (source->node_type == END_DEVICE) {
        if (distance > MESH_MAX_LINK_DISTANCE) {
            return false; // Exceeds max link distance for end devices
        }
    }


    // Check if link already exists in this direction
    for (int i = 0; i < source->output_link_count; i++) {
        if (source->output_link[i].destination->id == destination->id) {
            return false; // Link already exists
        }
    }
    // check if link already exists in the opposite direction
    for (int i = 0; i < destination->output_link_count; i++) {
        if (destination->output_link[i].destination->id == source->id) {
            return false; // Link already exists in opposite direction
        }
    }

    return true;
}

int init_mesh_link(mesh_node* source, mesh_node* destination, mesh* m) {
    // init of the link
    mesh_link link = source->output_link[source->output_link_count];
    link.id = source->id*1000+destination->id*10+source->output_link_count; // Unique ID based on source and destination IDs
    link.bandwidth = MESH_DEFAULT_BANDWIDTH; // Decrease bandwidth with length
    link.length = sqrt(pow(source->x - destination->x, 2) + pow(source->y - destination->y, 2));
    link.latency = ( link.length * 10);   
    link.source = source;
    link.destination = destination;


    // add to source output links
    source->output_link[source->output_link_count++] = link;
    source->node_status = CONNECTED;


    // add to destination input links
    destination->input_links[destination->input_link_count++] = &source->output_link[source->output_link_count - 1];
    destination->node_status = CONNECTED;


    // --------------- START NETWORK MANAGEMENT ---------------
    int source_network_id = source->network_id;
    int dest_network_id = destination->network_id;
    int source_network_index = get_mesh_network_index_by_id(m, source_network_id);
    int dest_network_index = get_mesh_network_index_by_id(m, dest_network_id);
    mesh_network* source_network = NULL;
    mesh_network* dest_network = NULL;
    if (source_network_id != -1) {
        source_network = m->mnets[source_network_index];
    }
    if (dest_network_id != -1) {
        dest_network = m->mnets[dest_network_index];
    }
    if (source_network_id == 211 || dest_network_id == 211) {
        //printf("Debug: Source network index: %d, Dest network index: %d\n", source_network_index, dest_network_index);
        //printf("Source node %d network ID: %d\n", source->id, source_network_id);
        //printf("Destination node %d network ID: %d\n", destination->id, dest_network_id);
    }
    if (source_network_id == -1 && dest_network_id == -1) {
        // both nodes are not in a network, create a new network
        mesh_network* new_network = init_mesh_network(source, m);
        add_node_to_mesh_network(new_network, destination);
    } else if (source_network_id != -1 && dest_network_id == -1) {
        // only source is in a network, add destination to source's network
        add_node_to_mesh_network(source_network, destination);
    } else if (source_network_id == -1 && dest_network_id != -1) {
        // only destination is in a network, add source to destination's network
        add_node_to_mesh_network(dest_network, source);
    } else if (source_network_id != dest_network_id) {
        // both nodes are in different networks, merge the networks
        merge_meshs_networks(source_network, dest_network, m);
    }



    // --------------- END NETWORK MANAGEMENT ---------------


    return source->output_link_count;
}

bool mesh_link_removable(mesh_node* source, mesh_node* destination) {
    for (int i = 0; i < source->output_link_count; i++) {
        if (source->output_link[i].destination->id == destination->id) {
            return true;
        }
    }
    return false;
}

int remove_mesh_link(mesh* m, mesh_node* source, mesh_node* destination) {
    for (int i = 0; i < source->output_link_count; i++) {
        if (source->output_link[i].destination->id == destination->id) {
            // Shift links to remove the link
            for (int j = i; j < source->output_link_count - 1; j++) {
                source->output_link[j] = source->output_link[j + 1];
            }
            source->output_link_count--;
            if (source->output_link_count == 0 && source->input_link_count == 0) {
                source->node_status = DISCONNECTED;
            }
            // Remove from destination's input links
            for (int k = 0; k < destination->input_link_count; k++) {
                if (destination->input_links[k]->source->id == source->id) {
                    for (int l = k; l < destination->input_link_count - 1; l++) {
                        destination->input_links[l] = destination->input_links[l + 1];
                    }
                    destination->input_link_count--;
                    break;
                }
            }
            m->link_count--;
            return source->output_link_count;
        }
    }
    return -1; // Link not found, no removal
}






// ------------ Mesh Path Functions ------------

mesh_path* init_mesh_path(mesh_path* path,int id, int start_node_id, int end_node_id) {
    path->id = id;
    path->start_node_id = -1;
    path->end_node_id = -1;
    path->length = 0;
    path->nodes = NULL;
    return path;
}





// ------------ Mesh Networks Functions ------------

mesh_network* init_mesh_network(mesh_node* fnode, mesh* mesh) {
    if(mesh->mnets == NULL) {
        mesh->mnets = (mesh_network**)malloc(sizeof(mesh_network*));
        mesh->mnet_count = 0;
        mesh->mnet_allocated = 1;
    }else if(mesh->mnet_count+1 >= mesh->mnet_allocated) {
        mesh->mnets = (mesh_network**)realloc(mesh->mnets, sizeof(mesh_network*) * mesh->mnet_allocated*2);
        mesh->mnet_allocated *= 2;
    }
    assert(mesh->mnets != NULL);

    mesh_network* mnet = (mesh_network*)malloc(sizeof(mesh_network));
    mesh->mnets[mesh->mnet_count] = mnet;
    mnet->id = mesh->created_networks++;
    mesh->mnet_count++;
    mnet->node_count = 1;
    mnet->nodes = (mesh_node**)malloc(sizeof(mesh_node*));
    mnet->nodes[0] = fnode;
    fnode->network_id = mnet->id;
    mnet->allocated_space = 1;
    mnet->needed_space = 1;
    mnet->color_r = 55 + random() % 200 ;
    mnet->color_g = 55 + random() % 200 ;
    mnet->color_b = 55 + random() % 200 ;
    return mnet;
}

int get_mesh_network_index_by_id(mesh* m, int id) {
    for (int i = 0; i < m->mnet_count; i++) {
        if (m->mnets[i]->id == id) {
            return i;
        }
    }
    return -1; // Not found
}

mesh_network* add_node_to_mesh_network(mesh_network* mnet, mesh_node* node) {
    // Reallocate if needed
    mnet->needed_space++;
    if (mnet->needed_space+1 >= mnet->allocated_space) {
        mnet->allocated_space *= 2;
        mnet->nodes = (mesh_node**)realloc(mnet->nodes, sizeof(mesh_node*) * mnet->allocated_space);
    }
    assert(mnet->nodes != NULL);
    node->network_id = mnet->id;
    mnet->nodes[mnet->node_count++] = node;
    return mnet;
}

mesh_network* merge_meshs_networks(mesh_network* net1, mesh_network* net2, mesh* mesh) {
    if (net1->node_count < net2->node_count) {
        // swap
        mesh_network* temp = net1;
        net1 = net2;
        net2 = temp;
    }
    
    
    
    
    for (int i = 0; i < net2->node_count; i++) {
        add_node_to_mesh_network(net1, net2->nodes[i]);
        net2->nodes[i]->network_id = net1->id;
    }
    int index = get_mesh_network_index_by_id(mesh, net2->id);
    free(net2->nodes);
    free(net2);

    mesh->mnet_count--;
    for (int i = index ; i < mesh->mnet_count; i++){
        mesh->mnets[i] = mesh->mnets[i+1];
    }

    return net1;
}

bool are_in_same_mesh_network(mesh* m, int start_id, int target_id){
    return (get_node_by_id(m->nodes,m->node_count,start_id)->network_id == get_node_by_id(m->nodes,m->node_count,target_id)->network_id);
}

mesh_node** get_nodes_in_mesh_network(mesh* m, int network_id, int* nodes_in_mnet_count){
    *nodes_in_mnet_count = 0;
    for(int i = 0; i<m->node_count;i++){
        if(m->nodes[i].network_id) {
            (*nodes_in_mnet_count)++;
        }
    }
    int index = 0;
    mesh_node** tab_of_node = (mesh_node**)calloc(*nodes_in_mnet_count,sizeof(mesh_node*));
        for(int i = 0; i<m->node_count;i++){
        if(m->nodes[i].network_id) {
            tab_of_node[index++] = &m->nodes[i];
        }
    }
    return tab_of_node;
}








// --------------------- Graph Function -------------------------

// int** mesh_generate_adjacency_matrix(mesh* m) {
//     int** matrix = (int**)malloc(m->node_count * sizeof(int*));
//     for (int i = 0; i < m->node_count; i++) {
        














// ------------ Mesh Save and Debug Functions ------------

int MESH_SAVEDUMP(mesh* m, enum data d) {
    FILE* file = fopen("mesh_data.csv", "w");
    if (!file) {
        return -1; // Failure to open file
    }

    // print the settings of the mesh
    fprintf(file, "# Mesh Name: %s\n", m->name);
    fprintf(file, "# Node Count: %d\n", m->node_count);
    fprintf(file, "# Link Count: %d\n", m->link_count);
    fprintf(file, "# Path Count: %d\n", m->path_count);
    fprintf(file, "# Settings:\n");
    fprintf(file, "# MESH_NODES_COUNT: %d\n", MESH_NODES_COUNT);
    fprintf(file, "# MESH_ROUTERS_COUNT: %d\n", MESH_ROUTERS_COUNT);
    fprintf(file, "# MESH_MAX_LINKS_PER_NODE: %d\n", MESH_MAX_LINKS_PER_NODE);
    fprintf(file, "# MESH_DEFAULT_BANDWIDTH: %.2f\n", MESH_DEFAULT_BANDWIDTH);
    fprintf(file, "# MESH_DEFAULT_LATENCY: %.2f\n", MESH_DEFAULT_LATENCY);
    fprintf(file, "# MESH_SIZE_X: %.2f\n", MESH_SIZE_X);
    fprintf(file, "# MESH_SIZE_Y: %.2f\n", MESH_SIZE_Y);
    fprintf(file, "# MESH_MAX_LINK_DISTANCE: %.2f\n", MESH_MAX_LINK_DISTANCE);
    fprintf(file, "\n");



    if (d == NODES || d == ALL) {
        fprintf(file, "NodeID,X,Y,Status,Type\n");
        for (int i = 0; i < m->node_count; i++) {
            mesh_node* node = &m->nodes[i];
            fprintf(file, "%d,%.2f,%.2f,%d,%d\n", node->id, node->x, node->y, node->node_status, node->node_type);
        }
    }

    if (d == LINKS || d == ALL) {
        fprintf(file, "LinkID,SourceID,DestinationID,Bandwidth,Latency\n");
        for (int i = 0; i < m->node_count; i++) {
            if (m->nodes[i].output_link_count == 0) continue;
            if (m->nodes[i].output_link_count >= 1) {
                mesh_link* link = &m->nodes[i].output_link[0];
                fprintf(file, "%d,%d,%d,%.2f,%.2f\n", link->id, link->source->id, link->destination->id, link->bandwidth, link->latency);
            }
            if (m->nodes[i].output_link_count == 2) {
                mesh_link* link = &m->nodes[i].output_link[1];
                fprintf(file, "%d,%d,%d,%.2f,%.2f\n", link->id, link->source->id, link->destination->id, link->bandwidth, link->latency);
            }
        }
    }

    if (d == PATHS || d == ALL) {
        fprintf(file, "PathStartID,PathEndID,Length\n");
        for (int i = 0; i < m->path_count; i++) {
            mesh_path* path = &m->paths[i];
            fprintf(file, "%d,%d,%d\n", path->start_node_id, path->end_node_id, path->length);
        }
    }

    fclose(file);
    return 0; // Success
}

void mesh_debug_print_node(mesh_node* node) {
    printf("Node ID: %02d                  ", node->id);
    printf("Coordinates: (%06.2f, %06.2f)    ", node->x, node->y);
    printf("Status: %d     ", node->node_status);
    printf("Type: %d       ", node->node_type);
    printf("Input Links Count: %d        ", node->input_link_count);
    if (node->output_link) {
        printf("Output Link ID: %05d       ", node->output_link->id);
        if (node->output_link_count > 1) {
            printf("Output Link ID 2: %05d     ", node->output_link[1].id);
        }
        printf("\n");
    } else {
        printf("No Output Link\n");
    }
}

void mesh_debug_print_link(mesh_link* link) {
    printf("Link ID: %05d                ", link->id);
    printf("Bandwidth: %08.2f Mbps       ", link->bandwidth);
    printf("Latency: %06.2f ms           ", link->latency);
    printf("Length: %06.2f meters        ", link->length);
    printf("Source Node ID: %02d         ", link->source->id);
    printf("Destination Node ID: %02d    \n", link->destination->id);
}

void mesh_debug_print_path(mesh_path* path) {
    printf("Path Start Node ID: %d   ", path->start_node_id);
    printf("Path End Node ID: %d     ", path->end_node_id);
    printf("Path Length: %d          ", path->length);
    printf("Nodes in Path:       ");
    for (int i = 0; i < path->length; i++) {
        printf("%d    ", path->nodes[i].id);
    }
    printf("\n");
}

void mesh_debug_print_mesh(mesh* m) {
    printf("Mesh Name: %s     ", m->name);
    printf("Node Count: %d    ", m->node_count);
    printf("Link Count: %d    ", m->link_count);
    printf("Path Count: %d    ", m->path_count);
    printf("\nNodes:\n");
    for (int i = 0; i < m->node_count; i++) {
        mesh_debug_print_node(&m->nodes[i]);
    }
    printf("\nLinks:\n");
    for (int i = 0; i < m->node_count; i++) {
        if (m->nodes[i].output_link_count == 0) continue;
        if (m->nodes[i].output_link_count >= 1) {
            mesh_debug_print_link(&m->nodes[i].output_link[0]);
        }
        if (m->nodes[i].output_link_count == 2) {
            mesh_debug_print_link(&m->nodes[i].output_link[1]);
        }
    }
    printf("\nPaths:\n");
    for (int i = 0; i < m->path_count; i++) {
        mesh_debug_print_path(&m->paths[i]);
    }
}

void mesh_debug_print_node_in_range(mesh_node* nodes, int node_count){
    int* out_count = calloc(node_count, sizeof(int));
    for (int i = 0; i < node_count; i++) {
        mesh_node* in_range_nodes = get_node_in_range(nodes, node_count, nodes[i].id, MESH_MAX_ROUTER_DISTANCE, &out_count[i]);
        if (out_count[i] == 0) {
            printf("Node %02d: No nodes in range\n", nodes[i].id);
            continue;
        }
        printf("Node %02d have %03d Nodes in range: ", nodes[i].id, out_count[i]);
        for (int j = 0; j < out_count[i]; j++) {
            printf("%02d at (%06.2f meters)", in_range_nodes[j].id, sqrt(pow(nodes[i].x - in_range_nodes[j].x, 2) + pow(nodes[i].y - in_range_nodes[j].y, 2)));
        }
        printf("\n");
        free(in_range_nodes);
    }
}

void mesh_debug_print_mesh_data(mesh* m) {
    printf("Mesh Data Summary:\n");
    printf("Total Nodes: %d\n", m->node_count);
    printf("Total Links: %d\n", m->link_count);
    int connected_router = 0;
    int connected_end_device = 0;
    for (int i = 0; i < MESH_ROUTERS_COUNT; i++) {
        if (m->nodes[i].node_status == CONNECTED) {
            connected_router++;
        }
    }
    for(int i = MESH_ROUTERS_COUNT; i < m->node_count; i++) {
        if (m->nodes[i].node_status == CONNECTED) {
            connected_end_device++;
        }
    }
    printf("Connected Routers: %d\n", connected_router);
    printf("Disconnected Routers: %d\n", MESH_ROUTERS_COUNT - connected_router);
    printf("Connected End Devices: %d\n", connected_end_device);
    printf("Disconnected End Devices: %d\n", (m->node_count - MESH_ROUTERS_COUNT) - connected_end_device);
    int connected_nodes = connected_router + connected_end_device;
    printf("Connected Nodes: %d\n", connected_nodes);
    printf("Disconnected Nodes: %d\n", m->node_count - connected_nodes);
    printf("Total Mesh Networks: %d\n", m->mnet_count);
}

void mesh_debug_print_network(mesh_network* network) {
    printf("Mesh Network ID: %d\n", network->id);
    printf("Node Count: %d\n", network->node_count);
    printf("Color: RGB(%d, %d, %d)\n", network->color_r, network->color_g, network->color_b);
    printf("Nodes in Network:\n");
    for (int i = 0; i < network->node_count; i++) {
        printf(" - Node ID: %02d\n", network->nodes[i]->id);
    }
}

void mesh_debug_print_networks(mesh* m) {
    printf("Mesh Networks Summary:\n");
    printf("Total Networks: %d\n", m->mnet_count);
    printf("Allocated Network Slots: %d, Used Slots: %d\n", m->mnet_allocated, m->mnet_count);
    for (int i = 0; i < m->mnet_count; i++) {
        mesh_debug_print_network(m->mnets[i]);
    }
}

void mesh_debug_print_network_stats(mesh* m, mesh_network** network) {
    // int total_links = 0;
    // for (int i = 0; i < m->mnet_count; i++) {
    //     for (int j = 0; j < m->mnets[i]->node_count; j++) {
    //     total_links += m->mnets[i]->nodes[j]->output_link_count;
    //     }
    // }
    int max_nodes_in_network = 0;
    for (int i = 0; i < m->mnet_count; i++) {
        if (m->mnets[i]->node_count > max_nodes_in_network) {
            max_nodes_in_network = m->mnets[i]->node_count;
        }
    }
    printf("Mesh Network Statistics:\n");
    printf(" - Total Networks: %d\n", m->mnet_count);
    printf(" - Max Nodes in any Network: %d\n", max_nodes_in_network);
}




