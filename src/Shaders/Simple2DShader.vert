#version 450
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;
layout(location = 4) in int selected; // 0 or 1

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
	mat4 model;
} push;

void main()
{
	gl_Position = push.model * vec4(position, 1.0);
	fragColor = color;
}