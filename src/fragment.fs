#version 330 core

out vec4 out_color;
in vec2 TexCoord;
in vec3 surfaceNormal;
in vec3 toLightVector;
in vec3 toCameraVector;
in vec4 FragPosLightSpace;
in vec3 FragPos;
in float visibility;

uniform vec3 color;
uniform sampler2D image;
uniform vec3 lightColor;
uniform float shineDamper;
uniform float reflectivity;
uniform sampler2D shadowMap;
uniform vec3 skyColor;


float ShadowCalculation(vec4 fragPosLightSpace)
{
  vec3 lightPos = toLightVector;
 // perform perspective dividew
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(surfaceNormal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(surfaceNormal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main(void) {
	vec3 unitNormal = normalize(surfaceNormal);
	vec3 unitLightVector = normalize(toLightVector);
	float nDotl = dot(unitNormal, unitLightVector);
	float brightness = max(nDotl, 0.0);
	vec3 diffuse = brightness * lightColor;

	vec3 unitVectorToCamera = normalize(toCameraVector);
	vec3 lightDirection = -unitLightVector;
	vec3 reflectedLightDirection = reflect(lightDirection, unitNormal);
	float specularFactor = dot(reflectedLightDirection, unitVectorToCamera);
	specularFactor = max(specularFactor, 0.0);
	float dampedFactor = pow(specularFactor, shineDamper);
	vec3 finalSpecular = dampedFactor * lightColor * reflectivity;

	float shadow = ShadowCalculation(FragPosLightSpace);       

	out_color = (vec4(diffuse, 1.0) * texture(image, TexCoord)) + vec4(finalSpecular, 1.0);
	out_color = mix(vec4(skyColor, 1.0), out_color, visibility);
	//out_color = vec4(0.5, 0.5, 0.5, 1.0);
}
