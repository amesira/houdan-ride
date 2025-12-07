/*==============================================================================

   ボーンなし3Dモデル描画用頂点シェーダー [shader_vertex_3dmodel_static.hlsl]
--------------------------------------------------------------------------------

==============================================================================*/

// 1. プロジェクション変換行列
cbuffer ProjectionCB : register(b0)
{
    matrix proj; // 4x4 行列
};

// 2. ビュー変換行列（毎フレーム更新）
cbuffer ViewCB : register(b1)
{
    matrix view; // 4x4 行列
};

//入力用頂点構造体
struct VS_INPUT
{
    // スロット0 (通常頂点バッファ)
    float4 posL : POSITION0; //頂点座標
    float4 normal : NORMAL0; //頂点法線
    float2 texcoord : TEXCOORD0; //テクスチャ座標
	
    // スロット1 (インスタンスバッファ)
    float4 InstanceWorldRow1 : INSTANCE_WORLD_ROW0;
    float4 InstanceWorldRow2 : INSTANCE_WORLD_ROW1;
    float4 InstanceWorldRow3 : INSTANCE_WORLD_ROW2;
    float4 InstanceWorldRow4 : INSTANCE_WORLD_ROW3;
};

//出力用頂点構造体
struct VS_OUTPUT
{
    float4 posH : SV_POSITION; //頂点座標
	float4 normal : NORMAL0;
    float2 texcoord : TEXCOORD0; //テクスチャ座標
};


//=============================================================================
// 頂点シェーダ
//=============================================================================
VS_OUTPUT main(VS_INPUT vs_in)
{
    VS_OUTPUT vs_out;
	
	//頂点を行列で変換
	
	// ワールド変換
    float4x4 world = float4x4(
        vs_in.InstanceWorldRow1,
        vs_in.InstanceWorldRow2,
        vs_in.InstanceWorldRow3,
        vs_in.InstanceWorldRow4
    );
    
    vs_in.posL = mul(vs_in.posL, world);
	vs_out.normal = float4(mul(vs_in.normal, world).xyz, 0.0f);
	
	// ビュー変換
    vs_in.posL = mul(vs_in.posL, view);
	
	// プロジェクション変換
    vs_out.posH = mul(vs_in.posL, proj);

    vs_out.texcoord = vs_in.texcoord;
	
    return vs_out;
}