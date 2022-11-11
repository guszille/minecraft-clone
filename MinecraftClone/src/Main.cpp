/*
 * Minecraft clone game.
 *
 * Developed by Gustavo Zille.
 */

#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION

#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Application.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

unsigned int g_ScreenWidth = 1600;
unsigned int g_ScreenHeight = 900;

float g_DeltaTime = 0.0f;
float g_LastFrame = 0.0f;

float g_CurrTime = 0.0f;
float g_LastTime = 0.0f;
unsigned int g_FramesCounter = 0;

Application g_GameApp(g_ScreenWidth, g_ScreenHeight);

void showFramesPerSecond(GLFWwindow* window)
{
    g_CurrTime = glfwGetTime();
    g_FramesCounter += 1;

    float diff = g_CurrTime - g_LastTime;

    if (diff >= 1.0f / 30.0f)
    {
        std::string FPS = std::to_string((int)((1.0f / diff) * g_FramesCounter));
        std::string ms = std::to_string((diff / g_FramesCounter) * 1000.0f);
        std::string newTitle = "Minecraft - [" + FPS + " FPS / " + ms + " ms]";

        glfwSetWindowTitle(window, newTitle.c_str());

        g_LastTime = g_CurrTime;
        g_FramesCounter = 0;
    }
}

int main(int argc, char* argv[])
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(g_ScreenWidth, g_ScreenHeight, "Minecraft", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSwapInterval(0); // Remove FPS limit.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!window)
    {
        std::cout << "[ERROR] Failed to create GLFW window." << std::endl;

        return -1;
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "[ERROR] Failed to initialize GLAD." << std::endl;

        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    glViewport(0, 0, g_ScreenWidth, g_ScreenHeight);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE); // Allows to enable/disable writing to the depth buffer.

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    g_GameApp.Setup();

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();

        g_DeltaTime = currentFrame - g_LastFrame;
        g_LastFrame = currentFrame;

        showFramesPerSecond(window);

        glfwPollEvents();

        g_GameApp.Update(g_DeltaTime);
        g_GameApp.ProcessInput(g_DeltaTime);

        glClearColor(0.45f, 0.65f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        g_GameApp.Render();

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    g_ScreenWidth = width;
    g_ScreenHeight = height;

    glViewport(0, 0, width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) // Window should close.
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            g_GameApp.SetKeyState(key, true);
        }
        else if (action == GLFW_RELEASE)
        {
            g_GameApp.SetKeyState(key, false);
        }
    }
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    g_GameApp.SetMousePosition(xpos, ypos);
}
