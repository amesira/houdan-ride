//===================================================
// renderer_3dcube_processor.cpp
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//===================================================
#include "renderer_3dcube_processor.h"

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "Sources/System/direct3d.h"
using namespace DirectX;

// graphics, devices
#include "Sources/System/Graphic/shader.h"
#include "Sources/System/Graphic/sprite.h"
#include "Sources/System/Device/keyboard.h"

#include "Sources/Scene/scene_interface.h"
#include "Sources/Core/game_object.h"

// component
#include "Sources/GameParts/Component/transform_component.h"
#include "Sources/GameParts/Component/cubemesh_component.h"

static ID3D11ShaderResourceView* g_DefTexture = NULL;

void Renderer3DCubeProcessor::Initialize()
{
    LoadTexture(&g_DefTexture, L"asset\\Texture\\sample.png");
    //Polygon3D_Initialize(Direct3D_GetDevice(), Direct3D_GetDeviceContext());
}

void Renderer3DCubeProcessor::Finalize()
{

}

void Renderer3DCubeProcessor::Process(IScene* pScene)
{
    auto* cubemeshPool = pScene->GetComponentPool<CubemeshComponent>();
    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    if (!cubemeshPool || !transformPool)return;

    auto& cubemeshList = cubemeshPool->GetList();

    for(CubemeshComponent& c : cubemeshList) {
        CubemeshComponent* cubemesh = &c;
        TransformComponent* transform = transformPool->GetByGameObjectID(c.GetOwner()->GetID());

        // コンポーネントが無効ならスキップ
        if (!transform)continue;
        if (!transform->GetEnable() || !cubemesh->GetEnable())continue;

        // 3Dキューブ描画
        ID3D11ShaderResourceView* tex = cubemesh->GetTexture();
        if (!tex)tex = g_DefTexture;

       /* Polygon3D_CubeRender(
            transform->GetPosition(),
            transform->GetRotation(),
            transform->GetScaling(),
            tex,
            cubemesh->GetColor()
        );*/
    }
}
