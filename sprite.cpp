//===================================================
// sprite.cpp [スプライト制御]
// 
// 2Dスプライトの初期化、描画、破棄などの管理を行う。
// ※DirectX 11 を使用している。
// 
// Author：Miu Kitamura
// Date  ：2025/06/17
//===================================================
#include "sprite.h"

#include "debug_ostream.h"
#include "shader.h"

//===================================================
// グローバル変数（Direct3Dデバイス／バッファ関連）
//===================================================

// 使用可能な最大頂点数
static constexpr int NUM_VERTEX = 4;

// 頂点バッファ：スプライト描画
static ID3D11Buffer* g_pVertexBuffer = nullptr;

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

//===================================================
// スプライト初期化
//===================================================
void InitializeSprite() {

	// デバイスの取得
	g_pDevice = Direct3D_GetDevice();
	g_pContext = Direct3D_GetDeviceContext();

	//----------------------------------------------------
	// 頂点バッファ生成
	//----------------------------------------------------
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * NUM_VERTEX; // 格納する最大頂点数
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_pDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);
}

void LoadTexture(ID3D11ShaderResourceView** texture, const wchar_t* fileName) {
	TexMetadata metadata;
	ScratchImage image;

	HRESULT hr = LoadFromWICFile(fileName, WIC_FLAGS_NONE, &metadata, image);
	if (FAILED(hr)) {
		hal::dout << "テクスチャ読み込み失敗" << std::endl;
		return;
	}
	hr = CreateShaderResourceView(g_pDevice, image.GetImages(), image.GetImageCount(), metadata, texture);
	if (FAILED(hr)) {
		hal::dout << "シェーダーリソース生成失敗" << std::endl;
		return;
	}
	assert(*texture); // 読み込み失敗時にダイアログを表示
}

//===================================================
// スプライト終了
//===================================================
void FinalizeSprite() {
	g_pVertexBuffer->Release();
}

//===================================================
// スプライト描画
//===================================================
void DrawSprite(XMFLOAT4 color, XMFLOAT4 uvRect, XMFLOAT3 normal)
{
	//----------------------------------------------------
	// 頂点バッファをロック
	//----------------------------------------------------
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得 memcpyとかで後から詰めるやり方もある
	Vertex* v = (Vertex*)msr.pData;

	// スプライト1つ当たりの比率
	float w = uvRect.z;
	float h = uvRect.w;

	// テクスチャの左上座標を計算
	DirectX::XMFLOAT2 uv;
	uv.x = uvRect.x;
	uv.y = uvRect.y;

	//----------------------------------------------------
	// スプライトの頂点設定
	//----------------------------------------------------
	XMFLOAT2 halfSize = { 0.5f, 0.5f };
	v[0].position = { -halfSize.x,-halfSize.y,0.0f };
	v[0].color = color;
	v[0].texCoord = { uv.x,uv.y };

	v[1].position = { halfSize.x,-halfSize.y,0.0f };
	v[1].color = color;
	v[1].texCoord = { uv.x + w,uv.y };

	v[2].position = { -halfSize.x,halfSize.y,0.0f };
	v[2].color = color;
	v[2].texCoord = { uv.x ,uv.y + h };

	v[3].position = { halfSize.x,halfSize.y,0.0f };
	v[3].color = color;
	v[3].texCoord = { uv.x + w,uv.y + h };

    // 法線の設定
	for (int i = 0; i < 4; i++) {
		v[i].normal = normal;
    }

	//----------------------------------------------------
	// ロック解除 → 描画準備
	//----------------------------------------------------
	g_pContext->Unmap(g_pVertexBuffer, 0);

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定 トライアングルストリップ
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//----------------------------------------------------
	// 描画命令
	//----------------------------------------------------
	g_pContext->Draw(4, 0); // 表示に使用する頂点数を指定
}
