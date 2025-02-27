#include "Cubemap.h"

#include <stb/stb_image.h>

namespace BOSL
{
	Cubemap::Cubemap()
	{
        glGenTextures(1, &textureObj);
    }

    void Cubemap::load() const
    {
        std::vector<std::string> faces =
        {
            "px.hdr",
            "nx.hdr",
            "py.hdr",
            "ny.hdr",
            "pz.hdr",
            "nz.hdr"
        };

        for (unsigned int i = 0; i < faces.size(); i++)
        {
            faces[i] = config::imagesDir + "Standard-Cube-Map-2\\" + faces[i];
        }

        glBindTexture(GL_TEXTURE_CUBE_MAP, textureObj);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            float* data = stbi_loadf(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (!data)
            {
                throw BoxOfSunlightError("Cubemap tex failed to load at path: " + faces[i]);
            }
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    Cubemap::Cubemap(Cubemap&& other) noexcept
        : textureObj(other.textureObj)
    {
        other.textureObj = 0;
    }

    Cubemap& Cubemap::operator=(Cubemap&& other) noexcept
    {
        // check for self-assignment
        if (this != &other)
        {
            release();
            // object is now 0
            std::swap(textureObj, other.textureObj);
        }
        return *this;
    }

    void Cubemap::release()
    {
        glDeleteTextures(1, &textureObj);
        textureObj = 0;
    }

    Cubemap::~Cubemap()
    {
        release();
    }
}