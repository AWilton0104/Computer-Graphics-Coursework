#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/maths.hpp>
#include <common/camera.hpp>
#include <common/model.hpp>
#include <common/light.hpp>

// Function prototypes
void keyboardInput(GLFWwindow* window);
void mouseInput(GLFWwindow* window);

// Frame timers
float previousTime = 0.0f;  // time of previous iteration of the loop
float deltaTime = 0.0f;  // time elapsed since the previous frame
float teapotSize = 0.8f; // Used for collisions with teapot
float teapotRotation = 0.0f;
float teapotScale = 1.0f;

int state = 0;
glm::vec3 state1 = glm::vec3(9.0f, 0.5f, 9.0f);
glm::vec3 state2 = glm::vec3(9.0f, 0.5f, -9.0f);
glm::vec3 state3 = glm::vec3(-9.0f, 0.5f, 9.0f);
glm::vec3 state4 = glm::vec3(-9.0f, 0.5f, -9.0f);

// Create camera object
Camera camera(glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f));

// Object struct
struct Object
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    float angle = 0.0f;
    std::string name;
};

int main(void)
{
    // =========================================================================
    // Window creation - you shouldn't need to change this code
    // -------------------------------------------------------------------------
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    GLFWwindow* window;
    window = glfwCreateWindow(1024, 768, "A_Wilton Coursework", NULL, NULL);

    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    // -------------------------------------------------------------------------
    // End of window creation
    // =========================================================================

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Use back face culling
    //glEnable(GL_CULL_FACE);

    // Ensure we can capture keyboard inputs
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Capture mouse inputs
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Compile shader program
    unsigned int shaderID, lightShaderID;
    shaderID = LoadShaders("vertexShader.glsl", "fragmentShader.glsl");
    lightShaderID = LoadShaders("lightVertexShader.glsl", "lightFragmentShader.glsl");

    // Activate shader
    glUseProgram(shaderID);

    // Add light sources
    Light lightSources;

    lightSources.addPointLight(glm::vec3(0.0f, 0.0f, 0.0f),        // position
        glm::vec3(1.0f, 1.0f, 1.0f),         // colour
        1.0f, 0.1f, 0.02f);                  // attenuation

    lightSources.addSpotLight(glm::vec3(0.0f, 8.0f, 0.0f),          // position
        glm::vec3(0.0f, -1.0f, 0.0f),         // direction
        glm::vec3(1.0f, 0.0f, 1.0f),          // colour
        1.0f, 0.1f, 0.02f,                    // attenuation
        std::cos(Maths::radians(180.0f)));     // cos(phi)

    lightSources.addDirectionalLight(glm::vec3(0.0f, -1.0f, 0.0f),  // direction
        glm::vec3(1.0f, 1.0f, 1.0f));  // colour

    // Load models
    Model teapot("../assets/teapot.obj"); 
    Model sphere("../assets/sphere.obj"); 

    // Load the textures
    teapot.addTexture("../assets/blue.bmp", "diffuse"); 
    teapot.addTexture("../assets/diamond_normal.png", "normal"); 

    // Define teapot object lighting properties
    teapot.ka = 0.2f; 
    teapot.kd = 0.7f; 
    teapot.ks = 0.6f; 
    teapot.Ns = 20.0f; 

    // Teapot positions
    glm::vec3 teapotPositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(-3.0f,  3.0f, -3.0f),
        glm::vec3(-4.0f,  5.0f, -8.0f),
        glm::vec3(2.0f,  2.0f, -6.0f),
        glm::vec3(3.0f,  3.0f, 3.0f),
        glm::vec3(0.0f,  5.0f, -5.0f),
        glm::vec3(8.0f,  2.0f, -4.0f),
        glm::vec3(2.0f,  4.0f, -2.0f),
        glm::vec3(-8.0f,  1.0f, -2.0f),
        glm::vec3(0.0f,  5.0f, 5.0f),
        glm::vec3(8.0f,  2.0f, 4.0f),
        glm::vec3(2.0f,  4.0f, 2.0f),
        glm::vec3(-8.0f,  1.0f, 2.0f)
    };

    // Add teapots to objects vector
    std::vector<Object> objects;
    Object object;
    object.name = "teapot";
    for (unsigned int i = 0; i < sizeof(teapotPositions)/sizeof(teapotPositions[0]); i++)
    {
        object.position = teapotPositions[i];
        object.rotation = glm::vec3(1.0f, 1.0f, 1.0f);
        object.scale = glm::vec3(0.75f, 0.75f, 0.75f);
        object.angle = Maths::radians(20.0f * i);
        objects.push_back(object);
    }

    // Load a Suzanne mode
    Model suzanne("../assets/suzanne.obj"); 
    suzanne.addTexture("../assets/suzanne_diffuse.png", "diffuse"); 
    suzanne.addTexture("../assets/suzanne_normal.png", "normal"); 

    // Define Suzanne light properties
    suzanne.ka = 0.2f; 
    suzanne.kd = 0.7f;  
    suzanne.ks = 0.6f; 
    suzanne.Ns = 20.0f; 

    // Add Suzanne model to objects vector
    object.position = glm::vec3(0.0f, 0.0f, 0.0f); 
    object.scale = glm::vec3(0.25f, 0.25f, 0.25f); 
    object.rotation = glm::vec3(0.0f, 1.0f, 0.0f); 
    object.angle = 0.0f; 
    object.name = "suzanne"; 
    objects.push_back(object); 

    // Load a 2D plane model for the floor and add textures
    Model floor("../assets/plane.obj");
    floor.addTexture("../assets/stones_diffuse.png", "diffuse");
    floor.addTexture("../assets/stones_normal.png", "normal");
    floor.addTexture("../assets/neutral_specular.png", "specular");

    // Define floor light properties
    floor.ka = 0.2f;
    floor.kd = 1.0f;
    floor.ks = 1.0f;
    floor.Ns = 20.0f;

    // Add floor model to objects vector
    object.position = glm::vec3(0.0f, -0.85f, 0.0f);
    object.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    object.rotation = glm::vec3(0.0f, 1.0f, 0.0f);
    object.angle = 0.0f;
    object.name = "floor";
    objects.push_back(object);

    // Load a 2D plane model for the wall and add textures
    Model wall("../assets/plane.obj");
    wall.addTexture("../assets/bricks_diffuse.png", "diffuse");
    wall.addTexture("../assets/bricks_normal.png", "normal");
    wall.addTexture("../assets/bricks_specular.png", "specular");

    // Define wall light properties
    wall.ka = 0.2f;
    wall.kd = 1.0f;
    wall.ks = 1.0f;
    wall.Ns = 20.0f;

    // Add walls model to objects vector
    object.name = "wall";
    object.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    object.angle = Maths::radians(90.0f);

    object.position = glm::vec3(-10.0f, 0.0f, 0.0f);
    object.rotation = glm::vec3(0.0f, 0.0f, 1.0f);
    objects.push_back(object);

    object.position = glm::vec3(10.0f, 0.0f, 0.0f);
    object.rotation = glm::vec3(0.0f, 0.0f, 1.0f); 
    objects.push_back(object);

    object.position = glm::vec3(0.0f, 0.0f, 10.0f);
    object.rotation = glm::vec3(1.0f, 0.0f, 0.0f);
    objects.push_back(object);

    object.position = glm::vec3(0.0f, 0.0f, -10.0f);
    object.rotation = glm::vec3(1.0f, 0.0f, 0.0f);
    objects.push_back(object);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Update timer
        float time = glfwGetTime();
        deltaTime = time - previousTime;
        previousTime = time;

        // Get inputs
        keyboardInput(window);
        mouseInput(window);

        // Clear the window
        glClearColor(0.0f, 0.f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate view and projection matrices
        camera.target = camera.eye + camera.front;
        camera.quaternionCamera(deltaTime);

        // Activate shader
        glUseProgram(shaderID);

        // Send view matrix to the shader
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "V"), 1, GL_FALSE, &camera.view[0][0]);

        if (camera.eye.x > 9.8f) {
            camera.eye.x = 9.8f;
        }
        else if (camera.eye.x < -9.8f) {
            camera.eye.x = -9.8f;
        }

        if (camera.eye.z > 9.8f) {
            camera.eye.z = 9.8f;
        }
        else if (camera.eye.z < -9.8f) {
            camera.eye.z = -9.8f;
        }

        if (Maths::magnitude(camera.eye - state1) < 2.0f) { state = 1; }
        else if (Maths::magnitude(camera.eye - state2) < 2.0f) { state = 2; }
        else if (Maths::magnitude(camera.eye - state3) < 2.0f) { state = 3; }
        else if (Maths::magnitude(camera.eye - state4) < 2.0f) { state = 4; }
        else { state = 0; }

        // Send light source properties to the shader
        lightSources.toShader(shaderID, camera.view, state, deltaTime); 

        // Loop through objects
        for (unsigned int i = 0; i < static_cast<unsigned int>(objects.size()); i++)
        {
            // Calculate model matrix
            glm::mat4 translate = Maths::translate(objects[i].position);
            glm::mat4 scale = Maths::scale(objects[i].scale);
            glm::mat4 rotate = Maths::rotate(objects[i].angle, objects[i].rotation);
            if (objects[i].name == "teapot") {
                rotate = Maths::rotate(objects[i].angle * teapotRotation, objects[i].rotation);
                scale = Maths::scale(objects[i].scale * cos(teapotScale)); 
                if (state == 1) {
                    teapotRotation += deltaTime/4;
                }
                else if (state == 2) {
                    teapotScale += deltaTime/4;
                }
                if (Maths::magnitude(camera.eye - objects[i].position) < teapotSize) {
                    camera.eye = objects[i].position + Maths::normalise(camera.eye - objects[i].position) * teapotSize;
                }
            }
            else if (objects[i].name == "suzanne") {
                if (!camera.thirdPerson) {
                    continue;
                }
                translate = Maths::translate(camera.eye);
                rotate = Maths::rotate(-camera.yaw, glm::vec3(0.0f, 1.0f, 0.0f)) * Maths::rotate(camera.pitch, glm::vec3(1.0f, 0.0f, 0.0f));
            }
            glm::mat4 model = translate * rotate * scale;

            // Send the MVP and MV matrices to the vertex shader
            glm::mat4 MV = camera.view * model;
            glm::mat4 MVP = camera.projection * MV;
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "MV"), 1, GL_FALSE, &MV[0][0]);

            // Draw the model
            if (objects[i].name == "teapot")
                teapot.draw(shaderID);

            if(objects[i].name == "suzanne")
                suzanne.draw(shaderID);

            if (objects[i].name == "floor")
                floor.draw(shaderID);

            if (objects[i].name == "wall")
                wall.draw(shaderID);
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    teapot.deleteBuffers();
    glDeleteProgram(shaderID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}

void keyboardInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Move the camera using WSAD keys
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.eye += 5.0f * deltaTime * camera.front;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.eye -= 5.0f * deltaTime * camera.front;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.eye -= 5.0f * deltaTime * camera.right;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.eye += 5.0f * deltaTime * camera.right;

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        camera.thirdPerson = false;

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        camera.thirdPerson = true;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.jump();

}

void mouseInput(GLFWwindow* window)
{
    // Get mouse cursor position and reset to centre
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Update yaw and pitch angles
    camera.yaw += 0.001f * float(xPos - 1024 / 2);
    camera.pitch += 0.001f * float(768 / 2 - yPos);

    // Calculate camera vectors from the yaw and pitch angles
    camera.calculateCameraVectors();
}
