#version 450 core
in vec2 texture_coords;

uniform sampler2D sprite;
uniform float offsetx;
uniform float offsety;

out vec4 frag_color; 

uniform sampler2D texture1;

void main()
{
    vec4 texel = texture(sprite, vec2(texture_coords.x + offsetx, (texture_coords.y + offsety) * -1));
    if(texel.a < 0.5) {
        discard;
    }

    frag_color = texel;
}