#version 430 core

struct Particle {
    vec4 position;   // w is size
    vec4 velocity;   // w is burst flag
    vec4 color;
    float life;
    float maxLife;
    vec2 flags;      // x = 1 (immortal)
};

layout(std430, binding = 0) readonly buffer ParticleBuffer {
    Particle particles[];
};

layout(std430, binding = 5) readonly buffer TrailBuffer {
    vec4 trailPoints[];
};

uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos_worldspace;

uniform uint  maxParticles;
uniform uint  trailSamples;   
uniform uint  trailHead; // slot written most recently
uniform float trailWidth;
uniform float trailAlpha;

out vec4 fragColor;
out vec2 localUV; // x = -1..1 across the ribbon, y = 0..1 along it

vec4 fetchSample(uint particleIndex, uint age)
{
    uint slot = (trailHead + trailSamples - age) % trailSamples; //Ring Buffer for slot selection
    return trailPoints[slot * maxParticles + particleIndex];
}

void main()
{
    uint particleIndex = uint(gl_InstanceID);
    Particle p = particles[particleIndex];

    uint  vert = uint(gl_VertexID);//takes vertex ID of the trail vertex we are generating
    uint  age  = vert >> 1u; // bitwise right shift, basically dividing by 2.
    float side = ((vert & 1u) == 0u) ? -1.0 : 1.0; //determine left/right side

    vec4  here  = fetchSample(particleIndex, age); 
    vec3  P     = here.xyz;//gets current trail position

    bool valid = (p.maxLife > 0.0) && (here.w == 1.0); //Retired slots are not valid

    vec3 newer = fetchSample(particleIndex, age == 0u ? 0u : age - 1u).xyz;
    vec3 older = fetchSample(particleIndex, min(age + 1u, trailSamples - 1u)).xyz;
    vec3 tangent = newer - older;// Tangent from neighbouring samples.

    if (dot(tangent, tangent) < 0.0001) { //in case tangent value is zero or close to zero
        tangent = p.velocity.xyz;
        if (dot(tangent, tangent) < 0.0001) {
            tangent = vec3(0.0, 1.0, 0.0);
        }
    }
    tangent = normalize(tangent);

    vec3 viewDir = cameraPos_worldspace - P;
    if (dot(viewDir, viewDir) < 0.0001) viewDir = vec3(0.0, 0.0, 1.0);
    viewDir = normalize(viewDir);

    vec3 sideVec = cross(tangent, viewDir); // direction across the width of the trail
    if (dot(sideVec, sideVec) < 0.0001) { // In case sideVec is pointed towards the camera
        sideVec = cross(tangent, vec3(0.0, 0.0, 1.0));
        if (dot(sideVec, sideVec) < 0.0001)
            sideVec = vec3(1.0, 0.0, 0.0);
    }
    sideVec = normalize(sideVec);

    float t = (trailSamples > 1u) ? float(age) / float(trailSamples - 1u) : 0.0;
    float taper = 1.0 - t; // narrow and fade toward the tail

    float halfWidth = valid ? trailWidth * taper * p.position.w : 0.0;
    float alpha     = valid ? trailAlpha * taper * taper : 0.0;

    vec3 worldPos = P + sideVec * (side * halfWidth);
    gl_Position = projection * view * vec4(worldPos, 1.0);

    fragColor = vec4(p.color.rgb, p.color.a * alpha);
    localUV   = vec2(side, t);
}
