#include "Program.h"

namespace BOSL {
    
    Program::Program()
        : object(0)
        , initialized(false)
        , linked(false)
        , isBeingUsed(false)
    { }
    
    void Program::init()
    {
        object = glCreateProgram();
        initialized = true;
    }

    void Program::link(const std::vector<Shader>& shaders)
    {        
        if (!initialized) {
            throw BoxOfSunlightError("Program was not initialized");
        }

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
        linked = true;
    }

    Program::~Program()
    {
        glDeleteProgram(object);
    }

    void Program::use()
    {
        if (!initialized || !linked) {
            throw BoxOfSunlightError("Program was not ready for use: "
                "initialize and link, then use.");
        }
        glUseProgram(object);
        isBeingUsed = true;
    }

    void Program::stopUsing()
    {
        glUseProgram(0);
        isBeingUsed = false;
    }

    void Program::setUniformVec3(const std::string& name, const glm::vec3& vector) const
    {
        if (!initialized) {
            throw BoxOfSunlightError("Program::setUniformVec3() - Program was not initialized");
        }
        if (!isBeingUsed) {
            throw BoxOfSunlightError("Program::setUniformVec3() - To set value of a uniform,"
                " the program needs to be used first");
        }
        glUniform3fv(glGetUniformLocation(object, name.c_str()), 1, &vector[0]);
    }

    void Program::setUniformInt(const std::string& name, int value) const
    {
        if (!initialized) {
            throw BoxOfSunlightError("Program::setUniformInt() - Program was not initialized");
        }
        if (!isBeingUsed) {
            throw BoxOfSunlightError("Program::setUniformInt() - To set value of a uniform,"
                " the program needs to be used first");
        }
        glUniform1i(glGetUniformLocation(object, name.c_str()), value);
    }
}
