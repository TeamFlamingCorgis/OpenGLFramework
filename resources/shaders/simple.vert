#version 150
#extension GL_ARB_explicit_attrib_location : require

//ASSINGMENT 3 - Studied from https://learnopengl.com/Lighting/Basic-Lighting and
//THEORY: https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php
//PRACTICAL: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/

// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;

//ASSIGNENT 3 - Variables for shading and lighting
uniform vec3 sunPosition;//sun position, main light source
uniform vec3 DiffuseColor;//diffuse color
uniform int ShaderMode;//shader mode

out vec3 pass_Normal;//Normal
out vec3 pass_vertexPos;//vertex view position
out vec3 pass_lightSourcePos;//light source
out vec3 pass_diffuseColor;//diffuse color
out float pass_ShaderMode;//shader mode

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	pass_Normal = (NormalMatrix * vec4(in_Normal, 0.0)).xyz;

    //ASSIGNMENT 3 - Vertex position and diffuse color
    pass_vertexPos = vec3(ViewMatrix * ModelMatrix * vec4(in_Position, 1.0));
    pass_lightSourcePos = sunPosition;
    pass_diffuseColor = DiffuseColor;

    //cast shader to float
    pass_ShaderMode = float(ShaderMode);
}