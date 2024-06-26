/*
 * Minecraft clone game.
 *
 * Developed by Gustavo Zille.
 */

// #define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION

#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Application.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
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
    g_CurrTime = (float)glfwGetTime();
    g_FramesCounter += 1;

    float delta = g_CurrTime - g_LastTime;

    if (delta >= 1.0f / 30.0f)
    {
        float FPS = (1.0f / delta) * g_FramesCounter;
        float ms = (delta / g_FramesCounter) * 1000.0f;

        std::string newTitle = "ZCraft - [" + std::to_string(int(FPS)) + " FPS / " + std::to_string(ms) + " ms]";

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

    // glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(g_ScreenWidth, g_ScreenHeight, "ZCraft", NULL, NULL);

    if (!window)
    {
        std::cout << "[ERROR] Failed to create GLFW window." << std::endl;

        return -1;
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow* parallelContext = glfwCreateWindow(g_ScreenWidth, g_ScreenHeight, "", NULL, window);

    if (!parallelContext)
    {
        std::cout << "[ERROR] Failed to create GLFW parallel context." << std::endl;

        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(0); // Remove FPS limit.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "[ERROR] Failed to initialize GLAD." << std::endl;

        return -1;
    }

    glViewport(0, 0, g_ScreenWidth, g_ScreenHeight);

    g_GameApp.Setup();

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();

        g_DeltaTime = currentFrame - g_LastFrame;
        g_LastFrame = currentFrame;

        showFramesPerSecond(window);

        glfwPollEvents();

        g_GameApp.Update(g_DeltaTime);
        g_GameApp.ProcessInput(g_DeltaTime);
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

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            g_GameApp.SetMouseButtonState(button, true);
        }
        else if (action == GLFW_RELEASE)
        {
            g_GameApp.SetMouseButtonState(button, false);
        }
    }
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    g_GameApp.SetMousePosition((float)xpos, (float)ypos);
}
