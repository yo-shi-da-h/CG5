float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}

struct PixelShaderOutput
{
	float4 color : SV_TARGET0;
};

PixelShaderOutput
main( )
{
    PixelShaderOutput output;
    output.color = float4(1, 1, 1, 1); 
    return output;
}