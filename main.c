#include "lib/mesh_compute.h"
#include "lib/mesh_draw.h"
#include "lib/utils.h"
#include <time.h>


int main() {

    srand(546789); // Seed for reproducibility


    // --------------------------INITIALIZATION--------------------------
    memset(bitmask, 0, sizeof(bitmask)); // Initialize bitmask to zero
    mesh* my_mesh = init_mesh("TestMesh"); // Initialize mesh
    if (SDL_Init(SDL_INIT_VIDEO) < 0) { // Initialize SDL
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
    Sdl_RenderMesh(renderer, my_mesh);    // Render the initial mesh
    SDL_Event event;
    // -----------------------END INITIALIZATION--------------------------





    // --------------------------SIMULATION CODE HERE--------------------------
    for (int i = 0; i < 100; i++) {
        int startnode = random() % my_mesh->node_count;
        int inrangecount = 0;
        mesh_node* in_range = get_node_in_range(my_mesh->nodes, my_mesh->node_count, startnode, MESH_MAX_ROUTER_DISTANCE, &inrangecount);        
        if (inrangecount == 0) {
            free(in_range);
            continue;
        }else {
            for (int j = 0; j < inrangecount; j++) {
                    
                int targetindex = random() % inrangecount;

                mesh_node* targetnode = get_node_by_id(my_mesh->nodes, my_mesh->node_count, in_range[targetindex].id);
                if (mesh_link_allowed(&my_mesh->nodes[startnode], targetnode) && (targetnode->network_id != my_mesh->nodes[startnode].network_id || targetnode->network_id == -1)) {
                    init_mesh_link(&my_mesh->nodes[startnode], targetnode, my_mesh);
                    my_mesh->link_count++;
                  //  my_mesh->nodes[startnode].node_status = CONNECTED;
                  //  targetnode->node_status = CONNECTED;
                    goto link_created;
                }
            }
link_created:
            free(in_range);
        }

        Sdl_sim_updater(my_mesh,renderer,event);



        Sdl_RenderMesh(renderer, my_mesh);    // Render the updated mesh after link creation
        SDL_Delay(250); // Delay for visualization
    }


    //Sdl_RenderZoomedMesh(renderer, my_mesh, 2.0f,-50.0f,-50.0f); // Render zoomed mesh



    // --------------------------END SIMULATION CODE--------------------------







    // --------------------------DEBUG PRINTS--------------------------
    // mesh_debug_print_mesh(my_mesh);
    // mesh_debug_print_node_in_range(my_mesh->nodes, my_mesh->node_count);
    // mesh_debug_print_networks(my_mesh);
    mesh_debug_print_mesh_data(my_mesh);
    mesh_debug_print_network_stats(my_mesh, my_mesh->mnets);
    // --------------------------END DEBUG PRINTS--------------------------






    // --------------------------WAIT FOR USER TO QUIT--------------------------
    while (true) {  
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }
    }
     // --------------------------END WAIT FOR USER TO QUIT--------------------------






    // --------------------------CLEANUP--------------------------
    MESH_SAVEDUMP(my_mesh, ALL);
    Sdl_Cleanup(window, renderer);
    free_mesh(my_mesh);
    
    return 0;
}