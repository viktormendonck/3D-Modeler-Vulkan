#version 450
layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;

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
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main()
{
	vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	vec3 surfaceNormal = normalize(fragNormalWorld);

	for (int i = 0; i < ubo.numPointLights; i++)
	{
		vec3 lightDir = ubo.pointLights[i].position.xyz - fragPosWorld;
		float distanceToLight = 1.0 / dot(lightDir, lightDir);
		float cosAngle = max(dot(surfaceNormal, normalize(lightDir)), 0.0);
		vec3 intensity = ubo.pointLights[i].color.xyz * ubo.pointLights[i].color.w * distanceToLight;
		diffuseLight += intensity * cosAngle;
	}
	outColor = vec4(fragColor * diffuseLight,1.0);
}
