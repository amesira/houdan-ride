//===================================================
// shader_vertex_fullscreen_quad.hlsl
//
// ・フルスクリーンクアッド用頂点シェーダー
// ・頂点バッファ・インデックスバッファを使用せずに描画できます
// 
// Author：Hayato Ushimaru
// Date  ：2025/11/19
//===================================================

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

VS_OUTPUT main(uint vI : SV_VertexID)
{
	VS_OUTPUT vs_out;
	
    vs_out.tex = float2(vI & 1, vI >> 1);
	vs_out.pos = float4((vs_out.tex.x - 0.5f) * 2, -(vs_out.tex.y - 0.5f) * 2, 0, 1);
	return vs_out;
}