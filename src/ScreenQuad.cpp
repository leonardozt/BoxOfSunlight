#include "ScreenQuad.h"

namespace BOSL
{
    ScreenQuad::ScreenQuad()
    {
        init();
    }

    ScreenQuad::ScreenQuad(ScreenQuad&& other) noexcept
        : VAO{other.VAO}
        , VBO{other.VBO}
    {
        other.VAO = 0;
        other.VBO = 0;
    }

    ScreenQuad& ScreenQuad::operator=(ScreenQuad&& other) noexcept
    {
        // check for self-assignment
        if (this != &other)
        {
            release();
            std::swap(VAO, other.VAO);
            std::swap(VBO, other.VBO);
        }
        return *this;
    }

    void ScreenQuad::init()
	{
        float vertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,   0.0f, 0.0f,
        -1.0f, -1.0f,   0.0f, 1.0f, 
         1.0f,  1.0f,   1.0f, 0.0f,

         1.0f, -1.0f,   1.0f, 1.0f,
         1.0f,  1.0f,   1.0f, 0.0f,
        -1.0f, -1.0f,   0.0f, 1.0f,
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glBindVertexArray(0);
	}

    void ScreenQuad::draw() const
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void ScreenQuad::release()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        VAO = 0;
        VBO = 0;
    }

    ScreenQuad::~ScreenQuad()
    {
        release();
    }
}