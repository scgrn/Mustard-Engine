in vec4 Color;
in vec2 TexCoord;
in float vDistance;
flat in int TextureUnit;

uniform sampler2D textureSamplers[16];

uniform mat4 colorTransform;

out vec4 color;

void main() {
    if (TextureUnit == 0) {
        color = texture(textureSamplers[0], TexCoord) * Color;
    } else if (TextureUnit == 1) {
        color = texture(textureSamplers[1], TexCoord) * Color;
    } else if (TextureUnit == 2) {
        color = texture(textureSamplers[2], TexCoord) * Color;
    } else if (TextureUnit == 3) {
        color = texture(textureSamplers[3], TexCoord) * Color;
    } else if (TextureUnit == 4) {
        color = texture(textureSamplers[4], TexCoord) * Color;
    } else if (TextureUnit == 5) {
        color = texture(textureSamplers[5], TexCoord) * Color;
    } else if (TextureUnit == 6) {
        color = texture(textureSamplers[6], TexCoord) * Color;
    } else if (TextureUnit == 7) {
        color = texture(textureSamplers[7], TexCoord) * Color;
    } else if (TextureUnit == 8) {
        color = texture(textureSamplers[8], TexCoord) * Color;
    } else if (TextureUnit == 9) {
        color = texture(textureSamplers[9], TexCoord) * Color;
    } else if (TextureUnit == 10) {
        color = texture(textureSamplers[10], TexCoord) * Color;
    } else if (TextureUnit == 11) {
        color = texture(textureSamplers[11], TexCoord) * Color;
    } else if (TextureUnit == 12) {
        color = texture(textureSamplers[12], TexCoord) * Color;
    } else if (TextureUnit == 13) {
        color = texture(textureSamplers[13], TexCoord) * Color;
    } else if (TextureUnit == 14) {
        color = texture(textureSamplers[14], TexCoord) * Color;
    } else if (TextureUnit == 15) {
        color = texture(textureSamplers[15], TexCoord) * Color;
    }
    color = color * colorTransform;

    float brightness = exp(-vDistance * 0.01);
    color = vec4(vec3(color * brightness), color.a);
}
