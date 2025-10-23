in vec2 vUV;
in vec4 vColor;
in vec3 vNormal;
in float vDistance;

out vec4 fragColor;

uniform sampler2D uTexture;

void main() {
    vec4 texColor = texture(uTexture, vUV) * vColor;

    float brightness = exp(-vDistance * 0.01);
    fragColor = texColor * brightness;
    fragColor.a = 1.0;
}
