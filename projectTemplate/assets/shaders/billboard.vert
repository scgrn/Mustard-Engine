layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 position;
layout (location = 2) in vec2 size;
layout (location = 3) in vec2 scale;
layout (location = 4) in float rotation;
layout (location = 5) in vec4 texCoord;
layout (location = 6) in int textureUnit;
layout (location = 7) in vec4 color;

out vec4 Color;
out vec2 TexCoord;
out float vDistance;
flat out int TextureUnit;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 perspectiveProjection;

void main() {
    mat3 camMat = mat3(inverse(view));
    vec3 camRight = camMat[0];
    vec3 camUp    = camMat[1];

    vec2 halfSize = size * scale;

    float s = sin(rotation);
    float c = cos(rotation);
    vec2 rotated = vec2(
        vertexPosition.x * c - vertexPosition.y * s,
        vertexPosition.x * s + vertexPosition.y * c
    );

    vec3 worldPos = position +
                    camRight * rotated.x * halfSize.x +
                    camUp    * rotated.y * halfSize.y;

    gl_Position = perspectiveProjection * view * vec4(worldPos, 1.0);

    vec4 viewPos = view * vec4(vertexPosition, 1.0);
    vDistance = length(viewPos.xyz);

    vec2 uvs[4] = vec2[4](
        vec2(texCoord.z, texCoord.w),
        vec2(texCoord.z, texCoord.y),
        vec2(texCoord.x, texCoord.y),
        vec2(texCoord.x, texCoord.w)
    );
    TexCoord = uvs[gl_VertexID];

    TextureUnit = textureUnit;
    Color = color;
}
