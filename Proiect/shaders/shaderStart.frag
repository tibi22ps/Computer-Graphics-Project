#version 410 core

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 passTexture;

out vec4 fragmentColour;
uniform sampler2D diffuseTexture;
uniform bool enableFog;

float computeFog()
{
    float fogDensity = 0.001;
    float fragmentDistance = length(fragPosition);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
    return clamp(fogFactor, 0.0, 1.0);
}

void main() {
    vec4 texColor = texture(diffuseTexture, passTexture);

    if (enableFog)
    {
        float fogFactor = computeFog();
        vec4 fogColor = vec4(0.5, 0.5, 0.5, 1.0);

        texColor.rgb = mix(texColor.rgb, fogColor.rgb, fogFactor);
    }

    fragmentColour = texColor;
}