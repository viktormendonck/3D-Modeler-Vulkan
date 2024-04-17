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
} push;



void main()
{
	fragoffset = OFFSETS[gl_VertexIndex];
	vec3 cameraRightWorld = vec3(ubo.viewMatrix[0][0], ubo.viewMatrix[1][0], ubo.viewMatrix[2][0]);
	vec3 cameraUpWorld = vec3(ubo.viewMatrix[0][1], ubo.viewMatrix[1][1], ubo.viewMatrix[2][1]);

	vec3 positionWorld = push.pos.xyz + 
	cameraRightWorld * fragoffset.x * push.rad + 
	cameraUpWorld * fragoffset.y * push.rad;

	gl_Position = ubo.projectionMatrix * ubo.viewMatrix* vec4(positionWorld, 1.0);
}
