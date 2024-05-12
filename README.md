# World Map LOD
My implementation of a **Level of Detail (LOD)** system that **dynamically** manages how detailed the terrain should be based on the distance to the camera to **_safe resources_** and **_increase performance_**.

## Implementation details
* Based on a dynamically managed **[Quadtree](https://en.wikipedia.org/wiki/Quadtree)**, meaning the **user can adjust at which distance** the quadtree should be split.
* I used: **C++** and **OpenGL**.
* The leafs **vertices and indices** are saved in **RAM** to check **whether they changed** to avoid sending data too often to the **GPU**, **further increasing performance**.
* For the sake of visualisation:
   * I coded a **quick and cheap water** shader.
   * the terrain's scale is **2:1**, because:
   * Earth's **heightmap** and **colormap** are used.
* The **_biggest struggle_** definitely was how I'd fix the cracks created by two neighbouring terrain faces having a different level of detail.
  * I Fixed that issue by using a **[Bitmask](https://en.wikipedia.org/wiki/Mask_(computing))** to **efficiently** find neighbouring terrain faces and compare their LOD, then adjusting the proper edge vertices of the higher LOD face.

## Try it out yourself
* Just downlaod the ZIP under "Releases"
* Unzip the archive and run the .exe
