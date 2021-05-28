// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 positionWorldSpace;
in vec3 normalCameraSpace;
in vec3 eyeDirectionCameraSpace;
in vec3 lightDirectionCameraSpace;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D textureSampler;
uniform mat4 MV;
uniform vec3 lightPositionWorldSpace;

void main(){

	// Light emission properties
	// You probably want to put them as uniforms
	vec3 lightColor = vec3(1,1,1);
	float lightPower = 70.0f; // 50.0f;
	
	// Material properties
	vec3 materialDiffuseColor = texture(textureSampler, UV).rgb;
	//vec3 materialAmbientColor = vec3(0.15,0.15,0.15) * materialDiffuseColor;
	vec3 materialAmbientColor = vec3(0.4,0.4,0.4) * materialDiffuseColor;
	vec3 materialSpecularColor = vec3(0.3,0.3,0.3);

	// Distance to the light
	// float distance = length(lightPositionWorldSpace - positionWorldSpace);
	float distance = 10.0f; 
	
	// Normal of the computed fragment, in camera space
	vec3 n = normalize(normalCameraSpace);
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize(lightDirectionCameraSpace );
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp(dot(n, l), 0.0, 1.0);
	
	// Eye vector (towards the camera)
	vec3 E = normalize(eyeDirectionCameraSpace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l, n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp(dot(E, R), 0.0, 1.0);
	
	vec3 finalColor = 
		// Ambient : simulates indirect lighting
		materialAmbientColor +
		// Diffuse : "color" of the object
		materialDiffuseColor * lightColor * lightPower * cosTheta / (distance*distance) +
		// Specular : reflective highlight, like a mirror
		materialSpecularColor * lightColor * lightPower * pow(cosAlpha, 5.0) / (distance*distance);

	color = vec4(vec3(finalColor), 1.0);
}