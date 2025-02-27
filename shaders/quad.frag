#version 460

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D quadTexture;

void main()
{
	vec3 texColor = texture(quadTexture, texCoords).rgb;
	
	// Convert from HDR to LDR (Reinhard tone mapping)
	//vec3 mapped = texColor / (texColor + vec3(1.0));
    float exposure = 1.0;
    vec3 mapped = vec3(1.0) - exp(-texColor * exposure);
	// gamma correction
	float gamma = 2.2;
	mapped = pow(mapped, vec3(1.0/gamma));
	
	fragColor = vec4(mapped, 1.0);
}