#version 430
	
in layout(location=0) vec3 vertexPositionModel;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normalModel;

uniform mat4 modelToProjectionMatrix;
uniform mat4 modelToWorldMatrix;
uniform vec3 ambientLight;
uniform vec3 lightPosition;

out vec3 normalWorld;
out vec3 vertexPositionWorld;

void main()
{
	vec4 v = vec4(vertexPositionModel,1.0);
	gl_Position = modelToProjectionMatrix * v;
	normalWorld = vec3(modelToWorldMatrix * vec4(normalModel,0.0));
	vertexPositionWorld = vec3(modelToWorldMatrix * v);
}