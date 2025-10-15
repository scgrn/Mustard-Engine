in vec2 vUV;
in vec4 vColor;
out vec4 fragColor;

uniform sampler2D uTexture;

void main() {
    fragColor = texture(uTexture, vUV) * vColor;
}

