cbuffer TestBuffer : register(b0) {
    float4x4 transform;
};

float4 VSMain(float3 position : POSITION) : SV_POSITION
{
    return mul(float4(position, 1.f), transform);
}

float4 PSMain() : SV_Target
{
    return 1.f;
}