in vec4 vColor;
in vec2 vTexCoord;
in vec3 vViewPos;
flat in int vTextureUnit;

uniform sampler2D textureSamplers[16];
uniform mat4 colorTransform;
uniform vec3 uFogColor;
uniform float uFogDensity;

out vec4 color;

void main() {
    if (vTextureUnit == 0) {
        color = texture(textureSamplers[0], vTexCoord) * vColor;
    } else if (vTextureUnit == 1) {
        color = texture(textureSamplers[1], vTexCoord) * vColor;
    } else if (vTextureUnit == 2) {
        color = texture(textureSamplers[2], vTexCoord) * vColor;
    } else if (vTextureUnit == 3) {
        color = texture(textureSamplers[3], vTexCoord) * vColor;
    } else if (vTextureUnit == 4) {
        color = texture(textureSamplers[4], vTexCoord) * vColor;
    } else if (vTextureUnit == 5) {
        color = texture(textureSamplers[5], vTexCoord) * vColor;
    } else if (vTextureUnit == 6) {
        color = texture(textureSamplers[6], vTexCoord) * vColor;
    } else if (vTextureUnit == 7) {
        color = texture(textureSamplers[7], vTexCoord) * vColor;
    } else if (vTextureUnit == 8) {
        color = texture(textureSamplers[8], vTexCoord) * vColor;
    } else if (vTextureUnit == 9) {
        color = texture(textureSamplers[9], vTexCoord) * vColor;
    } else if (vTextureUnit == 10) {
        color = texture(textureSamplers[10], vTexCoord) * vColor;
    } else if (vTextureUnit == 11) {
        color = texture(textureSamplers[11], vTexCoord) * vColor;
    } else if (vTextureUnit == 12) {
        color = texture(textureSamplers[12], vTexCoord) * vColor;
    } else if (vTextureUnit == 13) {
        color = texture(textureSamplers[13], vTexCoord) * vColor;
    } else if (vTextureUnit == 14) {
        color = texture(textureSamplers[14], vTexCoord) * vColor;
    } else if (vTextureUnit == 15) {
        color = texture(textureSamplers[15], vTexCoord) * vColor;
    }
    color = color * colorTransform;

    if (color.a < 0.1) {
        discard;
    }

    float distance = -vViewPos.z;

    float fogFactor = exp(-distance * uFogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 finalColor = mix(uFogColor, color.rgb, fogFactor);
    color = vec4(finalColor, color.a);
}
