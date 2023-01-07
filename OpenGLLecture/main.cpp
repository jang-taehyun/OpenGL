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
#define ASSERT(x) if((!x)) __debugbreak();       // __debugbreak()�� MSVC���� ��� ����
#define GLCALL(x) GLClearError();\
                  x;\
                  ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
    // glGetError()�� error�� �ϴϾ��� ��ȯ�ϱ� ������, �� ���� ��� error�� �̾Ƴ��� ���� �ʿ���
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
    // shader object ����
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();

    // shader source code �ؼ� ��� & shader source code�� GPU�� ����
    glShaderSource(id, 1, &src, nullptr);

    // shader source code ������
    glCompileShader(id);

    // Error Handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);              //���̴� ���α׷����κ��� ������ ���(log)�� ����
    if (result == GL_FALSE)                                     //�����Ͽ� ������ ���
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);         //log�� ���̸� ����
        char* message = (char*)alloca(length * sizeof(char));   //stack�� �����Ҵ�
        glGetShaderInfoLog(id, length, &length, message);       //���̸�ŭ log�� ����
        std::cout << "���̴� ������ ����! " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);                                     //������ ������ ��� ���̴� ����
        return 0;
    }

    // shader id return
    return id;
}

// shader program ���� function
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragShader)
{
    // shader program object ���� �� shader complie
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragShader);

    // compile�� shader�� shader program�� �߰� �� ��ũ
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // shader program�� ���� state���� ���� �������� Ȯ��
    glValidateProgram(program);

    // shader ��ȯ
    glDeleteShader(vs);
    glDeleteShader(fs);

    // shader program id return
    return program;
}

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

    // GLEW �ʱ�ȭ(�׻� glfwMakeContextCurrent()�� ȣ��� �Ŀ� glewInit()�� ������ ��)
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Initializing GLEW is failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // OpenGL version ���
    std::cout << glGetString(GL_VERSION) << std::endl;

    // �簢�� ����(���� CPU side memory�� ����)
    float position[vertexNum] =
    {
        -0.5f, -0.5f, 0.f,          // 0�� vertex
         0.5f, -0.5f, 0.f,          // 1�� vertex
         0.5f,  0.5f, 0.f,          // 2�� vertex
        -0.5f,  0.5f, 0.f           // 3�� vertex
    };

    // �ϳ��� triangle�� �̷�� �ִ� vertex���� ���(CCW ������ ���)
    unsigned int indices[triangleNum * 3] =
    {
        0, 1, 2,
        2, 3, 0
    };

    // vertex array buffer ���� �� data�� GPU�� ����
    unsigned int bufferID;
    glGenBuffers(1, &bufferID);                                                         // buffer ����
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);                                            // ������ buffer�� ���� ��� & bind
    glBufferData(GL_ARRAY_BUFFER, vertexNum * sizeof(float), position, GL_STATIC_DRAW); // vertex array buffer�� ����� �ﰢ�� data�� GPU�� ����

    // index array buffer ���� �� data�� GPU�� ����
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // buffer type
        triangleNum * 3 * sizeof(unsigned int),     // ������ data ũ��
        indices,                                    // data�� ����� ������
        GL_STATIC_DRAW);                            // data ��� ����
    
    

    // GPU�� ����� data ���� ��ġ, data �ؼ� ��� ����
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * dimension, 0);

    // shader source parsing
    ShaderProgramSource source = ParseShader("resource/shaders/Basic.shader");

    // shader program ���� �� shader program object ID ����
    unsigned int shader = CreateShader(source.VertexSource, source.FragSource);

    // shader program bind(shader program�� activate ���·� ��ȯ)
    glUseProgram(shader);      

    // Rendering loop
    while (!glfwWindowShouldClose(window))
    {
        // back buffer�� ���� clear
        glClear(GL_COLOR_BUFFER_BIT);

        // ���� activatee�� ���� deactivate
        // glUseProgram(0);
        // -> activate function�� parameter�� 0�� ������ ���� activate�� ���� deactivate �ȴ�.

        // draw
        // glDrawArrays(GL_TRIANGLES, 0, 3);    // -> index array�� ����ϸ� glDrawElements()�� ���
        GLCALL(glDrawElements(GL_TRIANGLES,     // draw�� polygon ����
                              triangleNum * 3,  // �׸� index�� ����
                              GL_UNSIGNED_INT,  // index array�� data type
                              nullptr));        // offset ������
        // -> gl function call �� error �߻� ��, console â���� error ���� Ȯ�� ����

        // swap front and back buffers
        glfwSwapBuffers(window);

        // keyboard, mouse input ó��
        glfwPollEvents();
    }

    // shader program ��ȯ
    glDeleteProgram(shader);

    // GLFW resource ��ȯ
    glfwTerminate();
    return 0;
}