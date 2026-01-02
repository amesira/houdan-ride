//----------------------------------------------------
// polygon3d.h
// 
// 
//----------------------------------------------------
#ifndef POLYGON_3D_H
#define POLYGON_3D_H

// directX
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
using namespace DirectX;

void    Polygon3D_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void    Polygon3D_Finalize();
void    Polygon3D_Update();

void    Polygon3D_CubeRender(
    DirectX::XMFLOAT3 position, 
    DirectX::XMVECTOR rotation, 
    DirectX::XMFLOAT3 scaling,
    ID3D11ShaderResourceView* tex,
    DirectX::XMFLOAT4 color);

#endif