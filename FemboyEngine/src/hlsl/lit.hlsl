cbuffer TestBuffer : register(b0) {
    float4x4 projMat;
    float4x4 viewMat;
};

float4 VSMain(float3 position : POSITION) : SV_POSITION
{
    return mul(mul(float4(position, 1.f), viewMat), projMat);
}

float4 PSMain() : SV_Target
{
    return 1.f;
}