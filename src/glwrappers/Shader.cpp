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
        std::string includeIdentifier = "#include ";

        std::ifstream file(sourcePath);
        if (!file.is_open())
        {
            throw BoxOfSunlightError("Could not open shader at path " + sourcePath);
        }

        std::string fullSourceCode;
        std::string lineBuffer;
        while (std::getline(file, lineBuffer))
        {
            if (lineBuffer.find(includeIdentifier) != std::string::npos)
            {
                // Erase include identifier
                lineBuffer.erase(0, includeIdentifier.size());
                // lineBuffer now contains name of the included shader source file

                // Write full path in lineBuffer
                lineBuffer.insert(0, config::shadersDir);

                fullSourceCode += loadShader(lineBuffer);
                
                continue;
            }

            fullSourceCode += lineBuffer + '\n';
        }

        file.close();

        return fullSourceCode;
    }

    GLuint Shader::getObject() const
    {
        return object;
    }

    void Shader::release()
    {
        glDeleteShader(object);
        object = 0;
    }

    Shader::~Shader()
    {
        release();
    }
    
    Shader::Shader(Shader&& other) noexcept
        : object{ other.object }
    {
        other.object = 0;
    }

    Shader& Shader::operator=(Shader&& other) noexcept
    {
        // check for self-assignment
        if (this != &other)
        {
            release();
            // object is now 0
            std::swap(object, other.object);
        }
        return *this;
    }
}