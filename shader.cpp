/*==============================================================================

   シェーダー [shader.cpp]
														 Author : Youhei Sato
														 Date   : 2025/05/15
--------------------------------------------------------------------------------

==============================================================================*/
#include "shader.h"

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include "direct3d.h"
#include "debug_ostream.h"
#include <fstream>

// 頂点シェーダー
static ID3D11VertexShader* g_pVertexShader = nullptr;	// 頂点シェーダー
static ID3D11InputLayout* g_pInputLayout = nullptr;		// 頂点レイアウト

static ID3D11Buffer* g_pVSConstantBuffer = nullptr;		// 定数バッファ1個
static ID3D11Buffer* g_pWorldConstantBuffer = nullptr;	// ワールド行列用定数バッファ

// ピクセルシェーダー
static ID3D11PixelShader* g_pPixelShader = nullptr;	// ピクセルシェーダー

static ID3D11Buffer* g_pOptionCB = nullptr;			// オプション用定数バッファ
struct OptionBuffer {
    float grayRate;  // グレースケール率
	float padding[3]; // 16バイトアライメント用パディング
};

// カスタムシェーダー
static ID3D11PixelShader* g_pFontShader = nullptr; // フォント用ピクセルシェーダー

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

// プロトタイプ宣言
struct VsBinaryData {
    unsigned char*	vsBinaryPointer;
	std::streamsize	fileSize;
};
bool	LoadVertexShader(const char* filename, ID3D11VertexShader** ppVertexShader, VsBinaryData* pData);
bool    LoadPixelShader(const char* filename, ID3D11PixelShader** ppPixelShader);

bool Shader_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	HRESULT hr; // 戻り値格納用

	// デバイスとデバイスコンテキストのチェック
	if (!pDevice || !pContext) {
		hal::dout << "Shader_Initialize() : Device or DeviceContext is invalid" << std::endl;
		return false;
	}

	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	VsBinaryData vsData{};

	if (!LoadVertexShader("shader_vertex.cso", &g_pVertexShader, &vsData)) {
		hal::dout << "Shader_Initialize() : 頂点シェーダーの作成に失敗しました" << std::endl;
		return false;
    }

	// 頂点レイアウトの定義
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT num_elements = ARRAYSIZE(layout); // 配列の要素数を取得

	// 頂点レイアウトの作成
	hr = g_pDevice->CreateInputLayout(layout, num_elements, vsData.vsBinaryPointer, vsData.fileSize, &g_pInputLayout);

    delete[] vsData.vsBinaryPointer; // メモリリークしないようにバイナリデータのバッファを解放

	if (FAILED(hr)) {
		hal::dout << "Shader_Initialize() : 頂点レイアウトの作成に失敗しました" << std::endl;
		return false;
	}

	// 頂点シェーダー用定数バッファの作成
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(XMFLOAT4X4); // バッファのサイズ
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // バインドフラグ

	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pVSConstantBuffer);

    // 事前コンパイル済みピクセルシェーダーの読み込み
	if (!LoadPixelShader("shader_pixel.cso", &g_pPixelShader)) {
		hal::dout << "Shader_Initialize() : ピクセルシェーダーの作成に失敗しました" << std::endl;
		return false;
    }

    // ピクセルシェーダー用定数バッファの作成
    // オプション用定数バッファの作成
	{
		buffer_desc.ByteWidth = sizeof(OptionBuffer); // バッファのサイズ
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // バインドフラグ

		g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pOptionCB);
	}

	if (!LoadPixelShader("shader_pixel_font.cso", &g_pFontShader)) {
		hal::dout << "Shader_Initialize() : フォント用ピクセルシェーダーの作成に失敗しました" << std::endl;
		return false;
    }

	return true;
}

void Shader_Finalize()
{
	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pVSConstantBuffer);
	SAFE_RELEASE(g_pInputLayout);
	SAFE_RELEASE(g_pVertexShader);

    SAFE_RELEASE(g_pFontShader);
}

