#include "mesh_compute.h"

int main() {
    // Initialize mesh
    mesh* my_mesh = init_mesh("TestMesh");

    MESH_SAVEDUMP(my_mesh, ALL);
    
    mesh_debug_print_mesh(my_mesh);

    for (int i = 0; i < 32; i++){
        init_mesh_link(&my_mesh->links[i], i, &my_mesh->nodes[i], &my_mesh->nodes[(i+1)%32]);
    }




    // Free mesh resources
    free_mesh(my_mesh);

    return 0;
}