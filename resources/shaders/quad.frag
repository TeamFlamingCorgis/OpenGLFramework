//ASSIGNMENT 5
#version 150

in vec2 pass_textureCoord;
in vec4 gl_FragCoord;

uniform sampler2D TexSampler;
uniform sampler2D TexID;
uniform int PP_FLAG;

out vec4 out_Color;

//ASSIGNMENT 5
//define binary flags
#define GREYSCALE 1
#define H_MIRRORED 2
#define V_MIRRORED 4
#define BLUR 8

void main() {


    vec2 newCoord = vec2(pass_textureCoord.x / 4.0, pass_textureCoord.y / 4.0);

    //horizontal mirroring
    if ( (PP_FLAG & H_MIRRORED ) == H_MIRRORED ) {
        //flip texture y(v) co-ordinate to horizontally mirror
        newCoord = vec2(newCoord.x, 1.0 - newCoord.y);
    }

    //vertical mirroring
    if ( (PP_FLAG & V_MIRRORED ) == V_MIRRORED ) {
        //flip texture x(u) co-ordinate to vertically mirror
        newCoord = vec2(1.0 - newCoord.x, newCoord.y);
    }


    out_Color = texture(TexID, newCoord);

   //blurring
   if ( (PP_FLAG & BLUR ) == BLUR ) {

       //calc pixel size as a vec2 (x and y)
       vec2 pixelSize = newCoord / vec2(gl_FragCoord);

       //sample color for 9 pixel kernel (adjusted and predefined coordinates)
       //center
       vec3 totalColor = vec3(out_Color) * 0.25;

       //left
       totalColor += 0.125 * vec3(texture(TexID, vec2(max(newCoord.x - pixelSize.x, 0.0), newCoord.y)));
       //right
       totalColor += 0.125 * vec3(texture(TexID, vec2(min(newCoord.x + pixelSize.x, 1.0), newCoord.y)));
       //top
       totalColor += 0.125 * vec3(texture(TexID, vec2(newCoord.x, min(newCoord.y + pixelSize.y, 1.0))));
       //bottom
       totalColor += 0.125 * vec3(texture(TexID, vec2(newCoord.x, max(newCoord.y - pixelSize.y, 0.0))));

       //left top
       totalColor += 0.0625 * vec3(texture(TexID, vec2(max(newCoord.x - pixelSize.x, 0.0), min(newCoord.y + pixelSize.y, 1.0))));
       //left bottom
       totalColor += 0.0625 * vec3(texture(TexID, vec2(max(newCoord.x - pixelSize.x, 0.0), max(newCoord.y - pixelSize.y, 0.0))));
       //right top
       totalColor += 0.0625 * vec3(texture(TexID, vec2(min(newCoord.x + pixelSize.x, 1.0), min(newCoord.y + pixelSize.y, 1.0))));
       //right bottom
       totalColor += 0.0625 * vec3(texture(TexID, vec2(min(newCoord.x + pixelSize.x, 1.0), max(newCoord.y - pixelSize.y, 0.0))));


       //output the sum
       out_Color = vec4(totalColor, 1.0);

   }

   //greyscale
   if ( (PP_FLAG & GREYSCALE ) == GREYSCALE ) {
       //calculate luminescance preserving grey value and assign to output
       vec3 greyVector = vec3(0.2126, 0.7152, 0.0722) * vec3(out_Color);
       float greyColor = dot(greyVector, vec3(1.0, 1.0, 1.0));
       out_Color = vec4(greyColor, greyColor, greyColor, 1.0);
   }
}