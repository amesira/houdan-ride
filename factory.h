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
using namespace DirectX;

#include "game_object.h"

namespace Factory {
    void CreateTpsCamera    (GameObject*, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 atPosition);
    void CreateLight        (GameObject*, DirectX::XMFLOAT4 direction, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 ambient);

    void CreatePlayer       (GameObject*, DirectX::XMFLOAT3 position);

    void CreateBall         (GameObject*, DirectX::XMFLOAT3 position);

    void CreateWoodbox      (GameObject*, DirectX::XMFLOAT3 position);

    void CreateEnemy        (GameObject*, DirectX::XMFLOAT3 position);

    void CreateBox          (GameObject* cube, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT4 color);
    void CreateTrain        (GameObject*, DirectX::XMFLOAT3 position);

    // component
    void CreateBoxCollider  (GameObject*, XMFLOAT3 position, XMFLOAT3 rotation, DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 size);

    // ui
    void CreateUiText       (GameObject*, XMFLOAT3 position, const char8_t* text, float fontSize, DirectX::XMFLOAT4 color, bool isCenter = false);
    void CreateUiImageWorld (GameObject*, XMFLOAT3 position, XMFLOAT3 euler, XMFLOAT3 scale, const wchar_t* texturePath = L"asset\\Texture\\test.jpg");
    void CreateUiSlider     (GameObject*, XMFLOAT3 position, float rotation, XMFLOAT2 size);
}

#endif