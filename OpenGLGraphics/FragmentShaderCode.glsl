#version 430

out vec4 col;
in vec3 normalWorld;
in vec3 vertexPositionWorld;
	
uniform vec3 lightPositionWorld;
uniform vec3 eyePositionWorld;
uniform vec4 ambientLight;

void main()
{
	// Diffuse
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = dot(lightVectorWorld, normalize(normalWorld));
	vec4 diffuseLight = vec4(brightness,brightness,brightness,1.0);


	// Specular 
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorld);
	vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
	float s = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld),0,1);
	s = pow(s,32);
	vec4 specularLight= vec4(s, 0, 0, 1);

	col = clamp(diffuseLight,0.0,1.0)  + ambientLight + clamp(specularLight,0.0,1.0);
	//col = clamp(specularLight,0.0,1.0);

}