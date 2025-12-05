in vec2 vUV;
in vec4 vColor;
in vec3 vNormal;
in vec3 vViewPos;

out vec4 fragColor;

uniform sampler2D uTexture;
uniform vec3 uFogColor;
uniform float uFogDensity;

void main() {
    vec4 texColor = texture(uTexture, vUV) * vColor;

    float distance = -vViewPos.z;

    float fogFactor = exp(-distance * uFogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 finalColor = mix(uFogColor, texColor.rgb, fogFactor);
    fragColor = vec4(finalColor, texColor.a);
}
