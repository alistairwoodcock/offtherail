#ifndef GAME_GL_PLATFORM
#define GAME_GL_PLATFORM

#include "shader.h"

namespace ShaderMaps{
    
    ShaderMap getMap(){
        ShaderMap sm = {};
        sm.count =  0;
        sm.maxCount = 256;
        sm.elements = (ShaderMapElement*)malloc(sm.maxCount * sizeof(ShaderMapElement));
        return sm;
    }

    bool indexHasKey(ShaderMap &sm, int i, const char* key){
        return sm.elements[i].key.compare(key) == 0;
    }

    void set(ShaderMap &sm, const char* key, Shader val){

        bool already_exists = false;
        int found_index = -1;

        for(int i = 0; i < sm.count; i++){
            if(indexHasKey(sm, i, key)){
                found_index = i;
                already_exists = true;
            }
        }

        if(!already_exists){
            found_index = sm.count;
            sm.count++;
        }

        if(sm.count < sm.maxCount)
        {

            if(!already_exists){
                ShaderMapElement newElement = {};

                printf("newElement.key - 1\n");
                newElement.key = std::string(key);
                printf("newElement.key - 2\n");
                sm.elements[found_index] = newElement;
                printf("newElement.key - 3\n");
            }

            sm.elements[found_index].val = val;
        }
    }


    bool contains(ShaderMap &sm, const char* key){

        for(int i = 0; i < sm.count; i++){
            if(indexHasKey(sm, i, key)){
                return true;
            }
        }
        return false;
    }

    Shader get(ShaderMap &sm, const char* key){
        for(int i = 0; i < sm.count; i++){
            if(indexHasKey(sm, i, key)){
                return sm.elements[i].val;
            }
        }

        Shader empty = {};
        return empty;
    }

    void setCallback(ShaderMap &sm, const char* key, void(*callback)(void *state)){
        for(int i = 0; i < sm.count; i++){
            if(indexHasKey(sm, i, key)){
                sm.elements[i].val.callback_set = true;
                sm.elements[i].val.callback = callback;
            }
        }
    }
}

namespace Shaders{

    void setup(State *state){
        GameState* game = &state->game_state;
        
        game->shaderMap = ShaderMaps::getMap();

        // count up all the shaders in the src/shaders/ folder
        std::string dir = "./src/shaders/";
            
        DIR* dirp = opendir(dir.c_str());
        struct dirent *dp;
        
        std::string vs_type = ".vs";
        std::string fs_type = ".fs";

        while ((dp = readdir(dirp)) != NULL)
        {
            printf("%s\n", dp->d_name);
            int l = strlen(dp->d_name);
            std::string fname = dp->d_name;

            if(l > 3){
                
                std::string shaderName = fname.substr(0, l-3);

                if(vs_type.compare(fname.substr(l-3, l)) == 0){
                    std::string vs = "";
                    vs += dir;
                    vs += fname;

                    if(!ShaderMaps::contains(game->shaderMap, shaderName.c_str())){
                        Shader newShader = {};
                        ShaderMaps::set(game->shaderMap, shaderName.c_str(), newShader);
                    }


                    Shader mapShader = ShaderMaps::get(game->shaderMap, shaderName.c_str());
                    mapShader.vsFileName = vs;
                    
                    ShaderMaps::set(game->shaderMap, shaderName.c_str(), mapShader);
                }
            
                if(fs_type.compare(fname.substr(l-3, l)) == 0)
                {
                    std::string fs = "";
                    fs += dir;
                    fs += fname;

                    printf("%s\n", fs.c_str());

                    if(!ShaderMaps::contains(game->shaderMap, shaderName.c_str())){
                        Shader newShader = {};
                        printf("setup empty shader\n");
                        ShaderMaps::set(game->shaderMap, shaderName.c_str(), newShader);
                        printf("set new shader: %s\n", shaderName.c_str());
                    }
                    
                    Shader mapShader = ShaderMaps::get(game->shaderMap, shaderName.c_str());
                    mapShader.fsFileName = fs;

                    ShaderMaps::set(game->shaderMap, shaderName.c_str(), mapShader);
                }
            
            }
        }
        (void)closedir(dirp);

        for(int i = 0; i < game->shaderMap.count; i++){
            Shader shader = game->shaderMap.elements[i].val;
            shader.name = game->shaderMap.elements[i].key;

            if(shader.vsFileName.length() > 0 && shader.fsFileName.length() > 0 && shader.name.length() > 0)
            {
                Shader loadedShader = loadShader(shader.name.c_str(), shader.vsFileName.c_str(), shader.fsFileName.c_str());
                shader.ID = loadedShader.ID;

                ShaderMaps::set(game->shaderMap, shader.name.c_str(), shader);
            }
            else
            {
                printf("shaderMap element[%s] had invalid data\n", game->shaderMap.elements[i].key.c_str());
            }
            
        }        
        

        game->shaderUpdateTimeout = 2;
    }

