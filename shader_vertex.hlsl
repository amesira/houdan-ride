/*==============================================================================

   2D描画用頂点シェーダー [shader_vertex.hlsl]
--------------------------------------------------------------------------------

==============================================================================*/

// 定数バッファ
cbuffer Buffer0 : register(b0)
{
    float4x4 mtx;
}
cbuffer Buffer1:register(b1)
{
    float4x4 World;
}

// 頂点データ受け渡しのための構造体作るよ
// ※入出力の構造体は、メンバ変数の順番を一致させること
// 入力用頂点構造体
struct VS_INPUT
{
    float4 posL : POSITION0; // 頂点座標
    float4 normal : NORMAL0; // 頂点法線
    float4 color : COLOR0; // 頂点カラー（R,G,B,A）
    float2 texcoord : TEXCOORD0;
};

// 出力用頂点構造体
struct VS_OUTPUT
{
    float4 posH : SV_Position; // 変換済み頂点座標
    float4 color : COLOR0; // 頂点カラー
    float2 texcoord : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT vs_in)
{
    VS_OUTPUT vs_out;
    
    // 頂点を行列変換
    vs_out.posH = mul(vs_in.posL, mtx);
    
    // 頂点カラー、テクスチャ座標はそのまま
    vs_out.color = vs_in.color;
    vs_out.texcoord = vs_in.texcoord;
    
    return vs_out;
}