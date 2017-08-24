#ifndef GAME_PLATFORM
#define GAME_PLATFORM


#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

#include "libs/shader.h"
#include "libs/model.h"

//Necessary for shader loading
#include <stdio.h>
#include <stdlib.h>



// Default screen width and height
const float WIDTH = 500;
const float HEIGHT = 500;

const int MAX_SHADERS = 32;
const int MAX_MODELS = 32;

struct Platform {
	bool initialized;
	bool closeWindow;

	int screenWidth;
	int screenHeight;

	float currTime;
	float prevTime;
	float deltaTime;

	Shader shaders[MAX_SHADERS];
	int shaderCount;

	// Model models[MAX_MODELS];
	// int modelCount;

	GLFWwindow *window;

	static void window_resize(GLFWwindow* window, int width, int height){ glViewport(0, 0, width, height); }

	bool init(){
		if(initialized) return false;

		glfwInit();

		glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );

		window = glfwCreateWindow(WIDTH, HEIGHT, "Off The Rail", NULL, NULL);
		
		if(window == NULL) {
			printf("Failed to create GLFW window\n");
			glfwTerminate();
			return false;
		}

		glfwGetFramebufferSize(window, &screenWidth, &screenHeight );
		glfwSetFramebufferSizeCallback(window, window_resize);

		glfwMakeContextCurrent(window);

		glewExperimental = GL_TRUE;
		if(glewInit() != GLEW_OK) {
			printf("Failed to initialize GLEW\n");
			glfwTerminate();
			return false;
		}

		glViewport(0, 0, screenWidth, screenHeight);
		glEnable(GL_DEPTH_TEST);  
		glEnable(GL_BLEND); 
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

		/* Setup shader memory */
		shaderCount = 0;
		
		/* Setup model memory */
		// modelCount = 0;
		
		initialized = true;

		return true;
	}

    bool reinit(){
        glfwTerminate();
        prevTime = 0;
        currTime = 0;
        deltaTime = 0;
        initialized = false;
        return init();
    }

	Input getInput() {
		Input in = {0};

		in.escape_pressed = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
		in.up_pressed = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
		in.down_pressed = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
		in.left_pressed = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
		in.right_pressed = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
		in.space_pressed = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
		in.enter_pressed = (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS);

		return in;
	}

	void pollEvents(){
		glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
		glfwPollEvents();
		closeWindow = glfwWindowShouldClose(window);
	}

	void clearBuff(glm::vec3 bg){
		// clear buffer
		glClearColor(bg.x, bg.y, bg.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void swapBuffs(){
		glfwSwapBuffers(window);
	}

	float getTime(){
		prevTime = currTime;
        currTime = glfwGetTime();
        deltaTime = currTime - prevTime;

		return currTime;
	}

    char* loadFileText(const char* fileName){
        char *source = NULL;
        FILE *fp = fopen(fileName, "r");
        if (fp != NULL) {
            /* Go to the end of the file. */
            if (fseek(fp, 0L, SEEK_END) == 0) {
                /* Get the size of the file. */
                long bufsize = ftell(fp);
                if (bufsize == -1) { /* Error */ }

                /* Allocate our buffer to that size. */
                source = (char*)malloc(sizeof(char) * (bufsize + 1));

                /* Go back to the start of the file. */
                if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */ }

                /* Read the entire file into memory. */
                size_t newLen = fread(source, sizeof(char), bufsize, fp);
                if ( ferror( fp ) != 0 ) {
                    fputs("Error reading file", stderr);
                } else {
                    source[newLen++] = '\0'; /* Just to be safe. */
                }
            }
            fclose(fp);
        }
        return source;
    }

	Shader loadShader(const char* shaderName, const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr){
        
        // 1. retrieve the vertex/fragment source code from filePath
        char* vShaderCode = loadFileText(vertexPath);
        if(vShaderCode == NULL) return {0};

        char* fShaderCode = loadFileText(fragmentPath);
        if(fShaderCode == NULL){
            free(vShaderCode);
            return {0};
        } 
        

        char* gShaderCode = NULL;
        
        if (geometryPath != nullptr)
        {
            gShaderCode = loadFileText(geometryPath);
            if(gShaderCode == NULL){
                free(vShaderCode);
                free(fShaderCode);
                return {0};
            }   
        }

        // 2. compile shaders
        unsigned int vertex, fragment;
        int success;
        char infoLog[512];
        // vertex shader

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // if geometry shader is given, compile geometry shader
        unsigned int geometry;
        if (gShaderCode)
        {
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");

            free(gShaderCode);
        }
        // shader Program
        unsigned int ID = glCreateProgram();

        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (geometryPath != nullptr)
            glAttachShader(ID, geometry);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometryPath != nullptr)
            glDeleteShader(geometry);

        Shader shader;
        shader.ID = ID;
        shader.name = "";

        int shaderNameLen = strlen(shaderName);

        if(shaderNameLen > 0){
            printf("strlen('%s'):%i \n", shaderName, strlen(shaderName));
            
            shader.name = (char*)malloc(strlen(shaderName)+1);
            for(int i = 0; i < strlen(shaderName); i++){
            	shader.name[i] = shaderName[i];
                shader.name[i+1] = '\0';
            }
            
        }

        if(ID > 0){
            shaders[shaderCount] = shader;
            shaderCount++;
        }


        free(vShaderCode);
        free(fShaderCode);


        return shader;
	}

    void freeShaders(){
        for(int i = 0; i < shaderCount; i++){
            Shader s = shaders[i];
            glDeleteProgram(s.ID);
            free(s.name);

            shaders[i];
        }

        shaderCount = 0;
    }

	void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

	Shader getShader(const char* name){
		for(int i = 0; i < shaderCount; i++){
			Shader s = shaders[i];
            if(std::strcmp(s.name, name) == 0){
				return s;
			}
		}

		Shader s = {0};
		return s;
	}

	void useShader(unsigned int ID){
        glUseProgram(ID);
	}

	void shaderSetBool(unsigned int ID, const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    
    void shaderSetInt(unsigned int ID, const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    
    void shaderSetFloat(unsigned int ID, const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    
    void shaderSetVec2(unsigned int ID, const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void shaderSetVec2(unsigned int ID, const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    
    void shaderSetVec3(unsigned int ID, const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void shaderSetVec3(unsigned int ID, const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    
    void shaderSetVec4(unsigned int ID, const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void shaderSetVec4(unsigned int ID, const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    
    void shaderSetMat2(unsigned int ID, const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    void shaderSetMat3(unsigned int ID, const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    void shaderSetMat4(unsigned int ID, const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }



	// Model* loadModel(const char* name, const char* path){
	// 	if(modelCount >= MAX_MODELS){
	// 		printf("TOO MANY MODELS\n");
	// 		return NULL;
	// 	}

	// 	Model *m = new (models+modelCount) Model(name, path);
	// 	modelCount++;
	// 	return m;
	// }

	// Model* getModel(const char* name){
	// 	for(int i = 0; i < modelCount; i++){
	// 		Model *m = models+i;
	// 		if(std::strcmp(m->name.c_str(), name) == 0){
	// 			return m;
	// 		}
	// 	}

	// 	return NULL;	
	// }
};

#endif