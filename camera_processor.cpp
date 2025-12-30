//===================================================
// camera_processor.cpp
// 
// Author：Miu Kitamura
// Date  ：2025/12/29
//===================================================
#include "camera_processor.h"
#include "scene_interface.h"
#include "game_object.h"

#include "transform_component.h"
#include "camera_component.h"

#include "shader.h"
#include "sprite.h"

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pDeviceContext = nullptr;

void CameraProcessor::Initialize()
{
    g_pDevice = Direct3D_GetDevice();
    g_pDeviceContext = Direct3D_GetDeviceContext();

    for(int i = 0; i < 8; ++i) {
        m_cameraObjects[i] = nullptr;
    }
}

void CameraProcessor::Finalize()
{

}

void CameraProcessor::Process(IScene* pScene)
{
    int idCounter = 0;
    for (int i = 0; i < 8; ++i) {
        m_cameraObjects[i] = nullptr;
    }

    // コンポーネントプール取得
    auto* transformCompPool = pScene->GetComponentPool<TransformComponent>();
    auto* cameraCompPool = pScene->GetComponentPool<CameraComponent>();

    if(!transformCompPool || !cameraCompPool)return;

    auto& cameraCompList = cameraCompPool->GetList();
    for (CameraComponent& camera : cameraCompList) {
        TransformComponent* transform = camera.GetOwner()->GetComponent<TransformComponent>();
        
        // 無効なコンポーネントはスキップ
        if (!transform) continue;
        if (!transform->GetEnable() || !camera.GetEnable()) continue;

        // gameObjectIDsに登録
        if (idCounter < 8) {
            m_cameraObjects[idCounter] = &camera;
            idCounter++;
        } else {
            break; // 最大8つまで
        }

        // ビュー行列、プロジェクション行列計算
        XMVECTOR vPos = XMVectorSet(
            transform->GetPosition().x,
            transform->GetPosition().y,
            transform->GetPosition().z,
            0.0f);
        XMVECTOR vAt = XMVectorSet(
            camera.GetAtPosition().x,
            camera.GetAtPosition().y,
            camera.GetAtPosition().z,
            0.0f);
        if (camera.GetUseRotationFromOwner()) {
            // オーナーの回転を使う場合、注視点を更新
            XMFLOAT3 forward = { 0.0f, 0.0f, 1.0f };
            XMVECTOR vForward = XMVectorSet(forward.x, forward.y, forward.z, 0.0f);
            XMVECTOR rotationQuat = transform->GetRotationQuaternion();
            XMVECTOR vRotatedForward = XMVector3Rotate(vForward, rotationQuat);
            vAt = XMVectorAdd(vPos, vRotatedForward);
        }
        XMVECTOR vUp = XMVectorSet(
            camera.GetUpVector().x,
            camera.GetUpVector().y,
            camera.GetUpVector().z,
            0.0f);

        XMMATRIX view = XMMatrixLookAtLH(vPos, vAt, vUp);
        XMMATRIX projection = XMMatrixPerspectiveFovLH(
            XMConvertToRadians(camera.GetFov()),
            camera.GetAspect(),
            camera.GetNearClip(),
            camera.GetFarClip());

        camera.SetViewMatrix(view);
        camera.SetProjectionMatrix(projection);
    }

    // 優先順などの処理はここに追加可能
    // m_cameraObjectIDsをバブルソートするイメージ

}

bool CameraProcessor::BindMatrix(int index)
{
    // コンポーネント取得
    CameraComponent* camera = m_cameraObjects[index];
    if (!camera) return false;

    // Direct3Dにセット
    Direct3D_SetProjectionMatrix(camera->GetProjectionMatrix());
    Direct3D_SetViewMatrix(camera->GetViewMatrix());

    return true;
}

bool CameraProcessor::SnapShotCamera(int index)
{
    // コンポーネント取得
    CameraComponent* camera = m_cameraObjects[index];
    if (!camera) return false;

    // スナップショット用テクスチャSRV作成
    Direct3D_CreateSnapshotSceneSRV(&m_cameraSnapshots[index]);
    camera->SetSnapshot(m_cameraSnapshots[index]);

    return true;
}

bool CameraProcessor::DrawSnapshot(int index, float x, float y, float width, float height)
{
    // コンポーネント取得
    CameraComponent* camera = m_cameraObjects[index];
    if (!camera) return false;
    if (!camera->GetSnapshot()) return false;

    // スナップショット描画
    ID3D11ShaderResourceView* srv = camera->GetSnapshot();

    SetBlendState(BLENDSTATE_NONE);
    g_pDeviceContext->PSSetShaderResources(0, 1, &srv);
    DrawSprite({x, y},{width, height},{1.0f,1.0f,1.0f,1.0f});

    return true;
}
