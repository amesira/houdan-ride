//===================================================
// renderer_image_processor.h
// 
// Author：Miu Kitamura
// Date  ：2025/11/18
//===================================================
#include "renderer_image_processor.h"

#include "direct3d.h"
#include "sprite.h"
#include "shader.h"

#include "scene_interface.h"
#include "game_object.h"
#include "rect_transform_component.h"
#include "image_component.h"

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

void RendererImageProcessor::Initialize()
{
    g_pDevice = Direct3D_GetDevice();
    g_pContext = Direct3D_GetDeviceContext();
}

void RendererImageProcessor::Finalize()
{

}

void RendererImageProcessor::Process(IScene* pScene)
{
    Shader_Begin();

    const float screenWidth = (float)Direct3D_GetBackBufferWidth();
    const float screenHeight = (float)Direct3D_GetBackBufferHeight();

    // 頂点シェーダーに変換行列を設定
    Shader_SetMatrix(XMMatrixOrthographicOffCenterLH(
        0.0f,
        screenWidth,
        screenHeight,
        0.0f,
        0.0f,
        1.0f));

    auto* imagePool = pScene->GetComponentPool<ImageComponent>();
    auto* rectTransformPool = pScene->GetComponentPool<RectTransformComponent>();

    auto& imageList = imagePool->GetList();

    for(ImageComponent& image : imageList) {
        RectTransformComponent* pRect = rectTransformPool->GetByGameObjectID(image.GetOwner()->GetID());
        ImageComponent* pImage = &image;

        // ★ 描画方法は仮。回転未対応。
        // 描画位置・サイズ取得
        DirectX::XMFLOAT2 pos = {pRect->GetPosition().x,pRect->GetPosition().y};
        DirectX::XMFLOAT2 size = { pRect->GetScaling().x * 100.0f,pRect->GetScaling().y * 100.0f};
        DirectX::XMFLOAT4 color = pImage->GetColor();
        ID3D11ShaderResourceView* texture = pImage->GetTexture();

        // テクスチャ設定
        g_pContext->PSSetShaderResources(0, 1, &texture);

        // スプライト描画
        DrawSprite(pos, size, color);
    }
}
