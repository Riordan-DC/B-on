#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 View;
uniform mat4 Projection;

void main()
{
    TexCoords = aPos;
    mat4 globalView = mat4(mat3(View));
    vec4 pos = Projection * globalView * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}  