#include "Shader.h"

namespace BOSL {
    Shader::Shader(const std::string& sourcePath, GLuint shaderType)
    {
        object = glCreateShader(shaderType);
        std::string src = Shader::loadShader(sourcePath);
        const GLchar* srcStrings[1];
        srcStrings[0] = src.c_str();
        glShaderSource(object, 1, srcStrings, NULL);
        glCompileShader(object);

        GLint success = GL_FALSE;
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE)
        {
            std::string msg = "";
            GLint logSize = 0;
            glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logSize);
            char* infoLog = new char[logSize + 1];
            glGetShaderInfoLog(object, logSize, NULL, infoLog);
            msg += sourcePath;
            msg += "\n";
            msg += infoLog;
            delete[] infoLog;
            throw BoxOfSunlightError("Shader compilation error: " + msg + "\n");
        }
    }

    std::string Shader::loadShader(const std::string& sourcePath)
    {
        std::ifstream file(sourcePath);
        if (!file.is_open())
        {
            throw BoxOfSunlightError("Could not open shader at path " + sourcePath);
        }

        std::string sourceCode;
        std::string lineBuffer;
        while (std::getline(file, lineBuffer))
        {
            sourceCode += lineBuffer + '\n';
        }

        file.close();

        return sourceCode;
    }

    GLuint Shader::getObject() const
    {
        return object;
    }

    Shader::~Shader()
    {
        // TODO: add copy/move constructor so that
        // a Shader's destructor doesn't get called
        // when you add it into a std::vector
        
        // Do this for all other classes that
        // have a destructor as well

        //glDeleteShader(object);
    }
}