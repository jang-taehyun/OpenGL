/*
# ���� ����Ʈ
OpenGL tutorial : https://www.opengl-tutorial.org/kr/
OpenGL API : https://docs.gl/
��Ʃ�� ���� ����Ʈ : https://www.youtube.com/TheChernoProject/video
�̷п� ���� ����Ʈ : https://www.songho.ca/opengl/index.html

# �ڵ� ���� github ����Ʈ : https://github.com/diskhkme/OpenGL_Lecture_Material
- commit�� ������ �������� ����
- '+' �κ� : �߰��� �κ�
  '-' �κ� : ���ŵ� �κ�

# function�� parameter�� �뵵�� OpenGL API�� ����
# �ڵ��� �帧�� �ľ��ϴ°� �켱 �߿�
*/

/*
* �̷�
3D ��ǻ�� �׷��Ƚ� : obejct�� 3D�� ǥ��

rendering : �̹����� ������ ����
rendering ���� : geometry(����) -> texture(����) -> lighting(���� ���� �� ����)

������ �ʿ��� data : ��, ī�޶�, ��ü
-> �� data���� ������ �ȼ��� color data�� ����Ѵ�.

graphics pipeline�� ���������� ó���Ѵ�.
vertex data, primitive data -> tessellation -> vertex processing -> geometry processing -> pixel processing -> pixel rendering
											   textrured surface ->	texture sampler ----->

��ȭ, �ִϸ��̼� -> ��ǽð� ��ǻ�� �׷��Ƚ�(30fps ����)
���� -> �ǽð� ��ǻ�� �׷��Ƚ�(30fps �̻�)

��ǻ�� �׷��Ƚ� �ֿ� �ܰ� : modeling -> rigging -> animation -> rendering -> post-processing
-> �ʿ��� �ڿ� : modeling, rigging, animation(art ����)
-> run-time�� �ϴ� ���� : rendering, post-processing(���� ����)

- modeling : ��ǻ�Ͱ� ��ü�� ǥ���� ��(geometry, ����)
  -> polygon mesh ��� : polygon mesh�� ������ ���
     -> polygon mesh �� triangle ���
  -> texture ��� : model�� ����(������ ��)�� �����µ� ���
     -> texture�� �̹��� data�� �����Ѵ�.

- rigging(skeleton) : model�� ���� �����ϴ� ����
  -> polygon mesh�� �Բ� rigging �۾��� �Ѵ�.

- animation : Ư�� event �߻� ��, �߻��ϴ� animation
  -> keyframe animation�� ����Ѵ�.(Quaternion)

- rendering : 3D scene(3D data)�� �̟G�� 2D imgae�� ����� ����
- post-processing(��ó��) : 1 frame�� ó���� �� ���� ���� ó���ϴ� ����, rendering�� ó���� �� ���� ���� ó��
  ex) motion blur(�����̰� �ִ� ��ü�� �帮�� ǥ���ϴ� ��)

# Graphics API : GPU �ȿ� �Ͼ�� instruction�� ȣ���ϴ� ��


* �Ǳ�
GLFW : window�� ���õ� ó���� ���� ����� �� �ֵ��� ������ library
-> WinAPI�� ����� �� ������, �ð��� ���� �ɸ��� ������ ���
-> window's context : OpenGL�� ������� �׸� window

GLEW : OpenGL�� �ֽ� ������ ����ϱ� ���� �ʿ��� library

State-Machine ��� : ���� activate�� �͸� �׸��� ���
- ���� : object ���� -> object�� activate ���·� ���� -> draw function ȣ��
- bind : buffer�� activate ���·� �����ϴ� ��

���ξ glfw : glfw library�� �ִ� function
���ξ gl   : OpenGL API�� �ִ� function
-> �������� ���õ� function�� ��κ� gl�� �����Ѵ�.

Double-buffering ��� : back buffer�� ���� �׸��� front buffer�� swap�ϴ� ���

glBegin() �Ǵ� glEnd() function�� �����ϴ� �ڵ�� ������ version�̴�.(Legacy �ڵ�)
-> shader�� ���� �� �˾Ƽ� ������� �׸����� �ϴ� function
*/

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// window size setting
const int WIN_WIDTH = 800;
const int WIN_HEIGHT = 600;

int main()
{
    // GLFW �ʱ�ȭ
    GLFWwindow* window;
    if (!glfwInit())
    {
        std::cout << "GLFW initialize falied!" << std::endl;
        return -1;
    }

    // window ����
    window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "OpenGL Lecture", NULL, NULL);
    if (!window)
    {
        std::cout << "Creating window is failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // window's context ����
    glfwMakeContextCurrent(window);

    // GLEW �ʱ�ȭ
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Initializing GLEW is failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // OpenGL version ���
    std::cout << glGetString(GL_VERSION) << std::endl;

    // �ﰢ�� ����
    float position[] =
    {
        -0.5f, -0.5f,
         0.f,   0.5f,
         0.5f, -0.5f
    };

    // buffer ����
    unsigned int bufferID;
    glGenBuffers(1, &bufferID);                     // buffer ����
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);        // ������ buffer�� ���� ��� & bind
    
    // buffer�� �ﰢ�� data ����
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), position, GL_STATIC_DRAW);

    // Rendering loop
    while (!glfwWindowShouldClose(window))
    {
        // back buffer�� ���� clear
        glClear(GL_COLOR_BUFFER_BIT);

        // draw
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // swap front and back buffers
        glfwSwapBuffers(window);

        // keyboard, mouse input ó��
        glfwPollEvents();
    }

    // resource ��ȯ
    glfwTerminate();
    return 0;
}

// ���� test�ӵ