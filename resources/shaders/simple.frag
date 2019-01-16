#version 150

//ASSINGMENT 3 - Studied from https://learnopengl.com/Lighting/Basic-Lighting
in vec3 pass_Normal;//Normal
in vec3 pass_vertexPos;//vertex view position
in vec3 pass_lightSourcePos;//light source
in vec3 pass_diffuseColor;//diffuse color
in float pass_ShaderMode;//shader mode
//ASSIGNMENT 4 - TEXTURES
in vec2 pass_textureCoord;
in vec3 pass_tangentSpace;

out vec4 out_Color;

//ASSIGNMENT 4 - TEXTURES
uniform sampler2D textureColor;
uniform sampler2D normalMapId;
uniform bool hasBumpMap;

//ASSIGNMENT 3 - LIGHT
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

    //ASSIGNMENT 4 - TEXTURES
    //adjust co-ordinates to better fit over planets
    vec2 newCoord = vec2(((pass_textureCoord.y + 1.0) * 0.5), ( (pass_textureCoord.x + 1.0) * 0.25));

    //Get the original color as the texture on the new coordinate
    vec3 originalColor = vec3(texture(textureColor, newCoord));

    //Normal mapping
    vec3 normal;

    //if the planet has a bump map - aka the one we decided to pass the bump mapping
    if(hasBumpMap){

        vec3 bumpNormal = normalize(vec3(texture(normalMapId, newCoord)));
        //translate to tangent space by scaling
        bumpNormal = vec3(bumpNormal.x * 2.0 - 1.0, bumpNormal.y * 2.0 - 1.0, bumpNormal.z);

        normal = normalize(pass_Normal);
        vec3 tangent = normalize(pass_tangentSpace);

        //calculate bitangent using cross product of N and T
        vec3 bitangent = cross(normal, tangent);
        //create matrix
        mat3 tangentMatrix = transpose(mat3(tangent,bitangent,normal));
        bumpNormal = bumpNormal * tangentMatrix;

        normal = normalize(bumpNormal);

    }
    else {
        normal = normalize(pass_Normal);
    }

    //ASSIGNMENT 3 - LIGHT
    //THEORY: https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php
    //PRACTICAL: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/

    //normalize normal vector
//    vec3 normal = normalize(pass_Normal);
    //Direction of light from origin to vertex position (https://learnopengl.com/Lighting/Basic-Lighting)
    vec3 lightDir = normalize(pass_lightSourcePos - pass_vertexPos);

    //calculate ambient light
    vec3 ambient = ambientConstant * originalColor;

    //Get the diffuse reflection using Lambert's Law!
    //Lambert's law states that the amount of reflected light is proportional to the cosine of the angle of incidence (dot product).
    //A surface is illuminated by a light source only if the angle of incidence is between 0 and 90 degrees.
    //lambertian is cos of angle between light and normal
    float lambertian = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lambertian * originalColor * diffuseConstant;

    vec3 viewDir = normalize(-pass_vertexPos);

    //specular
    float specularIntensity = 0.0;
    if (lambertian > 0.0) {

        vec3 halfwayVector = normalize(viewDir + lightDir);
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




