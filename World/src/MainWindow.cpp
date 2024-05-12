#include "MainWindow.h"
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw_gl3.h>
#include <algorithm>

void MainWindow::Start(int width, int height)
{
	windowWidth = width;
	windowHeight = height;

    std::cout << "Starting..." << std::endl;

    if (GLInit()) {
        std::cout << "OpenGL initiated!" << std::endl;
        Init();
    }
    else {
        std::cout << "Error initiating OpenGL!" << std::endl;
    }
}

// OpenGL boilerplate
bool MainWindow::GLInit()
{
    if (!glfwInit())
        return false;

    // Create a windowed mode window and its OpenGL context 
    window = glfwCreateWindow(windowWidth, windowHeight, "World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }

    // Make the window's context current 
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return false;
    }

    glViewport(0, 0, windowWidth,windowHeight);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
    glEnable(GL_CULL_FACE);

    // wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // ImGUI Init
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark;
    
	return true;
}


void MainWindow::Init()
{
    // load shader
    shader = new Shader("/res/shaders/basic.shader");
    shader->Bind();

    // load map
    map.Init(lod, "/res/textures/heightmap.png", "/res/textures/specular.png",
        "/res/textures/color.png",
        glm::vec2(0,0), mapSize, camera.position);

    camera.SetFov(90, windowWidth, windowHeight);
    camera.MoveTo(glm::vec3(100, 20, 50));

    // set texture location (for shader)
    shader->SetUniform1i("u_WaterColorMask", 4);
    shader->SetUniform1i("u_WaterNormalSmall", 3);
    shader->SetUniform1i("u_WaterNormalBig", 2);
    shader->SetUniform1i("u_Temperature", 1);
    shader->SetUniform1i("u_Mask", 0);
    shader->SetUniform2f("u_MapSize", mapSize);

    std::cout << "Finished!" << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        Update();
        std::chrono::steady_clock::time_point update = std::chrono::steady_clock::now();
        Render();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        // messure delta time
        updateTime = std::chrono::duration<double, std::milli>(update - begin).count();
        deltaTime = std::chrono::duration<double, std::milli>(end - begin).count();
        totalTime += deltaTime / 1000;

    }
   // map.shouldUpdate = false;
    glfwTerminate();
	return;
}

void MainWindow::Update()
{
    OnMouseUpdate();
    OnKeyboardUpdate();

    UpdateWater();

    // calculte model view matrix (missing object matrix, not needed)
    projCam = camera.proj * camera.mat4;
    glm::mat4 mvp = projCam;
    shader->SetUniformMat4f("u_MVP", mvp);

    map.Update();
}

float lightWaterColor[3] = {40.0f / 255.0f, 193.0f / 255.0f, 183.0f / 255.0f };
float deepWaterColor[3] = {31.0f / 255.0f, 51.0f / 255.0f, 91.0f / 255.0f };
float skyColor[3] = { 23.0f / 255.0f, 23.0f / 255.0f, 31.0f / 255.0f };

void MainWindow::Render()
{
    // clear
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(skyColor[0], skyColor[1], skyColor[2], 1.0f);

    // render map
    map.Render();
    
    // render gui
    GuiRender();

    glfwPollEvents();
    glfwSwapBuffers(window);
}
int lastX, lastY;
double rotationX, rotationY;
void MainWindow::OnMouseUpdate()
{
    if (!mouseLocked) return;

    auto xy = Input::GetMousePosition(window);
    std::pair<double, double> delta = { xy.first - lastX, xy.second - lastY };



    rotationX += delta.first * camera.rotationSpeed;
    rotationY += delta.second * camera.rotationSpeed;
    rotationY = std::clamp(rotationY, -89.0, 89.0);

    camera.Rotate(glm::vec3(rotationX, rotationY, 0));

    int centerX = windowWidth >> 1, centerY = windowHeight >> 1;
    int maxRadius = glm::min(windowWidth, windowHeight) / 3;
    glm::vec2 d = glm::vec2(lastX, lastY) - glm::vec2(centerX, centerY);
    float len = glm::length(d);
    if (len > maxRadius) {
        glfwSetCursorPos(window, (int)centerX, (int)centerY);
        lastX = centerX, lastY = centerY;
    }
    else {
        lastX = xy.first;
        lastY = xy.second;
    }
}