void Shader_SetMatrix(const DirectX::XMMATRIX& matrix)
{
	// 定数バッファ格納用行列の構造体を定義
	XMFLOAT4X4 transpose;

	// 行列を転置して定数バッファ格納用行列に変換
	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

	// 定数バッファに行列をセット
	g_pContext->UpdateSubresource(g_pVSConstantBuffer, 0, nullptr, &transpose, 0, 0);
}

void Shader_SetPixelOption(float grayRate)
{
	OptionBuffer optionBuffer;
    optionBuffer.grayRate = grayRate;

    g_pContext->UpdateSubresource(g_pOptionCB, 0, nullptr, &optionBuffer, 0, 0);
}

void Shader_Begin(ShaderBeginMode mode)
{
	switch (mode)
	{
    case ShaderBeginMode::Default:
    {
		g_pContext->VSSetShader(g_pVertexShader, nullptr, 0);
		g_pContext->PSSetShader(g_pPixelShader, nullptr, 0);
		g_pContext->IASetInputLayout(g_pInputLayout);

		g_pContext->VSSetConstantBuffers(0, 1, &g_pVSConstantBuffer);
        g_pContext->PSSetConstantBuffers(0, 1, &g_pOptionCB);
		break;
    }
      
	case ShaderBeginMode::TrueTypeFont:
	{
		g_pContext->VSSetShader(g_pVertexShader, nullptr, 0);
		g_pContext->PSSetShader(g_pFontShader, nullptr, 0);
		g_pContext->IASetInputLayout(g_pInputLayout);

		g_pContext->VSSetConstantBuffers(0, 1, &g_pVSConstantBuffer);
		break;
	}
	default: break;
	}
}

// 頂点シェーダー読み込み
bool LoadVertexShader(const char* filename, ID3D11VertexShader** ppVertexShader, VsBinaryData* pData)
{
	std::ifstream ifs_vs(filename, std::ios::binary);
	if (!ifs_vs)return false;

	// ファイルサイズを取得
	ifs_vs.seekg(0, std::ios::end); // ファイルポインタを末尾に移動
	pData->fileSize = ifs_vs.tellg(); // ファイルポインタの位置を取得（つまりファイルサイズ）
	ifs_vs.seekg(0, std::ios::beg); // ファイルポインタを先頭に戻す

	// バイナリデータを格納するためのバッファを確保
	pData->vsBinaryPointer = new unsigned char[pData->fileSize];

	ifs_vs.read((char*)pData->vsBinaryPointer, pData->fileSize); // バイナリデータを読み込む
	ifs_vs.close(); // ファイルを閉じる

	// 頂点シェーダーの作成
	HRESULT hr = g_pDevice->CreateVertexShader(pData->vsBinaryPointer, pData->fileSize, nullptr, ppVertexShader);
	if (FAILED(hr)) {
		delete[] pData->vsBinaryPointer; // メモリリークしないようにバイナリデータのバッファを解放
		return false;
	}

	return true;
}

// ピクセルシェーダー読み込み
bool LoadPixelShader(const char* filename, ID3D11PixelShader** ppPixelShader)
{
	// 事前コンパイル済みピクセルシェーダーの読み込み
	std::ifstream ifs_ps(filename, std::ios::binary);
	if (!ifs_ps)return false;

    // ファイルサイズを取得
	ifs_ps.seekg(0, std::ios::end);
	std::streamsize filesize = ifs_ps.tellg();
	ifs_ps.seekg(0, std::ios::beg);

    // バイナリデータを格納するためのバッファを確保
	unsigned char* psbinary_pointer = new unsigned char[filesize];
	ifs_ps.read((char*)psbinary_pointer, filesize);
	ifs_ps.close();

	// ピクセルシェーダーの作成
	HRESULT hr = g_pDevice->CreatePixelShader(psbinary_pointer, filesize, nullptr, ppPixelShader);

	delete[] psbinary_pointer; // バイナリデータのバッファを解放

	if (FAILED(hr))return false;

	return true;
}
