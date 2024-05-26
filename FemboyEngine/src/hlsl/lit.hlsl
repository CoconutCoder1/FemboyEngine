float4 VSMain(float3 position : POSITION) : SV_POSITION
{
    return float4(position, 1.f);
}

float4 PSMain() : SV_Target
{
    return 1.f;
}