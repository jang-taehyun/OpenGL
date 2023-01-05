/*
# 참고 사이트
OpenGL tutorial : https://www.opengl-tutorial.org/kr/
OpenGL API : https://docs.gl/
유튜브 참고 사이트 : https://www.youtube.com/TheChernoProject/video
이론에 관한 사이트 : https://www.songho.ca/opengl/index.html

# 코드 참고 github 사이트 : https://github.com/diskhkme/OpenGL_Lecture_Material
- commit을 눌러서 차근차근 보자
- '+' 부분 : 추가된 부분
  '-' 부분 : 제거된 부분

# function의 parameter와 용도는 OpenGL API를 참고
# 코드의 흐름을 파악하는게 우선 중요
*/

/*
* 이론
3D 컴퓨터 그래픽스 : obejct를 3D로 표현

rendering : 이미지를 만들어내는 과정
rendering 과정 : geometry(형상) -> texture(색상) -> lighting(빛의 방향 및 세기)

렌더링 필요한 data : 빛, 카메라, 물체
-> 이 data들을 조합해 픽셀의 color data를 계산한다.

graphics pipeline를 병렬적으로 처리한다.
vertex data, primitive data -> tessellation -> vertex processing -> geometry processing -> pixel processing -> pixel rendering
											   textrured surface ->	texture sampler ----->

영화, 애니메이션 -> 비실시간 컴퓨터 그래픽스(30fps 이하)
게임 -> 실시간 컴퓨터 그래픽스(30fps 이상)

컴퓨터 그래픽스 주요 단계 : modeling -> rigging -> animation -> rendering -> post-processing
-> 필요한 자원 : modeling, rigging, animation(art 영역)
-> run-time에 하는 동작 : rendering, post-processing(개발 영역)

- modeling : 컴퓨터가 물체를 표현한 것(geometry, 형상)
  -> polygon mesh 사용 : polygon mesh의 집합을 사용
     -> polygon mesh 중 triangle 사용
  -> texture 사용 : model의 색상(입혀진 것)을 입히는데 사용
     -> texture는 이미지 data로 저장한다.

- rigging(skeleton) : model에 뼈를 정의하는 과정
  -> polygon mesh와 함께 rigging 작업을 한다.

- animation : 특정 event 발생 시, 발생하는 animation
  -> keyframe animation을 사용한다.(Quaternion)

- rendering : 3D scene(3D data)을 이욯해 2D imgae로 만드는 과정
- post-processing(후처리) : 1 frame에 처리할 수 없는 것을 처리하는 과정, rendering에 처리할 수 없는 것을 처리
  ex) motion blur(움직이고 있는 물체를 흐리게 표현하는 것)

# Graphics API : GPU 안에 일어나는 instruction을 호출하는 것


* 실기
GLFW : window와 관련된 처리를 쉽게 사용할 수 있도록 구현한 library
-> WinAPI를 사용할 수 있지만, 시간이 오래 걸리기 때문에 사용
-> window's context : OpenGL의 결과물을 그릴 window

GLEW : OpenGL의 최신 버전을 사용하기 위해 필요한 library

State-Machine 방식 : 현재 activate된 것만 그리는 방식
- 과정 : object 정의 -> object를 activate 상태로 설정 -> draw function 호출
- bind : buffer를 activate 상태로 설정하는 것

접두어가 glfw : glfw library에 있는 function
접두어가 gl   : OpenGL API에 있는 function
-> 렌더링과 관련된 function은 대부분 gl로 시작한다.

Double-buffering 사용 : back buffer를 먼저 그리고 front buffer와 swap하는 방식

glBegin() 또는 glEnd() function이 존재하는 코드는 오래된 version이다.(Legacy 코드)
-> shader가 없을 떄 알아서 흰색으로 그리도록 하는 function
*/

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// window size setting
const int WIN_WIDTH = 800;
const int WIN_HEIGHT = 600;

int main()
{
    // GLFW 초기화
    GLFWwindow* window;
    if (!glfwInit())
    {
        std::cout << "GLFW initialize falied!" << std::endl;
        return -1;
    }

    // window 생성
    window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "OpenGL Lecture", NULL, NULL);
    if (!window)
    {
        std::cout << "Creating window is failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // window's context 설정
    glfwMakeContextCurrent(window);

    // GLEW 초기화
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Initializing GLEW is failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // OpenGL version 출력
    std::cout << glGetString(GL_VERSION) << std::endl;

    // 삼각형 정의
    float position[] =
    {
        -0.5f, -0.5f,
         0.f,   0.5f,
         0.5f, -0.5f
    };

    // buffer 생성
    unsigned int bufferID;
    glGenBuffers(1, &bufferID);                     // buffer 생성
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);        // 생성한 buffer의 종류 명시 & bind
    
    // buffer에 삼각형 data 전달
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), position, GL_STATIC_DRAW);

    // Rendering loop
    while (!glfwWindowShouldClose(window))
    {
        // back buffer의 내용 clear
        glClear(GL_COLOR_BUFFER_BIT);

        // draw
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // swap front and back buffers
        glfwSwapBuffers(window);

        // keyboard, mouse input 처리
        glfwPollEvents();
    }

    // resource 반환
    glfwTerminate();
    return 0;
}

// 수정 test임돠