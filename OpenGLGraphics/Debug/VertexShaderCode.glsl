#version 430
	
in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;

out vec3 theColor;
uniform vec2 translate;
uniform vec3 randomCol;

void main()
{
	gl_Position = vec4(position.xy + translate, position.z,1.0);
	//gl_Position = vec4(position,1.0);
	theColor = randomCol;
}