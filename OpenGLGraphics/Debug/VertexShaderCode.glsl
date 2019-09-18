#version 430
	
in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;

out vec3 theColor;
uniform vec2 translate;
uniform vec3 randomCol;
uniform mat4 fullTransformMatrix;

void main()
{
	vec4 v = vec4(position,1.0);
	//gl_Position = vec4(position.x + translate.x, position.y + translate.y, position.z,1.0);
	//gl_Position = vec4(position,1.0);
	gl_Position = fullTransformMatrix * v;
	theColor = vertexColor;
}