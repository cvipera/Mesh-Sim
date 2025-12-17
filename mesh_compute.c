#include "mesh_compute.h"

mesh_node* find_node_by_id(mesh_node* nodes, int node_count, int id) {
    return &nodes[id];
}

mesh* init_mesh(const char* name) {
    mesh* m = (mesh*)malloc(sizeof(mesh));
    strncpy(m->name, name, sizeof(m->name) - 1);
    m->name[sizeof(m->name) - 1] = '\0';
    m->node_count = MESH_NODES_COUNT;
    m->nodes = (mesh_node*)malloc(sizeof(mesh_node) * m->node_count);
    m->link_count = 0;
    m->links = NULL;
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
    if (m->links) {
        free(m->links);
        m->links = NULL;
    }
    if (m->paths) {
        free(m->paths);
        m->paths = NULL;
    }
    m->node_count = 0;
    m->link_count = 0;
    m->path_count = 0;
}

mesh_node* init_mesh_node (mesh_node* node, int id) {
    node->id = id;
    node->input_link_count = 0;
    node->node_status = DISCONNECTED; // Initially disconnected
    node->node_type = MESH_ROUTERS_COUNT > id ? ROUTER : END_DEVICE; // First N nodes are routers
    node->x = random() % (int)MESH_SIZE_X;
    node->y = random() % (int)MESH_SIZE_Y;
    node->output_link = (mesh_link){0}; // No output links initially
    node->input_links = (mesh_link*){0}; // No input links initially
    return node;
}

mesh_node init_mesh_link(mesh_link* link, int id, mesh_node* source, mesh_node* destination) {
    link->id = id;
    link->length = sqrt(pow(source->x - destination->x, 2) + pow(source->y - destination->y, 2));
    link->bandwidth = MESH_DEFAULT_BANDWIDTH; // Decrease bandwidth with length
    link->latency = ( link->length * 10);
    link->source = *source;
    link->destination = destination;
    destination->input_links[++destination->input_link_count] = link;
    source->output_link = *link;
    source->node_status = CONNECTED;
    return link;
}

mesh_path* init_mesh_path(mesh_path* path,int id, int start_node_id, int end_node_id) {
    path->id = id;
    path->start_node_id = -1;
    path->end_node_id = -1;
    path->length = 0;
    path->nodes = NULL;
    return path;
}

int MESH_SAVEDUMP(mesh* m, enum data d) {
    FILE* file = fopen("mesh_data.csv", "w");
    if (!file) {
        return -1; // Failure to open file
    }

    if (d == NODES || d == ALL) {
        fprintf(file, "NodeID,X,Y,Status,Type\n");
        for (int i = 0; i < m->node_count; i++) {
            mesh_node* node = &m->nodes[i];
            fprintf(file, "%d,%.2f,%.2f,%d,%d\n", node->id, node->x, node->y, node->node_status, node->node_type);
        }
    }

    if (d == LINKS || d == ALL) {
        fprintf(file, "LinkID,SourceID,DestinationID,Bandwidth,Latency\n");
        for (int i = 0; i < m->link_count; i++) {
            mesh_link* link = &m->links[i];
            fprintf(file, "%d,%d,%d,%.2f,%.2f\n", link->id, link->source->id, link->destination->id, link->bandwidth, link->latency);
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
    printf("Node ID: %d                  ", node->id);
    printf("Coordinates: (%.2f, %.2f)    ", node->x, node->y);
    printf("Status: %d     ", node->node_status);
    printf("Type: %d       ", node->node_type);
    printf("Input Links Count: %d        ", node->input_link_count);
    if (node->output_link) {
        printf("Output Link ID: %d       \n", node->output_link->id);
    } else {
        printf("No Output Link\n");
    }
}

void mesh_debug_print_link(mesh_link* link) {
    printf("Link ID: %d                ", link->id);
    printf("Length: %.2f meters        ", link->length);
    printf("Source Node ID: %d         ", link->source->id);
    printf("Destination Node ID: %d    ", link->destination->id);
    printf("Bandwidth: %.2f Mbps       ", link->bandwidth);
    printf("Latency: %.2f ms           \n", link->latency);
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
    for (int i = 0; i < m->link_count; i++) {
        mesh_debug_print_link(&m->links[i]);
    }
    printf("\nPaths:\n");
    for (int i = 0; i < m->path_count; i++) {
        mesh_debug_print_path(&m->paths[i]);
    }
}