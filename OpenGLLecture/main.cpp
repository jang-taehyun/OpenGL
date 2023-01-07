#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// window size setting
const int WIN_WIDTH = 800;
const int WIN_HEIGHT = 600;

// polygon setting
const int vertexNum = 12;
const int dimension = 3;
const int triangleNum = 2;

// shader file
struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragSource;
};

// error handling
#define ASSERT(x) if((!x)) __debugbreak();       // __debugbreak()는 MSVC에만 사용 가능
#define GLCALL(x) GLClearError();\
                  x;\
                  ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
    // glGetError()는 error를 하니씩만 반환하기 때문에, 한 번에 모든 error를 뽑아내는 것이 필요함
    while (glGetError() != GL_NO_ERROR);         // GL_NO_ERROR == 0
}
static bool GLLogCall(const char* function, const char* file, int line)
{
    
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << ") : " << function << " "
            << file << " in line " << line << std::endl;
        return false;
    }
    return true;
}

// shader parsing function
static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

// shader compile function
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    // shader object 생성
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();

    // shader source code 해석 방법 & shader source code를 GPU에 전달
    glShaderSource(id, 1, &src, nullptr);

    // shader source code 컴파일
    glCompileShader(id);

    // Error Handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);              //셰이더 프로그램으로부터 컴파일 결과(log)를 얻어옴
    if (result == GL_FALSE)                                     //컴파일에 실패한 경우
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);         //log의 길이를 얻어옴
        char* message = (char*)alloca(length * sizeof(char));   //stack에 동적할당
        glGetShaderInfoLog(id, length, &length, message);       //길이만큼 log를 얻어옴
        std::cout << "셰이더 컴파일 실패! " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);                                     //컴파일 실패한 경우 셰이더 삭제
        return 0;
    }

    // shader id return
    return id;
}

// shader program 생성 function
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragShader)
{
    // shader program object 생성 및 shader complie
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragShader);

    // compile된 shader를 shader program에 추가 및 링크
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // shader program이 현재 state에서 동작 가능한지 확인
    glValidateProgram(program);

    // shader 반환
    glDeleteShader(vs);
    glDeleteShader(fs);

    // shader program id return
    return program;
}

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

    // GLEW 초기화(항상 glfwMakeContextCurrent()가 호출된 후에 glewInit()을 수행할 것)
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Initializing GLEW is failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // OpenGL version 출력
    std::cout << glGetString(GL_VERSION) << std::endl;

    // 사각형 정의(현재 CPU side memory에 존재)
    float position[vertexNum] =
    {
        -0.5f, -0.5f, 0.f,          // 0번 vertex
         0.5f, -0.5f, 0.f,          // 1번 vertex
         0.5f,  0.5f, 0.f,          // 2번 vertex
        -0.5f,  0.5f, 0.f           // 3번 vertex
    };

    // 하나의 triangle을 이루고 있는 vertex들을 기술(CCW 순서로 기술)
    unsigned int indices[triangleNum * 3] =
    {
        0, 1, 2,
        2, 3, 0
    };

    // vertex array buffer 생성 및 data을 GPU에 전달
    unsigned int bufferID;
    glGenBuffers(1, &bufferID);                                                         // buffer 생성
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);                                            // 생성한 buffer의 종류 명시 & bind
    glBufferData(GL_ARRAY_BUFFER, vertexNum * sizeof(float), position, GL_STATIC_DRAW); // vertex array buffer에 저장된 삼각형 data을 GPU에 전달

    // index array buffer 생성 및 data를 GPU에 전달
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // buffer type
        triangleNum * 3 * sizeof(unsigned int),     // 전달할 data 크기
        indices,                                    // data가 저장된 포인터
        GL_STATIC_DRAW);                            // data 사용 패턴
    
    

    // GPU에 저장된 data 저장 위치, data 해석 방법 설정
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * dimension, 0);

    // shader source parsing
    ShaderProgramSource source = ParseShader("resource/shaders/Basic.shader");

    // shader program 생성 및 shader program object ID 저장
    unsigned int shader = CreateShader(source.VertexSource, source.FragSource);

    // shader program bind(shader program을 activate 상태로 전환)
    glUseProgram(shader);      

    // Rendering loop
    while (!glfwWindowShouldClose(window))
    {
        // back buffer의 내용 clear
        glClear(GL_COLOR_BUFFER_BIT);

        // 현재 activatee된 것을 deactivate
        // glUseProgram(0);
        // -> activate function의 parameter에 0을 넣으면 현재 activate된 것이 deactivate 된다.

        // draw
        // glDrawArrays(GL_TRIANGLES, 0, 3);    // -> index array를 사용하면 glDrawElements()를 사용
        GLCALL(glDrawElements(GL_TRIANGLES,     // draw할 polygon 종류
                              triangleNum * 3,  // 그릴 index의 개수
                              GL_UNSIGNED_INT,  // index array의 data type
                              nullptr));        // offset 포인터
        // -> gl function call 중 error 발생 시, console 창에서 error 내용 확인 가능

        // swap front and back buffers
        glfwSwapBuffers(window);

        // keyboard, mouse input 처리
        glfwPollEvents();
    }

    // shader program 반환
    glDeleteProgram(shader);

    // GLFW resource 반환
    glfwTerminate();
    return 0;
}