void MainWindow::OnKeyboardUpdate()
{
    // Zoom functionality
    if (Input::GetKeyPressed(window, GLFW_KEY_F)) {
        camera.Zoom(windowWidth, windowHeight);
    }
    else if (camera.fov != camera.fovDefault) {
        camera.SetFov(camera.fovDefault, windowWidth, windowHeight);
    }

    // Reset location
    if (Input::GetKeyPressed(window, GLFW_KEY_R)) {
        camera.MoveTo(glm::vec3(100, 20, 50));
    }

    // Wireframe mode
    if (Input::GetKeyDown(window, GLFW_KEY_M)) {
        if (wireFrame) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        wireFrame = !wireFrame;
    }

    if (Input::GetKeyDown(window, GLFW_KEY_L)) {
        mouseLocked = !mouseLocked;
        if (mouseLocked) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }


    // Basic Movement
    glm::vec3 velocity(0.0f);
    if (Input::GetKeyPressed(window, GLFW_KEY_W)) {
        velocity += camera.direction;
    }
    if (Input::GetKeyPressed(window, GLFW_KEY_S)) {
        velocity -= camera.direction;
    }
    if (Input::GetKeyPressed(window, GLFW_KEY_A)) {
        velocity -= camera.right;
    }
    if (Input::GetKeyPressed(window, GLFW_KEY_D)) {
        velocity += camera.right;
    }
    // Normalize input to avoid faster Movement when pressing 2 buttons eg W & D
    if (velocity.x != 0 && velocity.y != 0 && velocity.z != 0)
        velocity = glm::normalize(velocity);

    velocity *= camera.movementSpeed;

    // Flying: Up/Down
    if (Input::GetKeyPressed(window, GLFW_KEY_SPACE)) {
        velocity.y += camera.movementSpeed;
    }
    if (Input::GetKeyPressed(window, GLFW_KEY_LEFT_SHIFT)) {
        velocity.y -= camera.movementSpeed;
    }

    // "Running"
    if (Input::GetKeyPressed(window, GLFW_KEY_LEFT_CONTROL)) {
        velocity *= camera.movementSpeedMaxMulti;
    }

    camera.MoveBy(velocity * (float)deltaTime);
}

const float waterWaveSpeed = 0.00038f;
const float waterFlowSpeed = 0.0005f;
void MainWindow::UpdateWater()
{
    // x = speed, y,z = small waves
    glm::vec3 waterTexCoordOffset(totalTime * waterFlowSpeed, std::sinf(totalTime * 2) * waterWaveSpeed, std::cosf(totalTime * 2) * waterWaveSpeed);

    // send to shader
    shader->SetUniform3f("u_WaterOffset", waterTexCoordOffset);
    shader->SetUniform3f("u_CameraPosition", camera.position);

    // update water color
    shader->SetUniform3f("u_LigthWaterColor", glm::vec3(lightWaterColor[0], lightWaterColor[1], lightWaterColor[2]));
    shader->SetUniform3f("u_DeepWaterColor", glm::vec3(deepWaterColor[0], deepWaterColor[1], deepWaterColor[2]));

}

void MainWindow::GuiRender()
{
    //ImGui draw
    ImGui_ImplGlfwGL3_NewFrame();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Position: x=%.1f, y=%.1f, z=%.1f", camera.position.x, camera.position.y, camera.position.z);
    ImGui::Text("Time: Update=%.3f, Render=%.3f, Total=%.3f", updateTime, deltaTime-updateTime, deltaTime);
    //ImGui::ColorEdit3("Ligth Water Color", lightWaterColor);
    //ImGui::ColorEdit3("Deep Water Color", deepWaterColor);
    ImGui::ColorEdit3("Sky Color", skyColor);
    int _lod = lod;
    ImGui::SliderInt("LOD", &_lod, minLod, maxLod);
    lod = _lod;

    ImGui::Text("\nWASD: Movement");
    ImGui::Text("Space: Fly up, LShif: Fly down, LCtrl: Go faster");
    ImGui::Text("R: Reset location, L: Lock courser, F: Zoom");
    ImGui::Text("M: Wireframe mode");

    ImGui::Render();
    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}
