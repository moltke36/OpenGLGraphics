#version 430
	
in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;

out vec3 theColor;
uniform vec2 translate;

void main()
{
	gl_Position = vec4(position.x, position.y + translate.x * (position.x/abs(position.x)-1.0)/2 + translate.y * (position.x/abs(position.x)+1.0)/2 ,position.z,1.0);
	//gl_Position = vec4(position,1.0);
	theColor = vertexColor;
}