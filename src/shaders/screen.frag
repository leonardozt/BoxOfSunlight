#version 430 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D quadTexture;

void main()
{
	fragColor = texture(quadTexture, texCoords);
}