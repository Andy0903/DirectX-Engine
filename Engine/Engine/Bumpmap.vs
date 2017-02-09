cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

PixelInputType BumpMapVertexShader(VertexInputType aInput)
{
    PixelInputType output;
    float4 worldPosition;

    // Change the position vector to be 4 units for proper matrix calculations.
    aInput.position.w = 1.0f;
    
    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(aInput.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = aInput.tex;

    // Calculate the ... vector against the world matrix and then normalize the final val.
    output.normal = mul(aInput.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    output.tangent = mul(aInput.tangent, (float3x3)worldMatrix);
    output.tangent = normalize(output.tangent);
    output.binormal = mul(aInput.binormal, (float3x3) worldMatrix);
    output.binormal = normalize(output.binormal);

    return output;
}