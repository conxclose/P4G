#include "Constants.hlsl"

VertexLineOut main(VertexLineIn vin)
{
	VertexLineOut vout;
	
	// Transform to 2D space.
	//vout.PosH = float4(vin.PosH, 1.0f);
	vout.PosH = mul(gWorldViewProj, float4(vin.PosH, 1.0f));

	vout.Colour = vin.Colour;

	return vout;
}

