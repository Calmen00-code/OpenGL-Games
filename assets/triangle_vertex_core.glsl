#version 330 core
layout (location = 0) in vec3 aPos;

out vec4 color;

void main()
{
    gl_Position = vec4(aPos.x, -aPos.y, aPos.z, 1.0);

    if (aPos.x < 0.0) {
        color = vec4(1.0, 0.0, 0.0, 1.0);
    } else if (aPos.x == 0) {
        color = vec4(0.0, 1.0, 0.0, 1.0);
    } else {
        // color = vec4(0.0, 0.0, 1.0, 1.0);
        color = vec4(vec2(0.0), vec2(1.0));
    }
}
