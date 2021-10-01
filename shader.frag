#version 330 core

out vec4 FragColor;

// uniform sampler2D gTexture;
uniform int width;
uniform int height;

in vec3 FragPos;
in vec3 FragNorm;
in vec2 FragTex;

uniform sampler2D one;
uniform sampler2D two;

int color_to_int(vec4 c) {
    int res = 0;
    for (int i = 0; i < 4; i++) {
        res = res << 8;
        res = res | (int(c[3-i] * 255.) & 255);
    }
    return res;
}

float part_of(int x, int off) {
    return float((x >> off) & 255) / 255.;
}

vec4 int_to_color(int x) {
    return vec4(part_of(x, 0), part_of(x, 8), part_of(x, 16), part_of(x, 24));
}

vec2 normal_size(vec2 uv) {
    return uv / vec2(float(width), float(height));
}

int getCell(sampler2D t, int x, int y) {
    vec2 uv = normal_size(vec2(float(x), float(y)));
    return color_to_int(texture(t, uv));
}

int calc(int x, int y) {
    int a = getCell(one, x, y);
    int b = getCell(two, x, y);
    return b+a;
}

void main() {
    int x = int(gl_FragCoord.x);
    int y = int(gl_FragCoord.y);
    int res = calc(x, y);
    FragColor = int_to_color(res);
}
