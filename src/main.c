#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"

#define WIN_X (600)
#define WIN_Y (800)

// Verts for a full canvas flat rect, XYZ
static const float full_canvas_vrt[] = {
    -1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f
};
static const unsigned int full_canvas_ind[] = {
    0,1,2,
    1,2,3
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    GLint prog;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
    GLint sz_u = glGetUniformLocation(prog, "WindowSize");
    glProgramUniform2f(prog, sz_u, width, height);
}

void process_input(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void log_frame_time_throttled() {
    static double last_frame_time = 0;
    static int time_since_last_print = 0;
    time_since_last_print++;
    double this_frame_time = glfwGetTime();
    if(time_since_last_print > 1000) {
        time_since_last_print = 0;
        LOG("delta_t: %f ms", (this_frame_time - last_frame_time) * 1000);
    }
    last_frame_time = this_frame_time;
}

static void make_shader(GLuint shd, const char* fn) {
    FILE* source_file = fopen(fn, "rb");
    fseek(source_file, 0L, SEEK_END);
    int fsz = ftell(source_file);
    rewind(source_file);
    char* source_text = malloc(fsz * sizeof(char));

    if (!source_text) {
        LOG_ERROR("Could not allocate shader file: %s", fn);
        return;
    }
    if (fread(source_text, sizeof(char), fsz, source_file) != fsz) {
        LOG_ERROR("Could not read shader file: %s", fn);
    }

    glShaderSource(shd, 1, &source_text, &fsz);
    glCompileShader(shd);

    GLint  success;
    glGetShaderiv(shd, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shd, sizeof(log), NULL, log);
        LOG_ERROR("Could not compile shader [%s]: %s", fn, log);
    }

    free(source_text);
    fclose(source_file);
}

int main(int argc, char* argv[]) {
    LOG("Initializing GLFW");
    if (glfwInit() != GL_TRUE) {
        LOG_ERROR("Error initializing OpenGL");
        return -1;
    }

    // Set GL version and profile  
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIN_X, WIN_Y, "Reshef", NULL, NULL);
    if (!window) {
        LOG_ERROR("Could not create window");
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG_ERROR("GLAD fail");
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

    glViewport(0, 0, WIN_X, WIN_Y);
    GLint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    GLint VBO;
    glGenBuffers(1, &VBO); 
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(full_canvas_ind), full_canvas_ind, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(full_canvas_vrt), full_canvas_vrt, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 

    // load shaders
    GLuint vert_shd = glCreateShader(GL_VERTEX_SHADER);
    make_shader(vert_shd, "vert_shd.glsl");

    GLuint frag_shd = glCreateShader(GL_FRAGMENT_SHADER);
    make_shader(frag_shd, "frag_shd.glsl");

    GLint shader_prg = glCreateProgram();
    //glAttachShader(shader_prg, vert_shd);
    glAttachShader(shader_prg, frag_shd);
    glLinkProgram(shader_prg);
    glDeleteShader(vert_shd);
    glDeleteShader(frag_shd);

    GLint  success;
    glGetShaderiv(shader_prg, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(shader_prg, sizeof(log), NULL, log);
        LOG_ERROR("Could not compile shader program: %s", log);
        return -1;
    }

    glUseProgram(shader_prg);

    framebuffer_size_callback(window, WIN_X, WIN_Y);
    while(!glfwWindowShouldClose(window)) {
        log_frame_time_throttled();
        process_input(window);
        
        glDrawElements(GL_TRIANGLES, sizeof(full_canvas_ind), GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
    return 0;
}