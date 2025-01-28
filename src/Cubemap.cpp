#include "Cubemap.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace BOSL
{
	Cubemap::Cubemap() : textureObj(0)
	{}

	void Cubemap::init(GLuint texUnit)
	{
		std::vector<std::string> faces =
		{
			"right.jpg",
			"left.jpg",
			"top.jpg",
			"bottom.jpg",
			"front.jpg",
			"back.jpg"
		};
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            faces[i] = config::imagesDir + "cubemap\\" + faces[i];
        }

		textureObj = loadFaces(faces, texUnit);
	}

    GLuint Cubemap::loadFaces(std::vector<std::string> faces, GLuint texUnit)
    {
        glActiveTexture(texUnit);
        GLuint textureObj;
        glGenTextures(1, &textureObj);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureObj);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (!data)
            {
                throw BoxOfSunlightError("Cubemap tex failed to load at path: " + faces[i]);
            }            
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureObj;
    }
}