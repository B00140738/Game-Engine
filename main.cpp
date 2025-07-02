#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lib/imgui/backends/imgui_impl_glfw.h"
#include "lib/imgui/backends/imgui_impl_opengl3.h"
#include "lib/imgui/imgui.h"

#include "src/ButtonWidget.hpp"
#include "src/CollapsibleSectionWidget.hpp"
#include "src/ConsoleWidget.hpp"

#include "src/Camera.hpp"
#include "src/Menu.hpp"
#include "src/Mesh.hpp"
#include "src/Model.hpp"
#include "src/Renderer.hpp"
#include "src/load_obj.hpp"

const GLint WIDTH = 1200, HEIGHT = 800;

bool is_float(const std::string &str) {
  std::istringstream iss(str);
  float f;
  iss >> std::noskipws >> f;
  return iss.eof() && !iss.fail();
}

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;
Camera *camera_ptr = nullptr;
bool camera_mode = true;

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (!camera_ptr || !camera_mode)
    return;

  if (firstMouse) {
    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);
    firstMouse = false;
  }

  float xoffset = static_cast<float>(xpos) - lastX;
  float yoffset = lastY - static_cast<float>(ypos);
  lastX = static_cast<float>(xpos);
  lastY = static_cast<float>(ypos);

  camera_ptr->processMouseOffset(xoffset, yoffset);
}

int main(int argc, char **argv) {
  int mode = 0;
  bool print_fps = false;

  if (argc < 4) {
    std::cout << "usage: ./main (obj_file) (GL_POINTS or GL_TRIANGLES or GL_LINES) (distance) (optional: fps)\n";
    return 0;
  }

  std::string_view chosen_render_mode(argv[2]);

  if (chosen_render_mode == "GL_POINTS")
    mode = GL_POINTS;
  else if (chosen_render_mode == "GL_TRIANGLES")
    mode = GL_TRIANGLES;
  else if (chosen_render_mode == "GL_LINES")
    mode = GL_LINES;
  else {
    std::cout << "Invalid render mode, must be: GL_POINTS, GL_TRIANGLES or GL_LINES\n";
    return 0;
  }

  if (!is_float(argv[3])) {
    std::cout << "Invalid distance, must be a number\n";
    return 0;
  }

  if (argc == 5)
    print_fps = true;

  float distance = std::stof(argv[3]);

  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW\n";
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "OGL", nullptr, nullptr);
  if (!window) {
    std::cerr << "Couldn't make window\n";
    glfwTerminate();
    return -1;
  }

  int screenWidth, screenHeight;
  glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

  glfwMakeContextCurrent(window);
  glewExperimental = GL_TRUE;
  if (GLEW_OK != glewInit()) {
    std::cerr << "Couldn't init glew\n";
    glfwTerminate();
    return -1;
  }

  glViewport(0, 0, screenWidth, screenHeight);
  glEnable(GL_DEPTH_TEST);

  vec3 start_pos = {0.0f, 0.0f, 3.0f};
  Camera camera(start_pos);
  camera_ptr = &camera;

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO(); (void)io;
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  Renderer renderer("shaders/shader.vert", "shaders/shader.frag", screenWidth, screenHeight, mode, distance);

  std::optional<Model> model = load_obj(argv[1]);
  if (!model.has_value()) {
    std::cout << "Couldn't load file: " << argv[1] << '\n';
    glfwTerminate();
    return 0;
  }
  renderer.add_model(std::move(model.value()));

  // Stats widget
  class StatsWidget : public GUI::Widget {
  public:
    double &fps;
    double &time;
    int &ticks;
    StatsWidget(double &fps_, double &time_, int &ticks_)
        : fps(fps_), time(time_), ticks(ticks_) {}
    void Render() override {
      ImGui::Text("FPS: %.1f", fps);
      ImGui::Text("Time: %.2f", time);
      ImGui::Text("Ticks: %d", ticks);
    }
  };

  // Console widget
  class ConsoleWidget : public GUI::Widget {
  public:
    std::vector<std::string> logs;
    void AddLog(const std::string &log) { logs.push_back(log); }
    void Render() override {
      ImGui::BeginChild("ConsoleRegion", ImVec2(0, 150), true);
      for (const auto &log : logs)
        ImGui::TextUnformatted(log.c_str());
      ImGui::EndChild();
    }
  };

  double fps_val = 0.0;
  double time_val = 0.0;
  int ticks = 0;

  GUI::Menu left_menu("Game engine menu");
  auto stats_widget = std::make_shared<StatsWidget>(fps_val, time_val, ticks);
  left_menu.AddWidget(stats_widget);

  ConsoleWidget console_widget;
  GUI::Menu bottom_console("Console", nullptr);
  bottom_console.AddWidget(std::make_shared<ConsoleWidget>(console_widget));

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    double currentFrame = glfwGetTime();
    float delta_time = static_cast<float>(currentFrame - time_val);
    time_val = currentFrame;

    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
      if (camera_mode) {
        camera_mode = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstMouse = true;
      } else {
        camera_mode = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      }
    }

    if (camera_mode)
      camera.processKeyboard(window, delta_time);

    camera.updateViewMatrix();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Top menu bar
    if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("Camera")) {
        if (ImGui::MenuItem("Toggle Camera Mode", NULL, camera_mode)) {
          camera_mode = !camera_mode;
          if (camera_mode)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
          else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
          firstMouse = true;
        }
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Settings")) {
        ImGui::MenuItem("Placeholder", NULL, false, false);
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }

    // Render left menu (locked left panel)
    left_menu.Render();

    // Render bottom console window
    ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y - 150));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, 150));
    ImGuiWindowFlags console_flags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
    ImGui::Begin("Console", nullptr, console_flags);
    console_widget.Render();
    ImGui::End();

    // Render OpenGL scene
    renderer.setViewMatrix(&camera.view_matrix[0][0]);
    renderer.draw_models();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

    if (print_fps) {
      fps_val = 1.0 / delta_time;
      ticks++;
      std::ostringstream oss;
      oss << "FPS: " << fps_val << ", Time: " << time_val << ", Ticks: " << ticks;
      console_widget.AddLog(oss.str());
    }
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
  return 0;
}
