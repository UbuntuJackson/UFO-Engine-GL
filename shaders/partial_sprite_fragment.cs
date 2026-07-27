#version 450 core

in vec2 TexCoords;
out vec4 colour;

uniform sampler2D image;
uniform vec4 spriteColor;
uniform float corner_rounding;
uniform vec2 sample_size_not_normalised;
uniform vec2 sample_position;
uniform vec2 texture_size;

void main(){

    vec2 pixel = TexCoords * texture_size - sample_position * texture_size;

    vec2 top_left_corner = vec2(corner_rounding, corner_rounding);
    vec2 top_right_corner = vec2(sample_size_not_normalised.x-corner_rounding, corner_rounding);
    vec2 bottom_left_corner = vec2(corner_rounding, sample_size_not_normalised.y-corner_rounding);
    vec2 bottom_right_corner = vec2(sample_size_not_normalised.x-corner_rounding, sample_size_not_normalised.y-corner_rounding);

    if( (pixel.x < top_left_corner.x     && pixel.y < top_left_corner.y     && distance(pixel, top_left_corner)     > corner_rounding) ||
        (pixel.x > top_right_corner.x    && pixel.y < top_right_corner.y    && distance(pixel, top_right_corner)    > corner_rounding) ||
        (pixel.x < bottom_left_corner.x  && pixel.y > bottom_left_corner.y  && distance(pixel, bottom_left_corner)  > corner_rounding) ||
        (pixel.x > bottom_right_corner.x && pixel.y > bottom_right_corner.y && distance(pixel, bottom_right_corner) > corner_rounding)
    ){
        colour = vec4(0.0, 0.0, 0.0, 0.0);
    }
    else{
        colour = spriteColor * texture(image, TexCoords);
    }

}
