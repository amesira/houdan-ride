//===================================================
// camera_component_processor.cpp
// 
// Author：Hayato Ushimaru
// Date  ：2025/11/18
//===================================================
#include "camera_component_processor.h"

// directX
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
using namespace DirectX;

// component
#include "transform_component.h"
#include "camera_component.h"
#include <algorithm>
#include "shader.h"

void CameraComponentProcessor::Initialize()
{

}

void CameraComponentProcessor::Finalize()
{

}

void CameraComponentProcessor::Process()
{
    // depthでソートする
    std::sort(m_components.begin(), m_components.end(),
        [](const Components& a, const Components& b) {
            return a.m_camera->GetDepth() < b.m_camera->GetDepth();
		});

    for (Components& cmps : m_components) {
        
        // ビュー変換行列の更新
        {
            XMVECTOR rotationQuaterion = cmps.m_transform->GetRotationQuaternion();

            // transformの値を元にカメラのビュー行列を設定
            XMFLOAT3 position = cmps.m_transform->GetPosition();
            XMVECTOR eye = XMLoadFloat3(&position);

            // カメラの正面ベクトルを計算
            XMVECTOR forward = XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationQuaterion);

            // 焦点位置を計算
            XMVECTOR focus = XMVectorAdd(eye, forward);

            // カメラの上方向ベクトルを計算
            XMVECTOR up = XMVector3Rotate(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationQuaterion);

            //最終的な視点・焦点をもとにビュー変換行列を作成
            cmps.m_camera->SetViewMatrix(XMMatrixLookAtLH(eye, focus, up));
        }

        // 投影変換行列の更新
        {
            cmps.m_camera->SetProjectionMatrix(XMMatrixPerspectiveFovLH
            (
                XMConvertToRadians(cmps.m_camera->GetFieldOfView()),
                cmps.m_camera->GetAspectRatio(),
                cmps.m_camera->GetNearClip(),
                cmps.m_camera->GetFarClip()
            ));
        }
    }
}

void CameraComponentProcessor::BindMatrix(int componentIndex)
{
    if(componentIndex < 0 || static_cast<int>(m_components.size()) <= componentIndex)
    {
        Direct3D_SetViewMatrix(XMMatrixIdentity());
        // 投影変換行列をパイプラインに設定
        Direct3D_SetProjectionMatrix(XMMatrixIdentity());
	}

    CameraComponent* camera = m_components[componentIndex].m_camera;
    // ビュー行列をパイプラインに設定
    Direct3D_SetViewMatrix(camera->GetViewMatrix());
    // 投影変換行列をパイプラインに設定
	Direct3D_SetProjectionMatrix(camera->GetProjectionMatrix());
}

void CameraComponentProcessor::SnapShotSceneSRV(int componentIndex)
{
    if (componentIndex < 0 || static_cast<int>(m_components.size()) <= componentIndex)
        return;

    CameraComponent* camera = m_components[componentIndex].m_camera;
    ComPtr<ID3D11ShaderResourceView> srv;
	Direct3D_CreateSnapshotSceneSRV(srv.GetAddressOf());

	camera->SetShaderResourceView(srv);
}

ID3D11ShaderResourceView* CameraComponentProcessor::GetCameraSRV(int componentIndex)
{
    if (0 <= componentIndex && componentIndex < static_cast<int>(m_components.size()))
    {
        CameraComponent* camera = m_components[componentIndex].m_camera;
        return camera->GetShaderResourceView();
	}
    return nullptr;
}

void CameraComponentProcessor::DrawFSQuad(int componentIndex)
{
	auto pContext = Direct3D_GetDeviceContext();

	// ビューポートの設定
	float backbufferWidth = static_cast<float>(Direct3D_GetBackBufferWidth());
	float backbufferHeight = static_cast<float>(Direct3D_GetBackBufferHeight());

	CameraComponent::ViewportRect viewportRect = m_components[componentIndex].m_camera->GetViewportRect();
    D3D11_VIEWPORT dxViewport;
	dxViewport.TopLeftX = viewportRect.x * backbufferWidth;
	dxViewport.TopLeftY = (1.0f - (viewportRect.y + viewportRect.height)) * backbufferHeight;
	dxViewport.Width = viewportRect.width * backbufferWidth;
	dxViewport.Height = viewportRect.height * backbufferHeight;
    dxViewport.MinDepth = 0.0f;
	dxViewport.MaxDepth = 1.0f;

    pContext->RSSetViewports(1, &dxViewport);

    ID3D11ShaderResourceView* pSrv = GetCameraSRV(componentIndex);
	pContext->PSSetShaderResources(0, 1, &pSrv);

    Shader_Begin(ShaderBeginMode::FullScreenQuad);

    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// フルスクリーンクアッドの描画
    pContext->Draw(4, 0);
}
