layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec4 inColor;

uniform mat4 uProjView;
out vec2 vUV;
out vec4 vColor;

void main() {
    vUV = inUV;
    vColor = inColor;
    gl_Position = uProjView * vec4(inPos, 1.0);
}

