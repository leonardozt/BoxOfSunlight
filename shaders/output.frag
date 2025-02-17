#version 430 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D quadTexture;

void main()
{
	vec3 texColor = texture(quadTexture, texCoords).rgb;
	
	// gamma correction
	float gamma = 2.2;
	fragColor.rgb = pow(texColor, vec3(1.0/gamma));
	fragColor.a = 1.0;
}