struct VSInput {
    float3 position : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
};

struct PSInput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 worldPos : TEXCOORD0;
    float3 normal : TEXCOORD1;
};

cbuffer CameraBuffer : register(b0) {
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

cbuffer LightBuffer : register(b1) {
    float3 lightPosition;
    float lightIntensity;
    float4 lightColor;
};

PSInput main(VSInput input) {
    PSInput output;
    
    float4 worldPos = float4(input.position, 1.0f);
    
    output.position = mul(worldPos, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.color = input.color;
    output.worldPos = worldPos.xyz;
    output.normal = input.normal;
    
    return output;
} 