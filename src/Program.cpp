#include "Program.h"

namespace BOSL {
    
    Program::Program() : object(0), initialized(false), linked(false) { }
    
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

    void Program::use() const
    {
        if (!initialized || !linked) {
            throw BoxOfSunlightError("Program was not ready for use: "
                "initialize and link, then use.");
        }
        glUseProgram(object);
    }

    void Program::stopUsing() const
    {
        glUseProgram(0);
    }

    void Program::setUniformVec3(const std::string& name, const glm::vec3& vector) const
    {
        if (!initialized) {
            throw BoxOfSunlightError("Program was not initialized");
        }
        glUniform3fv(glGetUniformLocation(object, name.c_str()), 1, &vector[0]);
    }
}
