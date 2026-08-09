#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aOffset;
layout (location = 2) in vec4 aInstanceColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 cameraRight_worldspace;
uniform vec3 cameraUp_worldspace;

out vec4 fragColor;
out vec2 localUV;//This is for radial glow falloff

void main(){
	vec3 worldPos = aOffset + cameraRight_worldspace * aPos.x + cameraUp_worldspace * aPos.y;//This is the billboarding part, we calculate the world position of the vertex by adding the offset and the camera right and up vectors multiplied by the vertex position in the quad
	gl_Position = projection * view * vec4(worldPos, 1.0);
	fragColor = aInstanceColor;

	localUV = aPos.xy;
}