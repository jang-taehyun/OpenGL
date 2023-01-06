#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// window size setting
const int WIN_WIDTH = 800;
const int WIN_HEIGHT = 600;

// shader compile function
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    // shader object 생성
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();

    // shader source code 해석 방법 & shader source code를 GPU에 전달
    glShaderSource(id,          // complie된 shader의 id를 저장할 위치
                   1,           // compile할 source code의 개수
                   &src,        // 실제 source code가 있는 문자열 주소값
                   nullptr);    // 해당 문자열 전체를 사용(nullptr), 아니라면 길이 명시

    // shader source code 컴파일
    glCompileShader(id);

    // Error Handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); //셰이더 프로그램으로부터 컴파일 결과(log)를 얻어옴
    if (result == GL_FALSE) //컴파일에 실패한 경우
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); //log의 길이를 얻어옴
        char* message = (char*)alloca(length * sizeof(char)); //stack에 동적할당
        glGetShaderInfoLog(id, length, &length, message); //길이만큼 log를 얻어옴
        std::cout << "셰이더 컴파일 실패! " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id); //컴파일 실패한 경우 셰이더 삭제
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

    // GLEW 초기화
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Initializing GLEW is failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // OpenGL version 출력
    std::cout << glGetString(GL_VERSION) << std::endl;

    // 삼각형 정의(현재 CPU side memory에 존재)
    float position[] =
    {
        -0.5f, -0.5f, 0.f,
         0.f,   0.5f, 0.f,
         0.5f, -0.5f, 0.f
    };

    // buffer 생성
    unsigned int bufferID;
    glGenBuffers(1, &bufferID);                     // buffer 생성
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);        // 생성한 buffer의 종류 명시 & bind
    
    // buffer에 저장된 삼각형 data을 GPU에 전달
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), position, GL_STATIC_DRAW);

    // -------------- buffer에 저장된 data 해석 방법 명시 --------------- //
    // glEnableVertexAttribArray의 parameter와 glVertexAttribPointer의 parameter 1의 의미 : vertex shader에 저장할 location

    // data 저장 위치 설정
    glEnableVertexAttribArray(0);                   // vertex shader의 0번 location에 저장

    // data를 어떻게 나눠서 해석할지 설정
    glVertexAttribPointer(0,                        // 해석할 data가 저장된 location(glEnableVertexAttribArray에서 설정한 location)
                          3,                        // 하나의 vertex에 몇개의 data를 넘기는지(vertex data 중 넘길 data의 개수), 여기에선 위치정보(xyz 좌표)만 있으므로 3
                          GL_FLOAT,                 // data의 자료형
                          GL_FALSE,                 // normalization 유무
                          sizeof(float) * 3,        // stride : vertex 하나의 크기(byte 단위)
                          0);                       // offset : 시작할 index 번호

    // -------------- shader 생성 및 compile --------------- //
    // GLSL 언어로 shader source code 작성
    std::string vertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;\n"      // 여기 있는 location = 0가, glEnableVertexAttribArray의 parameter와 glVertexAttribPointer의 parameter 1의 의미함
                                                        // input이 들어오기 때문에 in 키워드 사용 & position 부분은 아무 이름으로 해도 괜찮음
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = position;\n"                 // 3개의 값만 전달했지만, 알아서 vec4로 변환
        "}";
    std::string fragShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;\n"        // 출력 color 
        "\n"
        "void main()\n"
        "{\n"
        "    color = vec4(1.f, 1.f, 0.f, 1.f);\n"       // 노란색 변환(각 vertex마다 어떤 색을 출력할지 결정)
        "}";

    // shader program 생성 및 shader program object ID 저장
    unsigned int shader = CreateShader(vertexShader, fragShader);

    // shader program bind(shader program을 activate 상태로 전환)
    glUseProgram(shader);      

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

    // shader program 반환
    glDeleteProgram(shader);

    // GLFW resource 반환
    glfwTerminate();
    return 0;
}