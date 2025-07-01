#pragma once

#include <list>
#include <string>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.hpp"
#include "Shader.hpp"

// Include cglm for the mat4 typedef
extern "C" {
#include <cglm/cglm.h>
}

/**
 * @brief Keep track of models and render them
 *
 */
class Renderer
{
public:
    std::list<std::unique_ptr<Model>> models;
    Shader shader;
    glm::mat4 projection;
    glm::mat4 view;
    int mode;
    float distance;

    Renderer(const std::string &vertexPath,
             const std::string &fragmentPath,
             int screenWidth,
             int screenHeight,
             int mode,
             float distance)
        : shader(vertexPath, fragmentPath), projection(1.0f), view(1.0f), mode(mode), distance(distance)
    {
        projection = glm::perspective(glm::radians(45.0f), (GLfloat)screenWidth / screenHeight, 0.1f, 500.0f);
    }

    void add_model(Model m)
    {
        models.push_back(std::make_unique<Model>(std::move(m)));
    }

    void setViewMatrix(const float *camera_view_matrix_ptr)
    {
        // Convert cglm mat4 (float[4][4]) to glm::mat4
        view = glm::make_mat4(camera_view_matrix_ptr);
    }

    void draw_models()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 model(1.0f);
        model = glm::rotate(model, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));

        GLint modelLoc = glGetUniformLocation(shader.program, "model");
        GLint viewLoc = glGetUniformLocation(shader.program, "view");
        GLint projectionLoc = glGetUniformLocation(shader.program, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        for (const auto &model : models)
        {
            glBindVertexArray(model->m_vao);
            glDrawElements(mode, model->m_mesh.indices.size(), GL_UNSIGNED_SHORT, 0);
            glBindVertexArray(0);
        }
    }
};

