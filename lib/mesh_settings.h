#pragma once
#if 1

#define MESH_NAME "Default Mesh Network"
#define MESH_NODES_COUNT 100
#define MESH_ROUTERS_COUNT 40 // the First N nodes will be routers
#define MESH_MAX_LINKS_PER_NODE 8
#define MESH_DEFAULT_BANDWIDTH 10000.0f // in Mbps
#define MESH_DEFAULT_LATENCY 00.0f    // in ms per meter
#define MESH_SIZE_X 36.f  // in meters
#define MESH_SIZE_Y 20.f  // in meters
#define MESH_MAX_ROUTER_DISTANCE 50.0f // in meters
#define MESH_MAX_LINK_DISTANCE 15.0f // in meters

#define SDL_WIDTH 1800
#define SDL_HEIGHT 1000
#define SDL_TITLE "Mesh Network Visualization"
#define SIZE_MULTIPLIER 50 // Multiplier to scale mesh coordinates to SDL window size


#endif