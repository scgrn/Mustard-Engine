layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 position;
layout (location = 2) in vec2 size;
layout (location = 3) in vec2 scale;
layout (location = 4) in float rotation;
layout (location = 5) in vec4 texCoord;
layout (location = 6) in int textureUnit;
layout (location = 7) in vec4 color;

uniform mat4 uProjView;
uniform mat4 uProjection;
uniform mat4 uView;
uniform int uSpherical; // true = spherical, false = cylindrical

out vec4 vColor;
out vec2 vTexCoord;
out vec3 vViewPos;
flat out int vTextureUnit;

void main() {
    mat3 camMat = mat3(transpose(uView));

    vec3 sphericalRight = camMat[0];
    vec3 sphericalUp    = camMat[1];

    vec3 cylindricalRight = normalize(vec3(camMat[0].x, 0.0, camMat[0].z));
    vec3 cylindricalUp    = vec3(0.0, 1.0, 0.0);

    vec3 camRight = normalize(mix(cylindricalRight, sphericalRight, float(uSpherical)));
    vec3 camUp = normalize(mix(cylindricalUp, sphericalUp, float(uSpherical)));

    vec2 halfSize = size * scale;

    vec2 rotated = vec2(
        vertexPosition.x * cos(rotation) - vertexPosition.y * sin(rotation),
        vertexPosition.x * sin(rotation) + vertexPosition.y * cos(rotation)
    );

    vec3 worldPos = position +
        camRight * rotated.x * halfSize.x +
        camUp * rotated.y * halfSize.y;

    gl_Position = uProjection * uView * vec4(worldPos, 1.0);

    vec4 viewPos = uView * vec4(worldPos, 1.0);
    vViewPos = viewPos.xyz;

    vec2 uvs[4] = vec2[4](
        vec2(texCoord.z, texCoord.w),
        vec2(texCoord.z, texCoord.y),
        vec2(texCoord.x, texCoord.y),
        vec2(texCoord.x, texCoord.w)
    );
    vTexCoord = uvs[gl_VertexID];

    vTextureUnit = textureUnit;
    vColor = color;
}
