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
void DrawSprite(XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color)
{
	g_pDevice = Direct3D_GetDevice();
	g_pContext = Direct3D_GetDeviceContext();

	//----------------------------------------------------
	// 頂点バッファをロック
	//----------------------------------------------------
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得 memcpyとかで後から詰めるやり方もある
	Vertex* v = (Vertex*)msr.pData;

	// 指定の位置に指定のサイズ、色の四角形を描画する
	XMFLOAT2 halfSize = { size.x / 2.0f,size.y / 2.0f };
	v[0].position = { pos.x - halfSize.x,pos.y - halfSize.y,0.0f };
	v[0].color = color;
	v[0].texCoord = { 0.0f,0.0f };

	v[1].position = { pos.x + halfSize.x,pos.y - halfSize.y,0.0f };
	v[1].color = color;
	v[1].texCoord = { 1.0f,0.0f };

	v[2].position = { pos.x - halfSize.x,pos.y + halfSize.y,0.0f };
	v[2].color = color;
	v[2].texCoord = { 0.0f,1.0f };

	v[3].position = { pos.x + halfSize.x,pos.y + halfSize.y,0.0f };
	v[3].color = color;
	v[3].texCoord = { 1.0f,1.0f };

	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定 トライアングルストリップ
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴン描画命令発行
	g_pContext->Draw(4, 0);
}

//===================================================
// スプライト描画Ex（スプライト分割可能）
//===================================================
void DrawSpriteEx(XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color, int bno, int wc, int hc)
{
	g_pDevice = Direct3D_GetDevice();
	g_pContext = Direct3D_GetDeviceContext();

	//----------------------------------------------------
	// 頂点バッファをロック
	//----------------------------------------------------
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得 memcpyとかで後から詰めるやり方もある
	Vertex* v = (Vertex*)msr.pData;

	//----------------------------------------------------
	// スプライトの頂点設定
	//----------------------------------------------------
	XMFLOAT2 halfSize = { size.x / 2.0f,size.y / 2.0f };

	// スプライト1つ当たりの比率
	float w = 1.0f / (float)wc;
	float h = 1.0f / (float)hc;

	v[0].position = { pos.x - halfSize.x,pos.y - halfSize.y,0.0f };
	v[0].color = color;
	v[0].texCoord = { (float)(bno % wc) * w,(float)(bno / wc) * h };

	v[1].position = { pos.x + halfSize.x,pos.y - halfSize.y,0.0f };
	v[1].color = color;
	v[1].texCoord = { (float)(bno % wc) * w + w,(float)(bno / wc) * h };

	v[2].position = { pos.x - halfSize.x,pos.y + halfSize.y,0.0f };
	v[2].color = color;
	v[2].texCoord = { (float)(bno % wc) * w ,(float)(bno / wc) * h + h };

	v[3].position = { pos.x + halfSize.x,pos.y + halfSize.y,0.0f };
	v[3].color = color;
	v[3].texCoord = { (float)(bno % wc) * w + w,(float)(bno / wc) * h + h };

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

//===================================================
// スプライト描画：スクロール用
//===================================================
void DrawSpriteScroll(XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color, XMFLOAT2 texcoord)
{
	g_pDevice = Direct3D_GetDevice();
	g_pContext = Direct3D_GetDeviceContext();

	//----------------------------------------------------
	// 頂点バッファをロック
	//----------------------------------------------------
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得 memcpyとかで後から詰めるやり方もある
	Vertex* v = (Vertex*)msr.pData;

	//----------------------------------------------------
	// 頂点設定
	//----------------------------------------------------
	XMFLOAT2 halfSize = { size.x / 2.0f,size.y / 2.0f };
	v[0].position = { pos.x - halfSize.x,pos.y - halfSize.y,0.0f };
	v[0].color = color;
	v[0].texCoord = { texcoord.x,texcoord.y };

	v[1].position = { pos.x + halfSize.x,pos.y - halfSize.y,0.0f };
	v[1].color = color;
	v[1].texCoord = { texcoord.x + 1.0f,texcoord.y };

	v[2].position = { pos.x - halfSize.x,pos.y + halfSize.y,0.0f };
	v[2].color = color;
	v[2].texCoord = { texcoord.x,texcoord.y + 1.0f };

	v[3].position = { pos.x + halfSize.x,pos.y + halfSize.y,0.0f };
	v[3].color = color;
	v[3].texCoord = { texcoord.x + 1.0f,texcoord.y + 1.0f };

	//----------------------------------------------------
	// 頂点バッファのロック解除 → 描画設定
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
	g_pContext->Draw(4, 0);
}

//===================================================
// スプライト描画Ex（スプライト分割 + 回転）
//===================================================
void DrawSpriteExRotation(XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color, int bno, int wc, int hc, float rad)
{
	g_pDevice = Direct3D_GetDevice();
	g_pContext = Direct3D_GetDeviceContext();

	//----------------------------------------------------
	// 頂点バッファをロック
	//----------------------------------------------------
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得 memcpyとかで後から詰めるやり方もある
	Vertex* v = (Vertex*)msr.pData;

	//----------------------------------------------------
	// スプライトの頂点設定
	//----------------------------------------------------
	XMFLOAT2 halfSize = { size.x / 2.0f,size.y / 2.0f };

	// スプライト1つ当たりの比率
	float w = 1.0f / (float)wc;
	float h = 1.0f / (float)hc;

	// テクスチャの左上座標を計算
	DirectX::XMFLOAT2 uv;
	uv.x = (bno % wc) * w;
	uv.y = (bno / wc) * h;

	v[0].position = {-halfSize.x,-halfSize.y,0.0f };
	v[0].color = color;
	v[0].texCoord = {uv.x,uv.y};

	v[1].position = { halfSize.x,-halfSize.y,0.0f };
	v[1].color = color;
	v[1].texCoord = {uv.x+ w,uv.y };

	v[2].position = { -halfSize.x,halfSize.y,0.0f };
	v[2].color = color;
	v[2].texCoord = { uv.x ,uv.y + h };

	v[3].position = { halfSize.x,halfSize.y,0.0f };
	v[3].color = color;
	v[3].texCoord = {uv.x + w,uv.y + h };

	// 回転処理
	float co = cosf(rad);
	float si = sinf(rad);
	for (int i = 0; i < 4; i++) {
		DirectX::XMFLOAT3 relPos = v[i].position;
		v[i].position.x = (relPos.x * co - relPos.y * si) + pos.x;
		v[i].position.y = (relPos.x * si + relPos.y * co) + pos.y;
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

//===================================================
// スプライト描画（行列使用ver）
//===================================================
void DrawSprite(XMFLOAT2 size, XMFLOAT4 color, int bno, int wc, int hc)
{
	g_pDevice = Direct3D_GetDevice();
	g_pContext = Direct3D_GetDeviceContext();

	//----------------------------------------------------
	// 頂点バッファをロック
	//----------------------------------------------------
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得 memcpyとかで後から詰めるやり方もある
	Vertex* v = (Vertex*)msr.pData;

	// スプライト1つ当たりの比率
	float w = 1.0f / (float)wc;
	float h = 1.0f / (float)hc;

	// テクスチャの左上座標を計算
	DirectX::XMFLOAT2 uv;
	uv.x = (bno % wc) * w;
	uv.y = (bno / wc) * h;

	//----------------------------------------------------
	// スプライトの頂点設定
	//----------------------------------------------------
	XMFLOAT2 halfSize = { size.x / 2.0f,size.y / 2.0f };
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
