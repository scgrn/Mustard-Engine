layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inColor;

uniform mat4 uProjView;

out vec2 vUV;
out vec4 vColor;
out vec3 vNormal;

void main() {
    vUV = inUV;
    vColor = inColor;
    vNormal = inNormal;
    gl_Position = uProjView * vec4(inPos, 1.0);
}