    void update(State *state){
        GameState* game = &state->game_state;
        PlatformState* platform = &state->platform;

        game->shaderUpdateTimeout -= platform->deltaTime;

        if(game->shaderUpdateTimeout > 0) return;


        for(int i = 0; i < game->shaderMap.count; i++){
            ShaderMapElement sme = game->shaderMap.elements[i];

            Shader shader = sme.val;

            freeShader(shader);

            Shader loadedShader = loadShader(shader.name.c_str(), shader.vsFileName.c_str(), shader.fsFileName.c_str());

            shader.ID = loadedShader.ID;

            if(shader.callback_set){
                shader.callback((void*)state);
            }

            game->shaderMap.elements[i].val = shader;
        }

        game->shaderUpdateTimeout = 2.0;
    }

    Shader get(State* state, const char* shaderName){
        GameState* game = &state->game_state;

        return ShaderMaps::get(game->shaderMap, shaderName);
    }

    void reloadCallback(State* state, const char* name, void(*callback)(void *state)){
        GameState* game = &state->game_state;
        
        ShaderMaps::setCallback(game->shaderMap, name, callback);

        callback(state);
    }

}

Shader loadShader(const char* shaderName, const char* vertexPath, const char* fragmentPath, const char* geometryPath){
    
    // 1. retrieve the vertex/fragment source code from filePath
    char* vShaderCode = loadFileText(vertexPath);

    Shader shader = {0};

    if(vShaderCode == NULL) return shader;

    char* fShaderCode = loadFileText(fragmentPath);
    if(fShaderCode == NULL){
        free(vShaderCode);
        return shader;
    } 
    

    char* gShaderCode = NULL;
    
    if (geometryPath != nullptr)
    {
        gShaderCode = loadFileText(geometryPath);
        if(gShaderCode == NULL){
            free(vShaderCode);
            free(fShaderCode);
            return shader;
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

    shader.ID = ID;
    //shader.name = "";

    int shaderNameLen = strlen(shaderName);

    if(shaderNameLen > 0){
        
        shader.name = (char*)malloc(strlen(shaderName)+1);
        for(int i = 0; i < strlen(shaderName); i++){
        	shader.name[i] = shaderName[i];
            shader.name[i+1] = '\0';
        }
        
    }

    free(vShaderCode);
    free(fShaderCode);

    return shader;
}

void freeShader(Shader s){
    glDeleteProgram(s.ID);
    // free(s.name);
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

void useShader(unsigned int ID){
    glUseProgram(ID);
}

void shaderSetBool(unsigned int ID, const std::string &name, bool value)
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void shaderSetInt(unsigned int ID, const std::string &name, int value)
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void shaderSetFloat(unsigned int ID, const std::string &name, float value)
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void shaderSetVec2(unsigned int ID, const std::string &name, const glm::vec2 &value)
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void shaderSetVec2(unsigned int ID, const std::string &name, float x, float y)
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void shaderSetVec3(unsigned int ID, const std::string &name, const glm::vec3 &value)
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void shaderSetVec3(unsigned int ID, const std::string &name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void shaderSetVec4(unsigned int ID, const std::string &name, const glm::vec4 &value)
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void shaderSetVec4(unsigned int ID, const std::string &name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void shaderSetMat2(unsigned int ID, const std::string &name, const glm::mat2 &mat)
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void shaderSetMat3(unsigned int ID, const std::string &name, const glm::mat3 &mat)
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void shaderSetMat4(unsigned int ID, const std::string &name, const glm::mat4 &mat)
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

#endif
