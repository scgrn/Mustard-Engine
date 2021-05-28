// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPositionModelSpace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormalModelSpace;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 positionWorldSpace;
out vec3 normalCameraSpace;
out vec3 eyeDirectionCameraSpace;
out vec3 lightDirectionCameraSpace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lightPositionWorldSpace;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPositionModelSpace,1);
	
	// Position of the vertex, in worldspace : M * position
	positionWorldSpace = (M * vec4(vertexPositionModelSpace, 1)).xyz;
	
	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 vertexPositionCameraSpace = ( V * M * vec4(vertexPositionModelSpace,1)).xyz;
	eyeDirectionCameraSpace = vec3(0,0,0) - vertexPositionCameraSpace;

	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	vec3 lightPositionCameraSpace = ( V * vec4(lightPositionWorldSpace,1)).xyz;
	lightDirectionCameraSpace = lightPositionCameraSpace + eyeDirectionCameraSpace;
	
	// Normal of the the vertex, in camera space
	normalCameraSpace = (V * M * vec4(vertexNormalModelSpace, 0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
	
	// UV of the vertex. No special space for this one.
	UV = vertexUV;
}

