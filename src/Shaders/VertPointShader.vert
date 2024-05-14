#version 450
//layout input

const vec2 OFFSETS[6] = vec2[](
  vec2(-1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, -1.0),
  vec2(1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, 1.0)
);


layout(location = 0) out vec2 fragoffset;

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
	vec4 worldPos = push.modelMatrix * vec4(push.pos.xyz, 1.0);
	gl_Position = ubo.projectionMatrix * ubo.viewMatrix * worldPos;
}
