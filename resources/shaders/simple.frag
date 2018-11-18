#version 150

//ASSINGMENT 3 - Studied from https://learnopengl.com/Lighting/Basic-Lighting
in vec3 pass_Normal;//Normal
in vec3 pass_vertexPos;//vertex view position
in vec3 pass_lightSourcePos;//light source
in vec3 pass_diffuseColor;//diffuse color
in float pass_ShaderMode;//shader mode

out vec4 out_Color;

//Ambient, diffuse and specular constants
float ambientConstant = 0.3;
float diffuseConstant = 0.7;
float specularConstant = 0.5;
float gloss = 2.0; //leave the planet a bit matte
vec3 specColor = vec3(1.0, 1.0, 1.0);
//Color: from hexadecimal to rgb, then to rgb divided by 255 to get the 0-1 number
//With the help of this online converter https://www.webpagefx.com/web-design/hex-to-rgb/
vec3 outColor = vec3(0.788, 0.788, 0.788);//Default outcolor: #c9c9c9 -> rgb(210,210,210) -> (0.788, 0.788, 0.788)


void main() {

    //THEORY: https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php
    //PRACTICAL: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/

    //normalize normal vector
    vec3 normal = normalize(pass_Normal);
    //Direction of light from origin to vertex position (https://learnopengl.com/Lighting/Basic-Lighting)
    vec3 viewDir = normalize(pass_lightSourcePos - pass_vertexPos);

    //calculate ambient light
    vec3 ambient = ambientConstant * pass_diffuseColor;

    //Get the diffuse reflection using Lambert's Law!
    //Lambert's law states that the amount of reflected light is proportional to the cosine of the angle of incidence (dot product).
    //A surface is illuminated by a light source only if the angle of incidence is between 0 and 90 degrees.
    //lambertian is cos of angle between light and normal
    float lambertian = max(dot(viewDir, normal), 0.0);
    vec3 diffuse = lambertian * pass_diffuseColor * diffuseConstant;

    viewDir = normalize(-pass_vertexPos);

    //specular
    float specularIntensity = 0.0;
    if (lambertian > 0.0) {

        vec3 halfwayVector = normalize(viewDir + viewDir);
        float specAngle = max(dot(halfwayVector, normal), 0.0);
        specularIntensity = pow(specAngle, gloss);
    }
    //spec = cosine of alpha to the light_ref_coef
    //http://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/
    vec3 specular = specularConstant * specColor * specularIntensity;

    //combine specular, diffuse and ambient
    out_Color = vec4(ambient + diffuse + specular, 1.0);


    //ASSIGNMENT 3 - Cel Shading effect
    if (pass_ShaderMode == 2.0) {

        //calc. cos of angle between normal and view direction
        float viewAngleCosine = dot(normal, viewDir);

        //cosine decreases as angle approaches 90 degrees
        //if cos value is less than x, colour to outline color
        if (viewAngleCosine < 0.3) {
            out_Color = vec4(outColor, 1.0);
        }else{
            out_Color = ceil(out_Color * 4) / 4;

        }
    }

}




