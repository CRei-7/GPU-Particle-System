#version 430 core

in vec4 fragColor;
in vec2 localUV;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

uniform float glowIntensity;
uniform float quadRadius;

void main(){
	float dist = length(localUV) / quadRadius; // Calculate distance from center of quad
	float falloff = smoothstep(1.0, 0.0, dist); // Smooth falloff based on distance
	falloff = pow(falloff, 1.5);

	vec3 hdrColor = fragColor.rgb * glowIntensity;
	vec4 outColor = vec4(hdrColor * falloff, falloff * fragColor.a);

	FragColor = outColor;

	float brightness = dot(outColor.rgb, vec3(0.2126, 0.7152, 0.0722)); // Luminance Coefficients, taken from learnopengl.com
	if(brightness > 1.0){
		BrightColor = vec4(outColor.rgb, outColor.a);
	}else{
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}