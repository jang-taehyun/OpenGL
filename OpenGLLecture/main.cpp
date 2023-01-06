#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// window size setting
const int WIN_WIDTH = 800;
const int WIN_HEIGHT = 600;

// shader compile function
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    // shader object ����
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();

    // shader source code �ؼ� ��� & shader source code�� GPU�� ����
    glShaderSource(id,          // complie�� shader�� id�� ������ ��ġ
                   1,           // compile�� source code�� ����
                   &src,        // ���� source code�� �ִ� ���ڿ� �ּҰ�
                   nullptr);    // �ش� ���ڿ� ��ü�� ���(nullptr), �ƴ϶�� ���� ���

    // shader source code ������
    glCompileShader(id);

    // Error Handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); //���̴� ���α׷����κ��� ������ ���(log)�� ����
    if (result == GL_FALSE) //�����Ͽ� ������ ���
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); //log�� ���̸� ����
        char* message = (char*)alloca(length * sizeof(char)); //stack�� �����Ҵ�
        glGetShaderInfoLog(id, length, &length, message); //���̸�ŭ log�� ����
        std::cout << "���̴� ������ ����! " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id); //������ ������ ��� ���̴� ����
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

    // GLEW �ʱ�ȭ
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Initializing GLEW is failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // OpenGL version ���
    std::cout << glGetString(GL_VERSION) << std::endl;

    // �ﰢ�� ����(���� CPU side memory�� ����)
    float position[] =
    {
        -0.5f, -0.5f, 0.f,
         0.f,   0.5f, 0.f,
         0.5f, -0.5f, 0.f
    };

    // buffer ����
    unsigned int bufferID;
    glGenBuffers(1, &bufferID);                     // buffer ����
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);        // ������ buffer�� ���� ��� & bind
    
    // buffer�� ����� �ﰢ�� data�� GPU�� ����
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), position, GL_STATIC_DRAW);

    // -------------- buffer�� ����� data �ؼ� ��� ��� --------------- //
    // glEnableVertexAttribArray�� parameter�� glVertexAttribPointer�� parameter 1�� �ǹ� : vertex shader�� ������ location

    // data ���� ��ġ ����
    glEnableVertexAttribArray(0);                   // vertex shader�� 0�� location�� ����

    // data�� ��� ������ �ؼ����� ����
    glVertexAttribPointer(0,                        // �ؼ��� data�� ����� location(glEnableVertexAttribArray���� ������ location)
                          3,                        // �ϳ��� vertex�� ��� data�� �ѱ����(vertex data �� �ѱ� data�� ����), ���⿡�� ��ġ����(xyz ��ǥ)�� �����Ƿ� 3
                          GL_FLOAT,                 // data�� �ڷ���
                          GL_FALSE,                 // normalization ����
                          sizeof(float) * 3,        // stride : vertex �ϳ��� ũ��(byte ����)
                          0);                       // offset : ������ index ��ȣ

    // -------------- shader ���� �� compile --------------- //
    // GLSL ���� shader source code �ۼ�
    std::string vertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;\n"      // ���� �ִ� location = 0��, glEnableVertexAttribArray�� parameter�� glVertexAttribPointer�� parameter 1�� �ǹ���
                                                        // input�� ������ ������ in Ű���� ��� & position �κ��� �ƹ� �̸����� �ص� ������
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = position;\n"                 // 3���� ���� ����������, �˾Ƽ� vec4�� ��ȯ
        "}";
    std::string fragShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;\n"        // ��� color 
        "\n"
        "void main()\n"
        "{\n"
        "    color = vec4(1.f, 1.f, 0.f, 1.f);\n"       // ����� ��ȯ(�� vertex���� � ���� ������� ����)
        "}";

    // shader program ���� �� shader program object ID ����
    unsigned int shader = CreateShader(vertexShader, fragShader);

    // shader program bind(shader program�� activate ���·� ��ȯ)
    glUseProgram(shader);      

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

    // shader program ��ȯ
    glDeleteProgram(shader);

    // GLFW resource ��ȯ
    glfwTerminate();
    return 0;
}