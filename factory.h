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
    void CreateTpsCamera       (GameObject*, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 atPosition);

    void CreatePlayer       (GameObject*, DirectX::XMFLOAT3 position);
    void CreatePlayer_Chara (GameObject*);

    void CreateBox          (GameObject* cube, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT4 color);
   
    void CreateUiText       (GameObject*, DirectX::XMFLOAT3 position, const char8_t* text, float fontSize, DirectX::XMFLOAT4 color, bool isCenter = false);
}

#endif