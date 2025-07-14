#version 450
layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 tex_coord;
layout(location = 0) out vec4 out_color;

layout(binding = 1) uniform sampler2D _picture;

void main() {
    //out_color = vec4(frag_color, 1.0);
    out_color = texture(_picture, tex_coord);
}
