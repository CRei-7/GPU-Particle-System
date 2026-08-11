#version 430 core

in vec4 fragColor;
in vec2 localUV; // x = -1..1 across the ribbon, y = 0..1 along it

uniform float glowIntensity;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;  

void main()
{
    float d = abs(localUV.x);
    float falloff = 1.0 - smoothstep(0.0, 1.0, d);// This is for edge fade out
    falloff *= falloff;

    float a = fragColor.a * falloff;
    if (a < 0.001) discard;

    vec3 color = fragColor.rgb * falloff * glowIntensity;
    FragColor = vec4(color, a);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, a);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
}
