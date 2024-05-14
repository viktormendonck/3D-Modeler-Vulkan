#version 450
layout (location = 0) in vec2 fragoffset;

layout (location = 0) out vec4 outColor;

struct PointLight
{
	vec4 position;
	vec4 color;
};

layout(set = 0, binding = 0) uniform UBO
{
	mat4 projectionMatrix;
	mat4 viewMatrix;
	vec4 ambientLightColor;// alpha
	PointLight pointLights[8];
	int numPointLights;
} ubo;

layout(push_constant) uniform Push
{
	vec4 pos;
	vec4 color;
	float rad;
	mat4 modelMatrix;
} push;

void main()
{
	float dist = length(fragoffset);
	if (dist > push.rad)
		discard;
	outColor = vec4(push.color.xyz, 1.0);
}
