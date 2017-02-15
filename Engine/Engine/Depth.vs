cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXTURE0;
};

PixelInputType DepthVertexShader(VertexInputType aInput)
{
    PixelInputType output;

    //Float 4 cast
    aInput.position.w = 1.0f;

    //Calc pos of verts in world, view and proj.
    output.position = mul(aInput.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.depthPosition = output.position;

    return output;
}