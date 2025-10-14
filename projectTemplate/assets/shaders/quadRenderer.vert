layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;

uniform mat4 uProjView;

out vec2 vUV;

void main() {
    vUV = inUV;
    gl_Position = uProjView * vec4(inPosition, 1.0);
}

