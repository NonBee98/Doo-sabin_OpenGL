#include "subdivision.hpp"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif
#include <GLFW/glfw3.h>
#include <cstdlib>

#ifdef _WIN32
#pragma comment(lib, "OpenGL32.lib")
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef near
#undef far
#undef max
#undef min
#include <GL/gl.h>
#include <GL/glu.h>
#endif

int gFramebufferWidth = 1280;
int gFramebufferHeight = 720;
const char *gWindowTitle = "Task2-3";

void draw_face(Polygon face)
{
    glBegin(GL_POLYGON);
    for (int i = 0; i < face.size(); i++)
    {
        glVertex3fv(value_ptr(face[i]));
    }
    glEnd();
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    gFramebufferWidth = width;
    gFramebufferHeight = height;
}

float p_start = 0;
int one_circle = 120; // time for run one circle
void render(FACES new_faces)
{
    glViewport(0, 0, gFramebufferWidth, gFramebufferHeight);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 16 / 9.f, 0.5f, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0, 0, -5);
    glRotatef(p_start, 1, 0, 0);
    glRotatef(p_start, 0, 1, 0);
    

    for (int i = 0; i < new_faces.size(); i++)
    {
        glColor3ub((i+1) * 30 % 255, (i+1)*10 % 255, (i+1)*20 % 255);
        draw_face(new_faces[i]);
    
    }

    p_start += 360.0f / one_circle / 60;
}


int main(void)
{
    load_faces();
    int round_times = 1; // the round of subdivision
    FACES new_faces = subdivison(faces,1);

    GLFWwindow *window = nullptr;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(
        gFramebufferWidth, gFramebufferHeight, gWindowTitle,
        nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwGetFramebufferSize(window, &gFramebufferWidth, &gFramebufferHeight);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render(new_faces);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}