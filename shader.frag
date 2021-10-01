#version 330 core

out vec4 FragColor;

// uniform sampler2D gTexture;
uniform int width;
uniform int height;

in vec3 FragPos;
in vec3 FragNorm;
in vec2 FragTex;

float part_of(int x, int off) {
    return float((x >> off) & 255) / 255.0;
}

vec4 int_to_color(int x) {
    return vec4(part_of(x, 24), part_of(x, 16), part_of(x, 8), part_of(x, 0));
}

void main() {
    int x = int(gl_FragCoord.x);
    int y = int(gl_FragCoord.y);
    int res = 65536;
    res = y * width + x;
    FragColor = int_to_color(res);
}
