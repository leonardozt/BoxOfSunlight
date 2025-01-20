#include "Program.h"

namespace BOSL {
    Program::Program(const std::vector<Shader> shaders)
    {
        object = glCreateProgram();
        for (unsigned i = 0; i < shaders.size(); i++) {
            glAttachShader(object, shaders[i].getObject());
        }
        glLinkProgram(object);

        for (unsigned i = 0; i < shaders.size(); i++) {
            glDetachShader(object, shaders[i].getObject());
        }

        GLint success = GL_FALSE;
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (success == GL_FALSE)
        {
            std::string msg = "Error while linking program\n";
            GLint logSize = 0;
            glGetProgramiv(object, GL_INFO_LOG_LENGTH, &logSize);
            char* infoLog = new char[logSize + 1];
            glGetProgramInfoLog(object, logSize, NULL, infoLog);
            msg += infoLog;
            delete[] infoLog;
            throw BoxOfSunlightError(msg + '\n');
        }
    }

    Program::~Program()
    {
        glDeleteProgram(object);
    }

    void Program::use() const
    {
        glUseProgram(object);
    }

    GLuint Program::getObject() const
    {
        return object;
    }
}
