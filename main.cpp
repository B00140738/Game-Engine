#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "src/Mesh.hpp"
#include "src/Model.hpp"
#include "src/Renderer.hpp"
#include "src/load_obj.hpp"
#include "src/Camera.hpp"

const GLint WIDTH = 1200, HEIGHT = 800;

bool is_float(const std::string &str)
{
    std::istringstream iss(str);
    float f;
    iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
    return iss.eof() && !iss.fail();
}

// Global variables for mouse handling
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;
Camera* camera_ptr = nullptr;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!camera_ptr)
        return;

    if (firstMouse)
    {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos); // reversed: y ranges bottom to top
    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    camera_ptr->processMouseOffset(xoffset, yoffset);
}

int main(int argc, char **argv)
{
    int mode = 0;
    bool print_fps = false;
    if (argc < 4)
    {
        std::cout
            << "usage: ./main (obj_file) (GL_POINTS or GL_TRIANGLES or GL_LINES) (distance) (optional: fps)\n";
        std::cout << "example: ./main teapot.obj GL_LINES 10\n";
        return 0;
    }
    std::string_view chosen_render_mode(argv[2]);

    if (chosen_render_mode == "GL_POINTS")
    {
        mode = GL_POINTS;
    }
    else if (chosen_render_mode == "GL_TRIANGLES")
    {
        mode = GL_TRIANGLES;
    }
    else if (chosen_render_mode == "GL_LINES")
    {
        mode = GL_LINES;
    }
    else
    {
        std::cout << "Invalid render mode, must be: GL_POINTS, GL_TRIANGLES or GL_LINES\n";
        std::cout << "You chose: " << chosen_render_mode;
        return 0;
    }

    if (!is_float(argv[3]))
    {
        std::cout << "Invalid distance, must be a number\n";
        std::cout << "You chose: " << argv[3];
        return 0;
    }

    if (argc == 5)
        print_fps = true;

    float distance = std::stof(argv[3]);

    std::ios_base::sync_with_stdio(false);

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "OGL", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Couldn't make window:(\n";
        glfwTerminate();
        return -1;
    }

    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    glfwMakeContextCurrent(window);
    // glfwSwapInterval(0); // vsync off - Huge difference!

    glewExperimental = GL_TRUE;

    if (GLEW_OK != glewInit())
    {
        std::cerr << "Couldn't init glew:(\n";
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);

    // Initialize camera
    vec3 start_pos = {0.0f, 0.0f, 3.0f};
    Camera camera(start_pos);
    camera_ptr = &camera; // Assign pointer for mouse callback

    // Set input mode and callbacks
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    Renderer r("shaders/shader.vert", "shaders/shader.frag", screenWidth, screenHeight, mode, distance);

    std::optional<Model> model = load_obj(argv[1]);

    if (!model.has_value())
    {
        std::cout << "Couldn't load file: " << argv[1] << '\n';
        glfwTerminate();
        return 0;
    }

    r.add_model(std::move(model.value()));

    double previous_time = glfwGetTime();
    int ticks = 0;

    double lastFrame = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        double currentFrame = glfwGetTime();
        float delta_time = float(currentFrame - lastFrame);
        lastFrame = currentFrame;

        // Process camera keyboard input
        camera.processKeyboard(window, delta_time);

        // Update camera view matrix
        camera.updateViewMatrix();

        // Pass updated view matrix to renderer
        r.setViewMatrix(&camera.view_matrix[0][0]);

        r.draw_models();

        glfwSwapBuffers(window);

        if (print_fps)
        {
            double current_time = glfwGetTime();
            double delta = current_time - previous_time;
            std::cout << "Fps: " << 1 / delta << '\n';
            std::cout << "Time: " << delta << '\n';
            std::cout << "Ticks: " << ticks << '\n';
            std::cout << '\n';
            previous_time = current_time;
            ticks += 1;
        }
    }

    glfwTerminate();
    return 0;
}



