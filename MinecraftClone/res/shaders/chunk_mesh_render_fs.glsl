#version 460 core

struct Light
{
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    vec3 Position; // Emulated position for directional light.
    vec3 Direction;
};

struct Material
{
    sampler2D Diffuse;
    sampler2D Specular;
    float Shininess;
}; 

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosViewSpace;
    vec4 FragPosLightSpace;
    vec3 DebugColor;
} fs_in;

uniform Light uLight;
uniform Material uMaterial;
uniform sampler2D uShadowMap;
uniform float uMinimumShadowBias;
uniform float uFogRadius;
uniform float uFogDensity;
uniform vec3 uFogColor;
uniform vec3 uViewPos;

out vec4 FragColor;

float calcShadowingFactor(vec3 lightDir)
{
    // Perform perspective division.
    vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;

    // Transform to [0,1] range.
    projCoords = projCoords * 0.5 + 0.5;

    // Get closest depth value from light's perspective.
    // float closestDepth = texture(uShadowMap, projCoords.xy).r;

    // Get depth of current fragment from light's perspective.
    float currentDepth = projCoords.z;

    // Check whether current fragment position is in shadow.
    if(projCoords.z > 1.0)
    {
        return 0.0;
    }
    else
    {
        // WARNING:
        //
        // Applying a bias to prevent shadow acne. Choosing the correct bias value(s) requires
        // some tweaking as this will be different for each scene, but most of the time it's simply
        // a matter of slowly incrementing the bias until all acne is removed.
        //
        float bias = max((10 * uMinimumShadowBias) * (1.0 - dot(fs_in.Normal, lightDir)), uMinimumShadowBias);
        float factor = 0.0;

        vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);

        // Applying percentage-closer filtering.
        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;

                factor += (currentDepth - bias > pcfDepth ? 1.0 : 0.0);
            }    
        }

        factor /= 9.0;

        return factor;
    }
}

float calcFogFactor()
{
    float distance2Frag = length(fs_in.FragPosViewSpace);
    float exp2Factor = 1.0;

    if (distance2Frag > uFogRadius)
    {
        exp2Factor = exp(-pow((distance2Frag - uFogRadius) * uFogDensity, 2.0));
    }

    return clamp(exp2Factor, 0.0, 1.0);
}

void main()
{
    vec3 fragNormal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(-uLight.Direction); // normalize(uLight.Position - fs_in.FragPos);
    vec3 viewDir = normalize(uViewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diffuseStr = max(dot(fragNormal, lightDir), 0.0);
    float specularStr = pow(max(dot(fragNormal, halfwayDir), 0.0), uMaterial.Shininess);

    // Calculating "Blinn-Phong Lighting" components.

    vec3 ambient = uLight.Ambient * vec3(texture(uMaterial.Diffuse, fs_in.TexCoords));
    vec3 diffuse = uLight.Diffuse * (diffuseStr * vec3(texture(uMaterial.Diffuse, fs_in.TexCoords)));
    vec3 specular = uLight.Specular * (specularStr * vec3(texture(uMaterial.Specular, fs_in.TexCoords)));

    // Claculating shadowing.

    float shadowingFactor = calcShadowingFactor(lightDir);

    vec4 pixelColor = vec4(ambient + ((1.0 - shadowingFactor) * (diffuse + specular)), 1.0);

    // Calculating fog.

    float fogFactor = calcFogFactor();

    FragColor = mix(vec4(uFogColor, 1.0), pixelColor, fogFactor);
}
