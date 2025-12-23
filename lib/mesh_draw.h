#pragma once 

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "mesh_compute.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PURE_WHITE (SDL_Color){255, 255, 255, 255}
#define WHITE (SDL_Color){228, 223, 218, 255}
#define GRAY (SDL_Color){54, 54, 54, 255}
#define RED (SDL_Color){172, 20, 20, 255}
#define GREEN (SDL_Color){101, 135, 97, 255}
#define YELLOW (SDL_Color){223, 160, 93, 255}
#define BLUE (SDL_Color){142, 184, 229, 255}
#define MAGENTA (SDL_Color){202, 168, 245, 255}


typedef struct Environment
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    int window_width;
    int window_height;
    int mouse_x;
    int mouse_y;
    float zoom_level;
    bool is_running;
    mesh* mesh_data;
} Environment;

typedef struct TextLabel
{
    SDL_Rect rect;
    SDL_Texture *texture;
} TextLabel;

// TextLabel UI_CreateTextLabel(UIEnvironment *env, int fontSize, const char *text, int x, int y, SDL_Color color);

SDL_Window* create_window();

SDL_Renderer* create_renderer(SDL_Window* window);

void Sdl_DrawNode(SDL_Renderer* renderer, mesh_node* node);

void Sdl_DrawLink(SDL_Renderer* renderer, mesh_link* link,mesh_network* mnet);

void Sdl_DrawMesh(SDL_Renderer* renderer, mesh_node* nodes, int node_count);

void Sdl_RenderMesh(SDL_Renderer* renderer, mesh* m);

void Sdl_DrawPath(SDL_Renderer* renderer, mesh_node* nodes, mesh_path* path);

void Sdl_Delay(int milliseconds);

void Sdl_Cleanup(SDL_Window* window, SDL_Renderer* renderer);

// function to render a zoomed view of the mesh
void Sdl_RenderZoomedMesh(SDL_Renderer* renderer, mesh* m, float zoom_factor, float offset_x, float offset_y);

void Sdl_sim_updater(Environment* env);