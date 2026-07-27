#version 450 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec4 spriteColor;
uniform vec2 sample_position;
uniform vec2 sample_size_not_normalised;

void main(){

    vec2 pixel = TexCoords * sample_size_not_normalised;

    vec2 top_left_corner = vec2(64.0, 64.0);
    vec2 top_right_corner = vec2(sample_size_not_normalised.x-64.0, 64.0);
    vec2 bottom_left_corner = vec2(64.0, sample_size_not_normalised.y-64.0);
    vec2 bottom_right_corner = vec2(sample_size_not_normalised.x-64.0, sample_size_not_normalised.y-64.0);

    if((pixel.x < top_left_corner.x && pixel.y < top_left_corner.y && distance(pixel, top_left_corner) > 64.0) ||
        (pixel.x > top_right_corner.x && pixel.y < top_right_corner.y && distance(pixel, top_right_corner) > 64.0) ||
        (pixel.x < bottom_left_corner.x && pixel.y > bottom_left_corner.y && distance(pixel, bottom_left_corner) > 64.0) ||
        (pixel.x > bottom_right_corner.x && pixel.y > bottom_right_corner.y && distance(pixel, bottom_right_corner) > 64.0)
    ){
        color = vec4(0.0, 0.0, 0.0, 0.0);
    }
    else color = spriteColor * texture(image, TexCoords);

}
