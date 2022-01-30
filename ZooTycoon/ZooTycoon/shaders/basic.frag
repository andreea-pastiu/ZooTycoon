#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;
// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform int showFog;
uniform int dirLight;

//components
vec3 ambient;
vec3 ambient2;
float ambientStrength = 0.02f;
float ambientStrength2 = 0.2f;
vec3 diffuse;
vec3 diffuse2;
vec3 specular;
vec3 specular2;
float specularStrength = 0.05f;
float specularStrength2 = 0.5f;
vec3 lightPosEye = vec3(10.0f, 0.0f, -1.0f);

void computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;
}

float constant = 1.0f;
float linear = 0.0045f;
float quadratic = 0.0075f;

void CalcPointLight()
{
	vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 lightPosEye = vec3(0.0f, 0.0f, 0.0f);
	
	//compute distance to light
	float dist = length(lightPosEye - fPosEye.xyz);
	//compute attenuation
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));

	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightPosEye - fPosEye.xyz);

	//compute view direction
	vec3 viewDirN = normalize(fPosEye.xyz);
	
	//compute half vector
	vec3 halfVector = normalize(lightDirN + viewDirN);
		
	//compute ambient light
	ambient2 = att * ambientStrength * lightColor;
	//compute diffuse light
	diffuse2 = att * max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), 32);
	specular2 = att * specularStrength * specCoeff * lightColor;
} 

float computeFog()
{
	vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
	float fogDensity = 0.2f;
	float fragmentDistance = length(fPosEye);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

	return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
    computeDirLight();
    CalcPointLight();


    //compute final vertex color
    vec3 color = min((ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specular * texture(specularTexture, fTexCoords).rgb, 1.0f);
    vec3 color2 = min((ambient2 + diffuse2) * texture(diffuseTexture, fTexCoords).rgb + specular2 * texture(specularTexture, fTexCoords).rgb, 1.0f);

	vec4 colorMix = vec4(color, 1.0f);

	if(dirLight == 0)
	{
		colorMix = vec4(color2, 1.0f);
	}

	if(showFog == 1)
	{
		float fogFactor = computeFog();
		vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
		fColor = fogColor * (1-fogFactor) + colorMix * fogFactor;
	}
	else
	{
		fColor = colorMix;
	}
}
