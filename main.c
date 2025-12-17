#include "mesh_compute.h"

int main() {
    // Initialize mesh
    mesh* my_mesh = init_mesh("TestMesh");

    for (int i = 0; i < 32; i++) {
        // Create links between nodes
        if (i < my_mesh->node_count - 1) {
            my_mesh->nodes[i].output_link[i%2] = init_mesh_link(&my_mesh->links[my_mesh->link_count++], my_mesh->link_count, &my_mesh->nodes[i], &my_mesh->nodes[i + 1]); 
        }
    }


    mesh_debug_print_mesh(my_mesh);
    MESH_SAVEDUMP(my_mesh, ALL);


    // Free mesh resources
    free_mesh(my_mesh);

    return 0;
}