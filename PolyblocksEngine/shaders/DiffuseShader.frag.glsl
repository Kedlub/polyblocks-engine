#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform vec3 LightPosition_worldspace;

void main() {
    //Will be better if put as uniforms
    vec3 LightColor = vec3(1, 1, 1);
    float LightPower = 50.0f;

    //Material properties
    vec3 MaterialDiffuseColor = texture(myTextureSampler, UV).rgb;
    vec3 MaterialAmbientColor = vec3(0.5, 0.5, 0.5) * MaterialDiffuseColor;
    vec3 MaterialSpecularColor = vec3(0.3, 0.3, 0.3);

    float distance = length(LightPosition_worldspace - Position_worldspace);

    // Normal of the computed fragment, in camera space
    vec3 n = normalize(Normal_cameraspace);
    // Direction of the light (from the fragment to the light)
    vec3 l = normalize(LightDirection_cameraspace);
    float cosTheta = clamp(dot(n, l), 0, 1);

    // Eye vector (towards the camera)
    vec3 E = normalize(EyeDirection_cameraspace);
    // Direction in which the triangle reflects the light
    vec3 R = reflect(-l, n);
    // Cosine of the angle between the Eye vector and the Reflect vector,
    // clamped to 0
    //  - Looking into the reflection -> 1
    //  - Looking elsewhere -> < 1
    float cosAlpha = clamp(dot(E, R), 0, 1);

    // Output color = color of the texture at the specified UV
    // color = texture( myTextureSampler, vec2(UV.x, UV.y) ).rgb;
    color.xyz = MaterialAmbientColor +
    MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance * distance);
    MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha, 5) / (distance * distance);

    color.a = 0.3;

}