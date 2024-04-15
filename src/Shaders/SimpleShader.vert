#version 450
//layout input
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform UBO
{
	mat4 projectionMatrix;
	vec3 lightDir;
	float ambientIntensity;
} ubo;

layout(push_constant) uniform Push
{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main()
{
	gl_Position = ubo.projectionMatrix * push.modelMatrix * vec4(position, 1.0);
	
	vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);
	
	float lightIntensity = max(dot(normalWorldSpace, ubo.lightDir), 0.0) + ubo.ambientIntensity;
	fragColor = lightIntensity * color;
}
