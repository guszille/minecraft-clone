# ZCRAFT

A simple Minecraft clone created by Gustavo Zille, using C++ and OpenGL.

The project is being developed as a way to study some computer graphics and game programming techniques.

## Features

- Procedural World Generation;
  - Terrain Generation: Using Perlin Noise to create a Height Map;
  - Random Flora Generation: Trees, Leaves and Flowers.
- Optimized Mesh Building: Sampling only the visible faces to compose the generated chunk mesh;
- Translucent/Transparent Blocks;
- Lighting: Using the Blinn-Phong Lighting Model (BPLM) to create a directional light;
- Shadows: Using a Shadow Map to store the depth of the fragments on the light view perspective;
- Ambient Occlusion: Simulating Ambient Occlusion (AO) with "Corner Shadows";
- Fog: Applying an exponential fog to hide the chunks far away;
- Collisions;
  - AABB x AABB: Between the player and the solid blocks;
  - AABB x Ray: Used when the player wants to Insert/Remove a block on/from the world.
- Physically Movement: Applying concepts like velocity, gravity and friction to the player's movement system;
- Basic Inventory System.

## Gallery

<p align="center">Enviroment</p>

![Enviroment 1](./MinecraftClone/res/screenshots/enviroment1.png)
![Enviroment 2](./MinecraftClone/res/screenshots/enviroment2.png)

<p align="center">Ambient Occlusion (Simulation)</p>

![Enviroment 2 AO](./MinecraftClone/res/screenshots/enviroment2_cornershadows.png)

<p align="center">Depth Map / Shadow Map</p>

![Depth Map](./MinecraftClone/res/screenshots/depthmap.png)

<p align="center">Building</p>

![House](./MinecraftClone/res/screenshots/house1.png)
![Bunker](./MinecraftClone/res/screenshots/bunker1.png)
