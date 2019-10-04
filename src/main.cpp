#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

void key_callback (GLFWwindow*,int,int,int,int);

const GLchar * vertex_shader_source =   "#version 330 core\n"
                                        "layout (location = 0) in vec3 position;\n" //layout (location = n) означает, что буфер, который 
                                                                                    //обределялся до следующего вызова glEnableVertexAttribArray(n) будет попадать в эту переменную.
                                                                                    //так можно передавать цвет и прочие параметры вершины.
                                        "out vec4 vertexColor;\n"
                                        "void main(){\n"
                                        "   gl_Position = vec4(position, 1.0);\n"
                                        "   vertexColor = vec4(0.7f, 0.1f, 0.9f, 1.0f);\n"
                                        "}\n\0";
const GLchar * fragment_sharer_source = "#version 330 core\n"
                                        "out vec4 color;\n"
                                        "in vec4 vertexColor;\n"
                                        "void main(){\n"
                                        "   color = vertexColor + vec4(0.0f, 0.2f, 0.2f, 0.0f);\n"
                                        "}\n\0";

const GLchar * fragment_blue_shader_source = "#version 330 core\n"
                                        "out vec4 color;\n"
                                        "in vec4 vertexColor;\n"
                                        "void main(){\n"
                                        "   color = vertexColor + vec4(0.2f, 0.5f, 1.0f, 0.0f);\n"
                                        "}\n\0";

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow * window = glfwCreateWindow(800, 600, "OpenGL Learning", nullptr, nullptr);
    if (window == nullptr){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK){
        std::cout << "GLEW init is not OK" << std::endl;
        return -1;
    }
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSetKeyCallback(window, key_callback);

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertex_shader, sizeof(infoLog), NULL, infoLog);
        std::cout << "VERTEX SHADER COMPILE FAILED\n" << infoLog << std::endl;
    }
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_sharer_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragment_shader, sizeof(infoLog), NULL, infoLog);
        std::cout << "FRAGMENT SHADER COMPILE FAILED\n" << infoLog << std::endl;
    }
    GLuint fragment_blue_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_blue_shader, 1, &fragment_blue_shader_source, NULL);
    glCompileShader(fragment_blue_shader);
    glGetShaderiv(fragment_blue_shader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragment_blue_shader, sizeof(infoLog), NULL, infoLog);
        std::cout << "FRAGMENT BLUE SHADER COMPILE FAILED\n" << infoLog << std::endl;
    }
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    
    GLuint shader_program1 = glCreateProgram();
    glAttachShader(shader_program1, vertex_shader);
    glAttachShader(shader_program1, fragment_blue_shader);
    glLinkProgram(shader_program1);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteShader(fragment_blue_shader);


    GLfloat vertices[] = {
        0.6f, 0.4f, 0.0f,  
        -0.1f, -0.7f, 0.0f,  
        -0.6f, 0.1f, 0.0f,  
        -0.3f, 0.5f, 0.0f,  
        0.4f, 0.8f, 0.0f,  
        0.4f, -0.4f, 0.0f
    };
    int points_count = sizeof(vertices)/sizeof(GLfloat)/3;

    GLuint indices[] = {
        0, 1, 2,
        3, 4, 5
    };

    GLuint VBO[2], EBO[2], VAO[2];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(2, EBO);
    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)/2, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glBindVertexArray(VAO[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)/2, &(vertices[(points_count/2)*3]), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);



    glBindVertexArray(0);
    
    GLfloat vertices_post[] = {
        -0.1f, -0.7f, 0.0f,  //1
        0.4f, -0.4f, 0.0f,//5
        0.1f, -0.9f, 0.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_post), vertices_post, GL_STATIC_DRAW);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
        glClearColor(0.1f, 0.4f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shader_program);
        glBindVertexArray(VAO[1]);

        glDrawArrays(GL_TRIANGLES, 0, points_count/2);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(shader_program1);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, points_count/2);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);
    glDeleteBuffers(2, EBO);

    glfwTerminate();
    return 0;
}

void key_callback (GLFWwindow* window, int key, int scancode, int action, int mode){
    if(action == GLFW_PRESS)
        if(key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, GL_TRUE);
}


