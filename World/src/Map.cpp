#include "Map.h"
#include <filesystem>

Map::Map()
{
}

Map::~Map()
{
}

void Map::Init(unsigned int& depth, std::string heightmap, std::string mask, std::string tempmap, glm::vec2 min, glm::vec2 max, glm::vec3& targetPos)
{
    start.isLeaf = true;
    // Swapping Z-Axis to acount for OpenGLs coordinate system
    // => Z-Axis is swapped here to avoid confusion
    float tmp = min.y;
    min.y = max.y;
    max.y = tmp;

    // generate & bind opengl buffers
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // configure vertexAttrivArray (how the date in the verts is aranged)
    // (index, size, GL_FLOAT, GL_FALSE, sizeAll, offset)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

    // heightmap
    heightMap.Load(std::filesystem::current_path().string() + heightmap, 1);

    // land/water mask
    maskMap.Load(std::filesystem::current_path().string() + mask, 1);
    maskMap.GlBind(0);

    // color map
    tempMap.Load(std::filesystem::current_path().string() + tempmap, 3);
    tempMap.GlBind(1);

    // water waves big
    waterNormalBig.Load(std::filesystem::current_path().string() + "/res/textures/water_normal_big.jpg", 3);
    waterNormalBig.GlBind(2);

    // water waves small
    waterNormalSmall.Load(std::filesystem::current_path().string() + "/res/textures/water_normal_big.jpg", 3);
    waterNormalSmall.GlBind(3);

    // water color mask
    waterColorMask.Load(std::filesystem::current_path().string() +  "/res/textures/water_color_mask.png", 3);
    waterColorMask.GlBind(4);

    // start new independent thread
    sideThread = std::thread([&] {
        start.min = min;
        start.max = max;

        while (true) {
            UpdateThread(depth, targetPos, min);
        }
    });
    sideThread.detach();
    
}
void Map::UpdateThread(unsigned int& depth, glm::vec3& targetPos, glm::vec2 offset)
{
    Mesh newMesh;

    // recursivly generate children based on start node
    start.GenerateChildren(depth, Box(targetPos - glm::vec3(0.5f, 0.5f, 0.5f), targetPos + glm::vec3(0.5f, 0.5f, 0.5f)));

    // convert nodes to vertices
    start.GetVertices(newMesh.vertices);

    // convert nodes to indices
    int ind = 0;
    start.GetIndices(newMesh.indices, ind);

    mutex.lock();

    // Rough way of checking if vertices changed -> update mesh
    int sizeA = mesh.vertices.size();
    int sizeB = newMesh.vertices.size();
    int qA = sizeA * 0.25f;
    if (sizeB > 0 && (sizeA != sizeB || mesh.vertices[sizeA - 1 - qA] != newMesh.vertices[sizeA - 1 - qA] || mesh.vertices[qA] != newMesh.vertices[qA])) {
        verticesUpdated = true;
        mesh = newMesh;
    } 
    mutex.unlock();  

    
}


void Map::Update()
{

    // load temp. mesh data if changed
    if (verticesUpdated) {

        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        mutex.lock();
        Mesh tempMesh = mesh;
        verticesUpdated = false;
        mutex.unlock();

        iboCount = tempMesh.indices.size();

        glBufferData(GL_ARRAY_BUFFER, tempMesh.vertices.size() * sizeof(float),
            tempMesh.vertices.data(), GL_DYNAMIC_DRAW);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, iboCount * sizeof(float), tempMesh.indices.data(), GL_DYNAMIC_DRAW);

    }
}

void Map::Render()
{
    // render
    glBindVertexArray(vao);
    if (mesh.vertices.size() > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, iboCount * 3, GL_UNSIGNED_INT, nullptr);
    }
 }
