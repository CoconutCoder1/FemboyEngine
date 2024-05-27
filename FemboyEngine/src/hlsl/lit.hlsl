cbuffer TestBuffer : register(b0) {
    float3 offset;
};

float4 VSMain(float3 position : POSITION) : SV_POSITION
{
    return float4(position + offset, 1.f);
}

float4 PSMain() : SV_Target
{
    return 1.f;
}