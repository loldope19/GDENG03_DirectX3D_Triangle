struct PSInput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float4 color1 : COLOR1;
};

cbuffer constant : register(b0)
{
    unsigned int m_time;
}

float4 main(PSInput input) : SV_TARGET {
    return lerp(input.color, input.color1, (sin(m_time / 100.0f) + 1.0f) / 2.0f);
} 