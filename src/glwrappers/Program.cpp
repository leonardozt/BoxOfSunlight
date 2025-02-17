#include "Program.h"

namespace BOSL {
    
    Program::Program()
        : isBeingUsed(false)
        , linked(false)
    {
        object = glCreateProgram();
    }
    
    Program::Program(Program&& other) noexcept
        : linked(other.linked)
        , isBeingUsed(other.isBeingUsed)
        , object(other.object)
    {
        other.linked = false;
        other.isBeingUsed = false;
        other.object = 0;
    }

    Program& Program::operator=(Program&& other) noexcept
    {
        // check for self-assignment
        if (this != &other)
        {
            release();
            // object is now 0
            std::swap(object, other.object);

            linked = other.linked;
            isBeingUsed = other.isBeingUsed;

            other.linked = false;
            other.isBeingUsed = false;
        }
        return *this;
    }

    void Program::link(const std::vector<Shader>& shaders)
    {        
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

    void Program::use()
    {
        if (!linked) {
            throw BoxOfSunlightError("Shader Program was not linked before use.");
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
        checkLinkedUsed("setUniformVec3");
        glUniform3fv(glGetUniformLocation(object, name.c_str()), 1, &vector[0]);
    }

    void Program::setUniformVec2(const std::string& name, const glm::vec2& vector) const
    {
        checkLinkedUsed("setUniformVec2");
        glUniform2fv(glGetUniformLocation(object, name.c_str()), 1, &vector[0]);
    }

    void Program::setUniformFloat(const std::string& name, float value) const
    {
        checkLinkedUsed("setUniformFloat");
        glUniform1f(glGetUniformLocation(object, name.c_str()), value);
    }

    void Program::setUniformInt(const std::string& name, int value) const
    {
        checkLinkedUsed("setUniformInt");
        glUniform1i(glGetUniformLocation(object, name.c_str()), value);
    }

    void Program::setUniformMat3(const std::string& name, const glm::mat3& matrix) const
    {
        checkLinkedUsed("setUniformMat3");
        glUniformMatrix3fv(glGetUniformLocation(object, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
    }

    Program::~Program()
    {
        release();
    }

    void Program::release()
    {
        glDeleteProgram(object);
        object = 0;
    }

    void Program::checkLinkedUsed(const std::string& callingFunction) const
    {
        std::string fullFuncName = "Program::" + callingFunction + "()";
        if (!linked) {
            throw BoxOfSunlightError(fullFuncName + " - Program was not linked before call.");
        }
        if (!isBeingUsed) {
            throw BoxOfSunlightError(fullFuncName + " - Program was not used before call.");
        }
    }
}
