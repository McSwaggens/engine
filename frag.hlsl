struct PSInput {
    [[vk::location(0)]] float3 vertex_color : COLOR;
};

struct PSOutput {
    [[vk::location(0)]] float4 color : SV_Target0;
};

PSOutput main(PSInput input) {
    PSOutput output;
    output.color = float4(input.vertex_color, 1.0f);
    return output;
}
