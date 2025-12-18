#include "mesh_compute.h"
#include "mesh_draw.h"

int main() {
    // Initialize mesh
    mesh* my_mesh = init_mesh("TestMesh");

    /*printf(" size of struct mesh: %lu bytes\n", sizeof(mesh));
    printf(" size of struct mesh_node: %lu bytes\n", sizeof(mesh_node));
    printf(" size of struct mesh_link: %lu bytes\n", sizeof(mesh_link));
    printf(" size of struct mesh_path: %lu bytes\n", sizeof(mesh_path));
    */




    for (int i = 0; i < my_mesh->node_count - 2; i++) {
        // Create links between nodes
        if (mesh_link_allowed(&my_mesh->nodes[i], &my_mesh->nodes[i + 1])) {
            my_mesh->link_count++;
            init_mesh_link(my_mesh->link_count - 1, &my_mesh->nodes[i], &my_mesh->nodes[i + 1]);
        }
        if (mesh_link_allowed(&my_mesh->nodes[i], &my_mesh->nodes[i + 2])) {
            my_mesh->link_count++;
            init_mesh_link(my_mesh->link_count - 1, &my_mesh->nodes[i], &my_mesh->nodes[i + 2]);
        }
    } 
    
    // Save mesh data to CSV
    MESH_SAVEDUMP(my_mesh, ALL);

    mesh_debug_print_mesh(my_mesh);

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = create_window();
    if (!window) {
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer = create_renderer(window);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    // Render the mesh
    Sdl_RenderMesh(renderer, my_mesh);
    scanf("%*c"); // Wait for user input to close

    Sdl_RenderZoomedMesh(renderer, my_mesh, 3, 0, 0); // Example zoomed render

    scanf("%*c"); // Wait for user input to close
    Sdl_Cleanup(window, renderer);

    // Free mesh resources
    free_mesh(my_mesh);

    return 0;
}