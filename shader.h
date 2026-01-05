/*==============================================================================

   シェーダー [shader.h]
														 Author : Youhei Sato
														 Date   : 2025/05/15
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef SHADER_H
#define	SHADER_H

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

/// @brief 頂点構造体
struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 color;		// 頂点カラー（R,G,B,A）
	DirectX::XMFLOAT2 texCoord;	// テクスチャ座標
};

enum ShaderBeginMode {
    Default = 0,
	TrueTypeFont,
	FullScreenQuad,

	MAX,
};

#define MAX_LIGHT (4)

bool Shader_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Shader_Finalize();

void Shader_SetMatrix(const XMMATRIX& matrix);
void Shader_SetWorldMatrix(const XMMATRIX& world);

void Shader_SetLight(int index, 
	const XMFLOAT4& dir, const XMFLOAT4& diff, const XMFLOAT4& ambi);
void Shader_SetLightEnable(int index, bool enable);

void Shader_SetPixelOption(float grayRate);

void Shader_Begin(ShaderBeginMode mode = ShaderBeginMode::Default);

#endif // SHADER_H
