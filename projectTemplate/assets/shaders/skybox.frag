out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main() {
    //FragColor = texture(skybox, TexCoords);
    FragColor = texture(skybox, vec3(TexCoords.x, -TexCoords.y, TexCoords.z));
}
