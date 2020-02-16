#version 410

attribute in vec2 Coord;

uniform mat4 ModelViewProjection; 
uniform float heightFactor;

uniform sampler2D heightMap;
uniform int horizontal;
uniform int vertical;
uniform float radius;
uniform float hAngle;
uniform float vAngle;
uniform int moveTex;

out vec2 textureCoordinate; 
out vec3 vertexNormal; 
out vec3 posWorld;


void calculatePos(inout vec3 pos,vec2 coord)
{
	float beta = coord.y * 	vAngle;
	float alpha = coord.x * hAngle;
	
	float z = radius * cos(beta);
	float y = radius * sin(beta) * sin(alpha);
	float x = radius * sin(beta) * cos(alpha);
	
	vec2 texCoord = vec2(mod(coord.x+moveTex,horizontal) / horizontal,mod(coord.y,vertical)/vertical);
	
	float height = texture(heightMap, texCoord).x;
	
	pos = vec3(x,y,z);	
	
	pos+= heightFactor * height * normalize(pos);
}

void calculateNormal(inout vec3 normal,vec3 pos,vec2 coord)
{
	normal = vec3(0,0,0);
	
	if(coord.y == 0)
	{
		vec3 dir0;
		calculatePos(dir0,vec2(0,1));
		dir0 -= pos;
		
		for(int i=1 ; i<= horizontal;i++)
		{
			vec3 dir1;
			calculatePos(dir1,vec2(i,1));
			dir1 -= pos;
			normal += cross(dir0, dir1);
			dir0 = dir1;
		}
	}
	else if(coord.y == 1)
	{
		vec3 dir0, dir1, dir2, dir3, dir4;
		calculatePos(dir0,vec2(0,0));
		calculatePos(dir1,coord + vec2(1,0));
		calculatePos(dir2,coord + vec2(0,1));
		calculatePos(dir3,coord + vec2(-1,1));
		calculatePos(dir4,coord + vec2(-1,0));
		dir0 -= pos;
		dir1 -= pos;
		dir2 -= pos;
		dir3 -= pos;
		dir4 -= pos;
		
		normal += cross(dir1, dir0);
    	normal += cross(dir2, dir1);
    	normal += cross(dir3, dir2);
    	normal += cross(dir4, dir3);
    	normal += cross(dir0, dir4);
	}
	else if(coord.y == vertical)
	{
		
		int v = vertical-1;
		
		vec3 dir0;
		calculatePos(dir0,vec2(0,v));
		dir0 -= pos;
		
		for(int i=1 ; i<= horizontal;i++)
		{
			vec3 dir1;
			calculatePos(dir1,vec2(i,v));
			dir1 -= pos;
			normal += cross(dir0, dir1);
			dir0 = dir1;
		}
	}
	else if(coord.y == (vertical-1))
	{
		vec3 dir0, dir1, dir2, dir3, dir4;
		
		calculatePos(dir0,coord + vec2(0,-1));
		calculatePos(dir1,coord + vec2(1,-1));
		calculatePos(dir2,coord + vec2(1,0));
		calculatePos(dir3,vec2(0,vertical));
		calculatePos(dir4,coord + vec2(-1,0));
		dir0 -= pos;
		dir1 -= pos;
		dir2 -= pos;
		dir3 -= pos;
		dir4 -= pos;
		
		normal += cross(dir1, dir0);
    	normal += cross(dir2, dir1);
    	normal += cross(dir3, dir2);
    	normal += cross(dir4, dir3);
    	normal += cross(dir0, dir4);
	
	}
	else
	{
		vec3 dir0, dir1, dir2, dir3, dir4, dir5;
		calculatePos(dir0,coord + vec2(0,-1));
		calculatePos(dir1,coord + vec2(1,-1));
		calculatePos(dir2,coord + vec2(1,0));
		calculatePos(dir3,coord + vec2(0,1));
		calculatePos(dir4,coord + vec2(-1,1));
		calculatePos(dir5,coord + vec2(-1,0));
		dir0 -= pos;
		dir1 -= pos;
		dir2 -= pos;
		dir3 -= pos;
		dir4 -= pos;
		dir5 -= pos;
		
		normal += cross(dir1, dir0);
    	normal += cross(dir2, dir1);
    	normal += cross(dir3, dir2);
    	normal += cross(dir4, dir3);
    	normal += cross(dir5, dir4);
    	normal += cross(dir0, dir5);	
    	
	}
	
	normal = normalize(normal);
}

void main()
{	
    vec3 pos;
    calculatePos(pos,Coord);

	vec3 normal;
	
	calculateNormal(normal, pos, Coord);
  	
  	textureCoordinate = vec2(mod(Coord.x+moveTex,horizontal)/horizontal,Coord.y/vertical);
  	vertexNormal = normal;
	posWorld = pos;
	
	gl_Position = ModelViewProjection * vec4(pos,1);  
}
