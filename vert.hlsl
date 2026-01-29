[[vk::binding(0, 0)]]
cbuffer Ubo : register(b0) {
    float time;
    float4x4 vp;
};

struct VSInput {
    [[vk::location(0)]] float3 in_pos   : POSITION;
    [[vk::location(1)]] float3 in_color : COLOR;
    [[vk::location(2)]] float3 position : TEXCOORD0;
};

struct VSOutput {
    float4 pos : SV_Position;
    [[vk::location(0)]] float3 frag_color : COLOR;
};

VSOutput main(VSInput input) {
    VSOutput output;
    output.pos = mul(vp, float4(input.in_pos + input.position, 1.0f));
    output.frag_color = input.in_color;
    return output;
}

