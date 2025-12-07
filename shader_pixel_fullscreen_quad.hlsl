//===================================================
// shader_pixel_fullscreen_quad.hlsl
//
// ・フルスクリーンクアッド用頂点シェーダー
// ・頂点バッファ・インデックスバッファを使用せずに描画できます
// 
// Author：Hayato Ushimaru
// Date  ：2025/11/19
//===================================================
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

struct PS_INPUT // VS_OUTPUTと同じ内容
{
    float4 posH : SV_Position; // ピクセルの座標
    float2 texcoord : TEXCOORD0; // テクスチャ座標
};

float4 main(PS_INPUT ps_in) : SV_TARGET
{
	float4 col = g_Texture.Sample(g_SamplerState, ps_in.texcoord);
	
	return col;
}