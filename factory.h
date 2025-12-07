//----------------------------------------------------
// factory.h [ゲームオブジェクト工場]
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//----------------------------------------------------
#ifndef FACTORY_H
#define FACTORY_H

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"

#include "game_object.h"

namespace Factory {
    GameObject* CreateTestPlayer(DirectX::XMFLOAT3 position);
    GameObject* CreateBox(DirectX::XMFLOAT3 position,DirectX::XMFLOAT4 color,DirectX::XMFLOAT3 scaling={1.0f,1.0f,1.0f});
    GameObject* CreateEfreet(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scaling = { 1.0f,1.0f,1.0f });
    GameObject* CreateMap(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scaling = { 1.0f,1.0f,1.0f });
    GameObject* CreateCamera(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 lookAt);
    GameObject* CreateUiText(DirectX::XMFLOAT3 position, const char8_t* text, float fontSize, DirectX::XMFLOAT4 color, bool isCenter = false);
}

#endif