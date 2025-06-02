struct PSInput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 worldPos : TEXCOORD0;
    float3 normal : TEXCOORD1;
};

cbuffer LightBuffer : register(b1) {
    float3 lightPosition;
    float lightIntensity;
    float4 lightColor;
};

float4 main(PSInput input) : SV_TARGET {
    // Calculate light direction
    float3 lightDir = normalize(lightPosition - input.worldPos);
    
    // Calculate diffuse lighting
    float diffuse = max(dot(normalize(input.normal), lightDir), 0.0f);
    
    // Combine base color with lighting
    float4 finalColor = input.color * lightColor * (diffuse * lightIntensity);
    
    // Add ambient light to prevent complete darkness
    finalColor += input.color * 0.2f;
    
    return finalColor;
} 