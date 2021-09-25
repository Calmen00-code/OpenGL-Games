#version 330 core
out vec4 FragColor;
in vec3 ourColor;	// the input variable from vertex shader (same name and type)

void main() {
	// FragColor = vec4(1.0f, 0.2f, 0.6f, 1.0f);
	FragColor = vec4(ourColor, 1.0);
}
