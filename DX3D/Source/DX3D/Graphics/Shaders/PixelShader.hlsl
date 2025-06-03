struct PS_INPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
};

cbuffer LightConstants : register(b1)
{
    float2 lightScreenPos;      // mouse position in client area pixel coordinates
    float2 screenResolution;    // client area width and height
    // float lightIntensity;    // Optional: if you want to control brightness
};

float4 main(PS_INPUT input) : SV_Target
{
    // SV_Position.xy are pixel coordinates relative to the render target.
    // No conversion to NDC is typically needed here if viewport is set correctly.
    float2 pixelScreenCoord = input.Position.xy;

    float dist = distance(pixelScreenCoord, lightScreenPos);

    // Attenuation: Decrease light intensity with distance
    // These values (0.005, 0.00001, 2.5) are examples, tweak them for desired effect
    float attenuation = 1.0 / (1.0 + 0.005 * dist + 0.00001 * dist * dist);
    attenuation = saturate(attenuation * 2.5f); // Amplify and clamp

    float4 outputColor = input.Color;
    outputColor.rgb *= attenuation; // Modulate object's color by light

    return outputColor;
}