//=============================================================================
// color.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Transforms and colors geometry.
//=============================================================================

cbuffer cbPerObject
{
	float4x4 gWVP; 
	//float gTime;
};

/*cbuffer cbPerFrame
{
	float gTime;
};*/

void VS(float3 iPosL  : POSITION,
        float4 iColor : COLOR,
        out float4 oPosH  : SV_POSITION,
        out float4 oColor : COLOR)
{
	// making the vertices dance 
	/*iPosL.xy += 0.5f * sin(iPosL.x) * sin(3*gTime);
	iPosL.z = 0.6f + 0.4f * sin(2*gTime);*/

	// Transform to homogeneous clip space.
	oPosH = mul(float4(iPosL, 1.0f), gWVP);
	
	// Just pass vertex color into the pixel shader.
    oColor = iColor;
}

float4 PS(float4 posH  : SV_POSITION,
          float4 color : COLOR) : SV_Target
{
    return color;
}

technique10 ColorTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
