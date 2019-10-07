#version 430

out vec4 col;
in vec3 normalWorld;
in vec3 vertexPositionWorld;
	
uniform vec3 lightPositionWorld;

void main()
{
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = dot(lightVectorWorld, normalize(normalWorld));
	col = vec4(brightness,brightness,brightness,1.0);
}