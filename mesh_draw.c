#include "mesh_draw.h"

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
    if(node->node_status == CONNECTED) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for nodes
    else SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue for inactive nodes
    SDL_Rect rect = { (int)node->x*SIZE_MULTIPLIER, (int)node->y*SIZE_MULTIPLIER, 10, 10 };
    SDL_RenderFillRect(renderer, &rect);
}

void Sdl_DrawLink(SDL_Renderer* renderer, mesh_link* link) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for links
    SDL_RenderDrawLine(renderer, (int)link->source->x*SIZE_MULTIPLIER + 5, (int)link->source->y*SIZE_MULTIPLIER + 5, (int)link->destination->x*SIZE_MULTIPLIER + 5, (int)link->destination->y*SIZE_MULTIPLIER + 5);
}

void Sdl_RenderMesh(SDL_Renderer* renderer, mesh* m) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    // Draw links first
    for (int i = 0; i < m->node_count; i++) {
        mesh_node* node = &m->nodes[i];
        for (int j = 0; j < node->output_link_count; j++) {
            Sdl_DrawLink(renderer, &node->output_link[j]);
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
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for links
            SDL_RenderDrawLine(renderer,
                (int)((node->x * zoom_factor + offset_x) * SIZE_MULTIPLIER) + 5,
                (int)((node->y * zoom_factor + offset_y) * SIZE_MULTIPLIER) + 5,
                (int)((node->output_link[j].destination->x * zoom_factor + offset_x) * SIZE_MULTIPLIER) + 5,
                (int)((node->output_link[j].destination->y * zoom_factor + offset_y) * SIZE_MULTIPLIER) + 5);
        }
    }   
    // Draw nodes on top of links
    for (int i = 0; i < m->node_count; i++) {
        mesh_node* node = &m->nodes[i];
        if(node->node_status == CONNECTED) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for nodes
        else SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue for inactive nodes
        SDL_Rect rect = { (int)((node->x * zoom_factor + offset_x) * SIZE_MULTIPLIER), (int)((node->y * zoom_factor + offset_y) * SIZE_MULTIPLIER), 10, 10 };
        SDL_RenderFillRect(renderer, &rect);
    }   
    SDL_RenderPresent(renderer);
}
    
