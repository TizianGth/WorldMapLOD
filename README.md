# World Map LOD
A C++ and OpenGL implementation of a Level of Detail (LOD) system for rendering a global terrain map. It dynamically adjusts terrain detail based on camera distance to optimize performance and conserve system resources.

---

## Overview
This project uses a dynamic quadtree to manage terrain detail levels in real-time, improving rendering efficiency and visual fidelity. Special attention is given to preventing common LOD issues like terrain cracks.

---

## Key Features
* Dynamic quadtree-based LOD
* Adjustable split distance for LOD refinement
* Crack-free terrain transitions
* Neighbors compared using a bitmask system for consistent edge resolution
* Real-world data integration (Uses Earth's actual heightmap and colormap)
* Simple visualization shaders:
  * Includes a basic water shader for aesthetic enhancement
    
---

## Getting Started
1. Download the latest ZIP from the [Releases](https://github.com/TizianGth/WorldMapLOD/releases) section
2. Unzip the archive
3. Run the executable (.exe)

No additional setup is required.

---

## Tech Stack
* C++
* OpenGL

---

## Known Challenges & Solutions
* Terrain Cracks at LOD Boundaries
* Fixed by:

  * Calculating neighbors using a bitmask

  * Adjusting the edge vertices of the higher LOD face to match the lower one

---

## Demo
![Clip1](https://github.com/TizianGth/readmeResources/blob/main/WorldMapLOD/Clip1.webp?raw=true)
![Clip2](https://github.com/TizianGth/readmeResources/blob/main/WorldMapLOD/Clip2.webp?raw=true)
![Clip3](https://github.com/TizianGth/readmeResources/blob/main/WorldMapLOD/Clip3.webp?raw=true)


 
