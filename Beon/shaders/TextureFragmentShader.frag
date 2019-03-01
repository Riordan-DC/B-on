#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
	sampler2D texture_normal;
	sampler2D texture_height;
	   
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;

	bool hasNormalTexture;
	bool hasDiffuseTexture;
	bool hasSpecularTexture;
	bool hasHeightTexture;
};

struct DirLight {
    bool On;

    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    bool On;

    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    bool On;

    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 cameraPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    // properties
	// Calculate normal. If a normal map exists, sample it instead.
	vec3 norm;
	if (material.hasNormalTexture && false) {
		norm = normalize(texture(material.texture_normal, TexCoords).rgb);
	}
	else {
		norm = normalize(Normal);
	}
    // Calculate the vector from the camera position to the fragment.
    vec3 viewDir = normalize(cameraPos - FragPos);
    
    vec3 result = vec3(0,0,0);

	if(dirLight.On == true){
		result = CalcDirLight(dirLight, norm, viewDir);
	}
    
	for(int i = 0; i < NR_POINT_LIGHTS; i++) {
		if(pointLights[i].On == true) {
			result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
		}
	}
    
	if(spotLight.On == true){
		result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
	}

	FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	if (material.hasDiffuseTexture) {
		ambient = light.ambient * texture(material.texture_diffuse, TexCoords).rgb;
		diffuse = light.diffuse * (diff * texture(material.texture_diffuse, TexCoords).rgb);
	}
	else {
		ambient = light.ambient * material.ambient;
		diffuse = light.diffuse * (diff * material.diffuse);
	}
	if (material.hasSpecularTexture) {
		specular = light.specular * (spec * texture(material.texture_specular, TexCoords).rgb);
	}
	else {
		specular = light.specular * (spec * material.specular);
	}
	return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));    
    // combine results
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	if (material.hasDiffuseTexture) {
		ambient = light.ambient * texture(material.texture_diffuse, TexCoords).rgb;
		diffuse = light.diffuse * (diff * texture(material.texture_diffuse, TexCoords).rgb);
	}
	else {
		ambient = light.ambient * material.ambient;
		diffuse = light.diffuse * (diff * material.diffuse);
	}
	if (material.hasSpecularTexture) {
		specular = light.specular * (spec * texture(material.texture_specular, TexCoords).rgb);
	}
	else {
		specular = light.specular * (spec * material.specular);
	}
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	if (material.hasDiffuseTexture) {
		ambient = light.ambient * texture(material.texture_diffuse, TexCoords).rgb;
		diffuse = light.diffuse * (diff * texture(material.texture_diffuse, TexCoords).rgb);
	}
	else {
		ambient = light.ambient * material.ambient;
		diffuse = light.diffuse * (diff * material.diffuse);
	}
	if (material.hasSpecularTexture) {
		specular = light.specular * (spec * texture(material.texture_specular, TexCoords).rgb);
	}
	else {
		specular = light.specular * (spec * material.specular);
	}
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}