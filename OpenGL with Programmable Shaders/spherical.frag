#version 410

out vec4 color;

uniform vec3 cameraPosition;
uniform vec3 lightOffSet;

uniform sampler2D textureMap;

in vec2 textureCoordinate; 
in vec3 vertexNormal; 
in vec3 posWorld;

vec3 lightPosition = vec3(0,1600,0) + lightOffSet;

vec4 AmbientCoefficient = vec4(0.25,0.25,0.25,1.0); 
vec4 AmbientLight = vec4(0.3,0.3,0.3,1.0); 
vec4 DiffuseCoefficient = vec4(1.0, 1.0, 1.0, 1.0); 
vec4 DiffuseLight = vec4(1.0, 1.0, 1.0, 1.0); 
vec4 SpecularCoefficient = vec4(1.0, 1.0, 1.0, 1.0); 
vec4 SpecularLight = vec4(1.0, 1.0, 1.0, 1.0); 
int specularExponent = 100; 
  	
void main() {
		
	vec3 vertexToCamera = normalize(cameraPosition - posWorld);
  	vec3 vertexToLight = normalize(lightPosition - posWorld);
  	
	vec3 textureColor = texture(textureMap, textureCoordinate).xyz;

  	vec3 h = normalize(vertexToCamera + vertexToLight);
  
  	vec4 ambient = AmbientCoefficient * AmbientLight;
  
  	vec4 diffuse = DiffuseCoefficient * DiffuseLight * max(dot(vertexNormal,vertexToLight),0);
  	
  	vec4 specular = SpecularCoefficient * SpecularLight * pow(max(dot(vertexNormal,h),0),specularExponent);
	
  	color = vec4(clamp( textureColor * vec3(ambient + diffuse + specular), vec3(0,0,0), vec3(1,1,1)), 1.0);
	
}
