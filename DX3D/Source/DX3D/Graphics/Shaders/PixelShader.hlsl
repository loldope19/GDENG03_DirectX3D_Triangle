struct PS_INPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
};

cbuffer LightConstants : register(b1)
{
    float2 lightScreenPos;      // mouse position in client area pixel coordinates
    float2 screenResolution;    // client area width and height
    // float lightIntensity;    // OPTIONAL: if u want to control brightness
};

float4 main(PS_INPUT input) : SV_Target
{
    float2 pixelScreenCoord = input.Position.xy;
    float dist = distance(pixelScreenCoord, lightScreenPos);        // check distance b/w pixel and cursor
    
    float attenuation = 1.0 / (1.0 + 0.005 * dist + 0.00001 * dist * dist);
    attenuation = saturate(attenuation * 2.5f);                     // saturate means clamp apparently

    float4 outputColor = input.Color;
    outputColor.rgb *= attenuation; // modulate object color by light

    return outputColor;
}