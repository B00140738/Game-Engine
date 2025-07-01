#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <cmath>
#include <cstring>  // For memcpy

class Camera {
public:
    vec3 position;
    float yaw;
    float pitch;
    float speed;
    float sensitivity;
    mat4 view_matrix;

    Camera(const vec3& start_position) {
        std::memcpy(position, start_position, sizeof(vec3));
        yaw = -90.0f;
        pitch = 0.0f;
        speed = 2.5f;
        sensitivity = 0.1f;
        glm_mat4_identity(view_matrix);
    }

    void processKeyboard(GLFWwindow* window, float delta_time) {
        float velocity = speed * delta_time;
        vec3 front = {cos(glm_rad(yaw)) * cos(glm_rad(pitch)), sin(glm_rad(pitch)),
                      sin(glm_rad(yaw)) * cos(glm_rad(pitch))};
        glm_vec3_normalize(front);

        vec3 right;
        glm_vec3_cross(front, (vec3){0.0f, 1.0f, 0.0f}, right);
        glm_vec3_normalize(right);

        vec3 temp;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            glm_vec3_scale(front, velocity, temp);
            glm_vec3_add(position, temp, position);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            glm_vec3_scale(front, velocity, temp);
            glm_vec3_sub(position, temp, position);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            glm_vec3_scale(right, velocity, temp);
            glm_vec3_sub(position, temp, position);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            glm_vec3_scale(right, velocity, temp);
            glm_vec3_add(position, temp, position);
        }
        // Move up when SPACE is pressed
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            vec3 up = {0.0f, 1.0f, 0.0f};
            glm_vec3_scale(up, velocity, temp);
            glm_vec3_add(position, temp, position);
        }
        // Quit when Q is pressed
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }

    void processMouseOffset(float xoffset, float yoffset) {
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    void updateViewMatrix() {
        vec3 front = {cos(glm_rad(yaw)) * cos(glm_rad(pitch)), sin(glm_rad(pitch)),
                      sin(glm_rad(yaw)) * cos(glm_rad(pitch))};
        glm_vec3_normalize(front);

        vec3 target;
        glm_vec3_add(position, front, target);

        glm_lookat(position, target, (vec3){0.0f, 1.0f, 0.0f}, view_matrix);
    }
};

#endif // CAMERA_HPP

