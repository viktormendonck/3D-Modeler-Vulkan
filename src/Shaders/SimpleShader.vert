#version 450
//layout input
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;


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
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main()
{
	vec4 worldPos = push.modelMatrix * vec4(position, 1.0);
	gl_Position = ubo.projectionMatrix * ubo.viewMatrix * worldPos;
	fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
	fragPosWorld = worldPos.xyz;
	fragColor = color;
	
}
