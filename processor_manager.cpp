// processor_manager.cpp
#include "processor_manager.h"

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"

#include "scene_interface.h"

#include "debug_renderer.h"

#include "renderer_3dcube_processor.h"
#include "renderer_3dmodel_processor.h"
#include "physics_processor.h"
#include "collision_processor.h"
#include "dynamics_processor.h"
#include "renderer_font_processor.h"
#include "renderer_image_processor.h"
#include "camera_processor.h"

static Renderer3DCubeProcessor* g_Renderer3DCubeProcessor = nullptr;
static Renderer3DModelProcessor* g_Renderer3DModelProcessor = nullptr;

static PhysicsProcessor* g_PhysicsProcessor = nullptr;
static CollisionProcessor* g_CollisionProcessor = nullptr;
static DynamicsProcessor* g_DynamicsProcessor = nullptr;

static RendererFontProcessor* g_RendererFontProcessor = nullptr;
static RendererImageProcessor* g_RendererImageProcessor = nullptr;

static CameraProcessor* g_CameraProcessor = nullptr;

class CAMERA {
public:
    DirectX::XMFLOAT3    Position;   // 座標
    DirectX::XMFLOAT3    AtPosition; // 注視点
    DirectX::XMFLOAT3    UpVector;   // 上方ベクトル

    float       Fov;        // 視野角（画角）
    float       Aspect;     // 画面のアスペクト比
    float       NearClip;   // 近面クリップ距離
    float       FarClip;    // 遠面クリップ距離

    DirectX::XMMATRIX    View;       //ビュー行列
    DirectX::XMMATRIX    Projection; // プロジェクション行列
};
static CAMERA cameraObj = {
    { 0.0f, 0.0f, -5.0f },    // Position
    { 0.0f, 0.0f, 0.0f },     // AtPosition
    { 0.0f, 1.0f, 0.0f },     // UpVector
    60.0f,                     // Fov
    16.0f / 9.0f,              // Aspect
    0.1f,                      // NearClip
    100.0f,                    // FarClip
};

void ProcessorM_Initialize()
{
    DebugRenderer_Initialize();

    // Processorインスタンス化
    g_Renderer3DCubeProcessor = new Renderer3DCubeProcessor();
    g_Renderer3DModelProcessor = new Renderer3DModelProcessor();

    g_PhysicsProcessor = new PhysicsProcessor();
    g_CollisionProcessor = new CollisionProcessor();
    g_DynamicsProcessor = new DynamicsProcessor();

    g_RendererFontProcessor = new RendererFontProcessor();
    g_RendererImageProcessor = new RendererImageProcessor();

    g_CameraProcessor = new CameraProcessor();

    // Processor初期化
    {   // 3D描画系プロセッサー初期化
        g_Renderer3DCubeProcessor->Initialize();
        g_Renderer3DModelProcessor->Initialize();
    }
    {   // 物理演算系プロセッサー初期化
        g_PhysicsProcessor->Initialize();
        g_CollisionProcessor->Initialize();
        g_DynamicsProcessor->Initialize();
    }
    {   // 2D描画系プロセッサー初期化
        g_RendererFontProcessor->Initialize();
        g_RendererImageProcessor->Initialize();
    }
    {
        g_CameraProcessor->Initialize();
    }

    // カメラ設定

    // プロジェクション行列作成
    cameraObj.Projection = DirectX::XMMatrixPerspectiveFovLH(
        DirectX::XMConvertToRadians(cameraObj.Fov),
        cameraObj.Aspect,
        cameraObj.NearClip,
        cameraObj.FarClip
    );

    // ビュー行列作成
    DirectX::XMVECTOR    vPos = DirectX::XMVectorSet(
        cameraObj.Position.x,
        cameraObj.Position.y,
        cameraObj.Position.z,
        0.0f);
    DirectX::XMVECTOR    vAt = DirectX::XMVectorSet(
        cameraObj.AtPosition.x,
        cameraObj.AtPosition.y,
        cameraObj.AtPosition.z,
        0.0f);
    DirectX::XMVECTOR    vUp = DirectX::XMVectorSet(
        cameraObj.UpVector.x,
        cameraObj.UpVector.y,
        cameraObj.UpVector.z,
        0.0f);
    cameraObj.View = DirectX::XMMatrixLookAtLH(vPos, vAt, vUp);
    Direct3D_SetViewMatrix(cameraObj.View);
    Direct3D_SetProjectionMatrix(cameraObj.Projection);
}

void ProcessorM_Finalize()
{
    DebugRenderer_Finalize();
    
    // 終了処理
    {
        g_Renderer3DCubeProcessor->Finalize();
        g_Renderer3DModelProcessor->Finalize();

        delete g_Renderer3DCubeProcessor;
        g_Renderer3DCubeProcessor = nullptr;
        delete g_Renderer3DModelProcessor;
        g_Renderer3DModelProcessor = nullptr;
    }
    {
        g_PhysicsProcessor->Finalize();
        g_CollisionProcessor->Finalize();
        g_DynamicsProcessor->Finalize();

        delete g_PhysicsProcessor;
        g_PhysicsProcessor = nullptr;
        delete g_CollisionProcessor;
        g_CollisionProcessor = nullptr;
        delete g_DynamicsProcessor;
        g_DynamicsProcessor = nullptr;
    }
    {
        g_RendererFontProcessor->Finalize();
        g_RendererImageProcessor->Finalize();

        delete g_RendererFontProcessor;
        g_RendererFontProcessor = nullptr;
        delete g_RendererImageProcessor;
        g_RendererImageProcessor = nullptr;
    }
    {
        g_CameraProcessor->Finalize();

        delete g_CameraProcessor;
        g_CameraProcessor = nullptr;
    }
}

void ProcessorM_Update(IScene* pScene)
{
    // 物理演算制御プロセッサー処理
    g_PhysicsProcessor->Process(pScene);
    g_CollisionProcessor->Process(pScene);
    g_DynamicsProcessor->Process(pScene);
}

void ProcessorM_Draw(IScene* pScene)
{
    // カメラ設定
    g_CameraProcessor->Process(pScene);

    for(int i = 0; i < g_CameraProcessor->GetCameraCount(); i++) {

        // バッファのクリアとシーン描画用RTVのセット
        Direct3D_BeginScene();
        g_CameraProcessor->BindMatrix(i);

        // 各3D描画プロセッサーの実行
        g_Renderer3DModelProcessor->Process(pScene);
        g_Renderer3DCubeProcessor->Process(pScene);

        DebugRenderer_DrawFlush();

        // スナップショット撮影
        g_CameraProcessor->SnapShotCamera(i);
    }
    
    Direct3D_Clear();

    // カメラからのスナップショットをフルスクリーンに描画
    g_CameraProcessor->DrawSnapshot(0, 0.0f, 0.0f, (float)Direct3D_GetBackBufferWidth(), (float)Direct3D_GetBackBufferHeight());

    // 各2D描画プロセッサーの実行
    g_RendererImageProcessor->Process(pScene);
    g_RendererFontProcessor->Process(pScene);

    Direct3D_Present();
}
