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
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for nodes
    SDL_Rect rect = { (int)node->x, (int)node->y, 10, 10 };
    SDL_RenderFillRect(renderer, &rect);
}