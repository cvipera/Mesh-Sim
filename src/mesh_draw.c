#include "../lib/mesh_draw.h"


SDL_Window* create_window() {
    SDL_Window* window = SDL_CreateWindow("Mesh Render", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SDL_WIDTH, SDL_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("Could not create window: %s", SDL_GetError());
        return NULL;
    }
    return window;
}

SDL_Renderer* create_renderer(SDL_Window* window) {
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Could not create renderer: %s", SDL_GetError());
        return NULL;
    }
    return renderer;
}

void Sdl_DrawNode(SDL_Renderer* renderer, mesh_node* node) {

    if(node->node_status == CONNECTED) {
        if (node->node_type == ROUTER) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for routers
        else SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow for end devices
    }
    else if(node->node_status == ACTIVE){
        if (node->node_type == ROUTER) SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255); // Darker Green for active routers
        else SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255); // Darker Yellow for active end devices
    } 
    else if(node->node_status == INACTIVE){
        if (node->node_type == ROUTER) SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255); // Darkest Green for inactive routers
        else SDL_SetRenderDrawColor(renderer, 100, 100, 0, 255); // Darkest Yellow for inactive end devices
    } 
    else if(node->node_status == DISCONNECTED && node->node_type == ROUTER){
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue for inactive routers

    } else if(node->node_status == DISCONNECTED && node->node_type == END_DEVICE){
        SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255); // Light Blue for inactive end devices
    }
    SDL_Rect rect = { (int)node->x*SIZE_MULTIPLIER, (int)node->y*SIZE_MULTIPLIER, 2, 2 };
    SDL_RenderFillRect(renderer, &rect);
}






void Sdl_DrawLink(SDL_Renderer* renderer, mesh_link* link,mesh_network* mnet) {
    SDL_SetRenderDrawColor(renderer, mnet->color_r, mnet->color_g, mnet->color_b, 255); // Network color for links
    SDL_RenderDrawLine(renderer, (int)link->source->x*SIZE_MULTIPLIER, (int)link->source->y*SIZE_MULTIPLIER, (int)link->destination->x*SIZE_MULTIPLIER, (int)link->destination->y*SIZE_MULTIPLIER);
}







void Sdl_RenderMesh(SDL_Renderer* renderer, mesh* m) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    // Draw links first
    for (int i = 0; i < m->node_count; i++) {
        mesh_node* node = &m->nodes[i];
        for (int j = 0; j < node->output_link_count; j++) {
            mesh_network* mnet = NULL;
            if (node->network_id != -1 && node->network_id < m->created_networks) {
                mnet = m->mnets[get_mesh_network_index_by_id(m,node->network_id)];
            }else printf("Warning: Node %d has invalid network ID %d\n", node->id, node->network_id);
            Sdl_DrawLink(renderer, &node->output_link[j], mnet);
        }
    }

    // Draw nodes on top of links
    for (int i = 0; i < m->node_count; i++) {
        Sdl_DrawNode(renderer, &m->nodes[i]);
    }

    SDL_RenderPresent(renderer);
}

void Sdl_Delay(int milliseconds) {
    SDL_Delay(milliseconds);
}

void Sdl_Cleanup(SDL_Window* window, SDL_Renderer* renderer) {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

// function to render a zoomed view of the mesh
void Sdl_RenderZoomedMesh(SDL_Renderer* renderer, mesh* m, float zoom_factor, float offset_x, float offset_y) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);
    // Draw links first
    for (int i = 0; i < m->node_count; i++) {
        mesh_node* node = &m->nodes[i];
        for (int j = 0; j < node->output_link_count; j++) {
            SDL_SetRenderDrawColor(renderer, m->mnets[get_mesh_network_index_by_id(m,node->network_id)]->color_r,
                m->mnets[get_mesh_network_index_by_id(m,node->network_id)]->color_g,
                m->mnets[get_mesh_network_index_by_id(m,node->network_id)]->color_b, 255); // Network color for links
            SDL_RenderDrawLine(renderer,
                (int)((node->x * zoom_factor + offset_x) * SIZE_MULTIPLIER),
                (int)((node->y * zoom_factor + offset_y) * SIZE_MULTIPLIER),
                (int)((node->output_link[j].destination->x * zoom_factor + offset_x) * SIZE_MULTIPLIER),
                (int)((node->output_link[j].destination->y * zoom_factor + offset_y) * SIZE_MULTIPLIER));
        }
    }   
    // Draw nodes on top of links
    for (int i = 0; i < m->node_count; i++) {
        mesh_node* node = &m->nodes[i];
        if(node->node_status == CONNECTED) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for nodes
        else SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue for inactive nodes
        SDL_Rect rect = { (int)((node->x * zoom_factor + offset_x) * SIZE_MULTIPLIER), (int)((node->y * zoom_factor + offset_y) * SIZE_MULTIPLIER), 2, 2 };
        SDL_RenderFillRect(renderer, &rect);
    }   
    SDL_RenderPresent(renderer);
}
    




void Sdl_sim_updater(Environment* env)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                Sdl_Cleanup(env->window, env->renderer);  
                break;

            case SDL_MOUSEWHEEL:
                if (event.wheel.y > 0) {
                    env->zoom_level *= 1.1f; // Zoom in
                } else if (event.wheel.y < 0) {
                    env->zoom_level /= 1.1f; // Zoom out 
                }
                break;
            case SDL_MOUSEMOTION:
                env->mouse_x += event.motion.xrel;
                env->mouse_y += event.motion.yrel;
                break;
                case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    Sdl_Cleanup(env->window, env->renderer);  
                    exit(0);
                }else if (event.key.keysym.sym == SDLK_r) {
                    // Reset view
                    env->zoom_level = 1.0f;
                    env->mouse_x = 0;
                    env->mouse_y = 0;
                }else if (event.key.keysym.sym == SDLK_SPACE) {
                    // Pause/Resume simulation
                    env->is_running = !env->is_running;
                }
                break;
            }
        }
    }

