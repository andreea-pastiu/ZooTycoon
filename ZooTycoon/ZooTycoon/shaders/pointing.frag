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

//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;

float constant = 1.0f;
float linear = 0.7f;
float quadratic = 1.8f;

void computeDirLight()
{
    //compute eye space coordinates
    vec3 cameraPosEye = vec3(10.0f, 0.0f, -1.0f);
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 lightPosEye = vec3(10.0f, 0.0f, -5.0f);
    float dist = length(lightPosEye - fPosEye.xyz);
    float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = normalize(lightPosEye - fPosEye.xyz);
    /*
    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(cameraPosEye - fPosEye.xyz);

    //compute half vector
	vec3 halfVector = normalize(lightDirN + viewDir);
		
	//compute ambient light
	ambient = att * ambientStrength * lightColor;
	//compute diffuse light
	diffuse = att * max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), 32);
	specular = att * specularStrength * specCoeff * lightColor;*/

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

void main() 
{
    computeDirLight();

    //compute final vertex color
    vec3 color = min((ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specular * texture(specularTexture, fTexCoords).rgb, 1.0f);

    fColor = vec4(color, 1.0f);
}
