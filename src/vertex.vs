#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 normal;

out vec2 TexCoord;

out vec3 surfaceNormal;
out vec3 toLightVector;
out vec3 toCameraVector;
out vec4 FragPosLightSpace;
out vec2 pos_vs;
out vec3 FragPos;
out float visibility;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPosition;
uniform sampler2D heightmap;
uniform vec2 facePos;
uniform mat4 lightSpaceMatrix;

const float density = 0.0002;
const float gradient = 1.5;

void main()
{
	vec2 localPosition = (vec4(aPos.x + facePos.x, aPos.y, aPos.z +facePos.y, 1.0f)).xz / 1024.0;
	vec4 worldPosition = model * vec4(aPos.x, aPos.y , aPos.z, 1.0f);
	vec4 positionRelativeToCam = view * worldPosition;
	gl_Position = projection * positionRelativeToCam;
	pos_vs = localPosition;
	TexCoord = tex;
	
    FragPos = vec3(model * vec4(aPos, 1.0));

	surfaceNormal = (model * vec4(normal, 0.0)).xyz;
	toLightVector = lightPosition - worldPosition.xyz;
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
	toCameraVector = (inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPosition.xyz;

	float distance = length(positionRelativeToCam.xyz);
	visibility = exp(-pow((distance*density), gradient)) + (worldPosition.y/500);
	visibility = clamp(visibility, 0.3, 1.0);
}
