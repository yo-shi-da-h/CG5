float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}

struct VertexShaderOutout
{
	float4 position : SV_POSITION;
};

struct VertexShaderInput
{
	float4 position : POSITION0;
    
};

VertexShaderOutout main( VertexShaderInput input )
{
    VertexShaderOutout output;
    output.position = input.position;
    return output;
}