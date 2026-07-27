#version 450 core

in vec2 TexCoords;
out vec4 color;

uniform vec4 spriteColor;
uniform float corner_rounding;
uniform vec2 size_not_normalised;

void main(){
    vec2 pixel = TexCoords * size_not_normalised;

    vec2 top_left_corner = vec2(corner_rounding, corner_rounding);
    vec2 top_right_corner = vec2(size_not_normalised.x-corner_rounding, corner_rounding);
    vec2 bottom_left_corner = vec2(corner_rounding, size_not_normalised.y-corner_rounding);
    vec2 bottom_right_corner = vec2(size_not_normalised.x-corner_rounding, size_not_normalised.y-corner_rounding);

    if((pixel.x < top_left_corner.x && pixel.y < top_left_corner.y && distance(pixel, top_left_corner) > corner_rounding) ||
        (pixel.x > top_right_corner.x && pixel.y < top_right_corner.y && distance(pixel, top_right_corner) > corner_rounding) ||
        (pixel.x < bottom_left_corner.x && pixel.y > bottom_left_corner.y && distance(pixel, bottom_left_corner) > corner_rounding) ||
        (pixel.x > bottom_right_corner.x && pixel.y > bottom_right_corner.y && distance(pixel, bottom_right_corner) > corner_rounding)
    ){
        color = vec4(0.0, 0.0, 0.0, 0.0);
    }
    else color = spriteColor;
}
