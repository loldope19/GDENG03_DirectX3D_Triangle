cbuffer AnimationConstantBuffer : register(b0)
{
    float interpolationFactor;
    float3 padding;
};

struct VS_INPUT
{
    float3 posA : POSITIONA;
    float4 colorA : COLORA;
    float3 posB : POSITIONB;
    float4 colorB : COLORB;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    // lerp(a, b, t) = a + t*(b-a)
    float3 interpolatedPosition = lerp(input.posA, input.posB, interpolationFactor);
    output.position = float4(interpolatedPosition, 1.0f);
    
    float4 interpolatedColor = lerp(input.colorA, input.colorB, interpolationFactor);
    output.color = interpolatedColor;

    return output;
}