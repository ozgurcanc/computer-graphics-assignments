#version 410 

attribute in vec3 position;

uniform mat4 ModelViewProjection; 
uniform float heightFactor;

uniform sampler2D textureMap;
uniform sampler2D heightMap;
uniform int widthTexture;
uniform int heightTexture;
uniform int moveTex;

out vec3 textureColor;
out vec3 vertexNormal; 
out vec3 posWorld;


void getDirection(vec3 pos, vec2 neighbor, inout vec3 dir)
{
	if(neighbor.x < 0 || neighbor.x >= widthTexture || neighbor.y < 0 || neighbor.y >= heightTexture)
	{
		dir = vec3(0,0,0);
		return;
	}
	
	vec2 neigborTexPos = vec2(0,0);
	neigborTexPos.x = abs(mod(pos.x+moveTex,widthTexture) - widthTexture) / widthTexture;
	neigborTexPos.y = abs(neighbor.y - heightTexture) / heightTexture;
	
	vec3 height = texture(heightMap,neigborTexPos).xyz;
	
	float y = heightFactor * height.x;
	
	vec3 neighborPos = vec3(neighbor.x, y , neighbor.y);
	
	dir = neighborPos - pos;
	
}


void main()
{

    vec3 pos = position;
	
	vec2 textureCoordinate = vec2(0,0);
	textureCoordinate.x = abs(mod(pos.x+moveTex,widthTexture) - widthTexture) / widthTexture;
	textureCoordinate.y = abs(pos.z - heightTexture) / heightTexture;
	
    vec3 height = texture(heightMap, textureCoordinate).xyz;
	
	pos.y = heightFactor * height.x;
	
    vec3 normal = vec3(0,0,0);
	
	vec3 dir0, dir1, dir2, dir3, dir4, dir5;
	
	getDirection(pos,vec2(pos.x,pos.z +1),dir0);
    getDirection(pos,vec2(pos.x + 1,pos.z +1),dir1);
    getDirection(pos,vec2(pos.x + 1,pos.z),dir2);
    getDirection(pos,vec2(pos.x,pos.z - 1),dir3);
    getDirection(pos,vec2(pos.x - 1,pos.z - 1),dir4);
    getDirection(pos,vec2(pos.x - 1,pos.z),dir5);
	
	normal += cross(dir0, dir1);
    normal += cross(dir1, dir2);
    normal += cross(dir2, dir3);
    normal += cross(dir3, dir4);
    normal += cross(dir4, dir5);
    normal += cross(dir5, dir0);
	
	normal = normalize(normal);
    
    
    textureColor = texture(textureMap, textureCoordinate).xyz;
    vertexNormal = normal;
  	posWorld = pos;
  	
  	gl_Position = ModelViewProjection * vec4(pos,1);  

}
