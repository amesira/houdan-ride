// processor_manager.cpp
#include "processor_manager.h"

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"

#include "scene_interface.h"

#include "renderer_3dcube_processor.h"
#include "physics_processor.h"
#include "collision_processor.h"
#include "dynamics_processor.h"
#include "renderer_font_processor.h"
#include "renderer_image_processor.h"

static Renderer3DCubeProcessor* g_Renderer3DCubeProcessor = nullptr;

static PhysicsProcessor* g_PhysicsProcessor = nullptr;
static CollisionProcessor* g_CollisionProcessor = nullptr;
static DynamicsProcessor* g_DynamicsProcessor = nullptr;

static RendererFontProcessor* g_RendererFontProcessor = nullptr;
static RendererImageProcessor* g_RendererImageProcessor = nullptr;

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
    // Processorインスタンス化
    g_Renderer3DCubeProcessor = new Renderer3DCubeProcessor();
    g_PhysicsProcessor = new PhysicsProcessor();
    g_CollisionProcessor = new CollisionProcessor();
    g_DynamicsProcessor = new DynamicsProcessor();
    g_RendererFontProcessor = new RendererFontProcessor();
    g_RendererImageProcessor = new RendererImageProcessor();

    // Processor初期化
    g_Renderer3DCubeProcessor->Initialize();
    {   // 物理演算系プロセッサー初期化
        g_PhysicsProcessor->Initialize();
        g_CollisionProcessor->Initialize();
        g_DynamicsProcessor->Initialize();
    }
    {   // 2D描画系プロセッサー初期化
        g_RendererFontProcessor->Initialize();
        g_RendererImageProcessor->Initialize();
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
    // 終了処理
    g_Renderer3DCubeProcessor->Finalize();
    {
        g_PhysicsProcessor->Finalize();
        g_CollisionProcessor->Finalize();
        g_DynamicsProcessor->Finalize();
    }
    {
        g_RendererFontProcessor->Finalize();
        g_RendererImageProcessor->Finalize();
    }
    
    // delete
    delete g_Renderer3DCubeProcessor;
    g_Renderer3DCubeProcessor = nullptr;
    {
        delete g_PhysicsProcessor;
        g_PhysicsProcessor = nullptr;
        delete g_CollisionProcessor;
        g_CollisionProcessor = nullptr;
        delete g_DynamicsProcessor;
        g_DynamicsProcessor = nullptr;
    }
    {
        delete g_RendererFontProcessor;
        g_RendererFontProcessor = nullptr;
        delete g_RendererImageProcessor;
        g_RendererImageProcessor = nullptr;
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
    
  //  for (int i = 0; i < g_CameraComponentProcessor->GetSize(); i++)
  //  {
		//// バッファのクリアとシーン描画用RTVのセット
  //      Direct3D_BeginScene();

		//// 各カメラのビュー・投影変換行列をパイプラインに紐づける
		//g_CameraComponentProcessor->BindMatrix(i);

  //      // 各描画プロセッサーの実行
  //      g_Renderer3DCubeProcessor->Process();

		//// スナップショット
		//g_CameraComponentProcessor->SnapShotSceneSRV(i);
  //  }

    Direct3D_Clear();

    g_Renderer3DCubeProcessor->Process(pScene);
    
    /*for (int i = 0; i < g_CameraComponentProcessor->GetSize(); i++)
		g_CameraComponentProcessor->DrawFSQuad(i);*/

    // ビューポートのリセット
    //Direct3D_ResetViewport();

    g_RendererImageProcessor->Process(pScene);
    g_RendererFontProcessor->Process(pScene);

    Direct3D_Present();
}
