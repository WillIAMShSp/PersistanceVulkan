#version 450


layout(location = 0) in vec2 uvcords;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texsampler;

void main() {

    outColor = vec4(texture(texsampler, vec2(uvcords.x, 1 - uvcords.y)));
    //outColor = vec4(uvcords, 0, 1);

    //outColor = vec4(0.0, 1.0, 0.2, 1.0);
}