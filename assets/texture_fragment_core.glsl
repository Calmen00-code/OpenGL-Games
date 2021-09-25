#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	// FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0);  // setting color to texture 1
	// FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5);

	// inverse texture picture
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, vec2(1.0 - TexCoord.x, TexCoord.y)), 0.5);
}