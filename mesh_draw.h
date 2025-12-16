#pragma once 

#include <SDL2/SDL.h>
#include "mesh_compute.h"

#define SDL_WIDTH 800
#define SDL_HEIGHT 600
#define SDL_TITLE "Mesh Network Visualization"

SDL_Window* create_window();

SDL_Renderer* create_renderer(SDL_Window* window);

void Sdl_DrawNode(SDL_Renderer* renderer, mesh_node* node);

void Sdl_DrawLink(SDL_Renderer* renderer, mesh_link* link);

void Sdl_DrawMesh(SDL_Renderer* renderer, mesh_node* nodes, int node_count);

void Sdl_RenderMesh(SDL_Window* window, SDL_Renderer* renderer, mesh_node* nodes, int node_count);

void Sdl_DrawPath(SDL_Renderer* renderer, mesh_node* nodes, mesh_path* path);