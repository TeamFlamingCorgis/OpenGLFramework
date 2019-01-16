//ASSIGNMENT 5
#version 150
#extension GL_ARB_explicit_attrib_location : require

// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;

out vec2 pass_textureCoord;

void main(void)
{
    gl_Position = vec4(in_Position, 1.0);

    //translate position coords to tex coords
    pass_textureCoord = vec2((in_Position.x + 1.0) * 2.0, (in_Position.y + 1.0) * 2.0);
}