// processor_manager.cpp
#include "processor_manager.h"

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"

#include "camera_component_processor.h"
#include "renderer_3dcube_processor.h"
#include "renderer_3dmodel_processor.h"
#include "physics_processor.h"
#include "collision_processor.h"
#include "dynamics_processor.h"
#include "renderer_font_processor.h"
#include "renderer_image_processor.h"

static CameraComponentProcessor* g_CameraComponentProcessor = nullptr;
static Renderer3DCubeProcessor* g_Renderer3DCubeProcessor = nullptr;
static Renderer3DModelProcessor* g_Renderer3DModelProcessor = nullptr;

static PhysicsProcessor* g_PhysicsProcessor = nullptr;
static CollisionProcessor* g_CollisionProcessor = nullptr;
static DynamicsProcessor* g_DynamicsProcessor = nullptr;

static RendererFontProcessor* g_RendererFontProcessor = nullptr;
static RendererImageProcessor* g_RendererImageProcessor = nullptr;

void ProcessorM_Initialize()
{
    // Processorインスタンス化
    g_CameraComponentProcessor = new CameraComponentProcessor();
    g_Renderer3DCubeProcessor = new Renderer3DCubeProcessor();
    g_Renderer3DModelProcessor = new Renderer3DModelProcessor();
    g_PhysicsProcessor = new PhysicsProcessor();
    g_CollisionProcessor = new CollisionProcessor();
    g_DynamicsProcessor = new DynamicsProcessor();
    g_RendererFontProcessor = new RendererFontProcessor();
    g_RendererImageProcessor = new RendererImageProcessor();

    // Processor初期化
    g_CameraComponentProcessor->Initialize();
    g_Renderer3DCubeProcessor->Initialize();
    g_Renderer3DModelProcessor->Initialize();
    {   // 物理演算系プロセッサー初期化
        g_PhysicsProcessor->Initialize();
        g_CollisionProcessor->Initialize();
        g_DynamicsProcessor->Initialize();
    }
    {   // 2D描画系プロセッサー初期化
        g_RendererFontProcessor->Initialize();
        g_RendererImageProcessor->Initialize();
    }
}

void ProcessorM_Finalize()
{
    // 終了処理
    g_CameraComponentProcessor->Finalize();
    g_Renderer3DCubeProcessor->Finalize();
    g_Renderer3DModelProcessor->Finalize();
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
    delete g_CameraComponentProcessor;
    g_CameraComponentProcessor = nullptr;
    delete g_Renderer3DCubeProcessor;
    g_Renderer3DCubeProcessor = nullptr;
    delete g_Renderer3DModelProcessor;
    g_Renderer3DModelProcessor = nullptr;
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

void ProcessorM_Update()
{
    // 物理演算制御プロセッサー処理
    g_PhysicsProcessor->Process();
    g_CollisionProcessor->Process();
    g_DynamicsProcessor->Process();
}

void ProcessorM_Draw()
{
    g_CameraComponentProcessor->Process();
    
    for (int i = 0; i < g_CameraComponentProcessor->GetSize(); i++)
    {
		// バッファのクリアとシーン描画用RTVのセット
        Direct3D_BeginScene();

		// 各カメラのビュー・投影変換行列をパイプラインに紐づける
		g_CameraComponentProcessor->BindMatrix(i);

        // 各描画プロセッサーの実行
        g_Renderer3DCubeProcessor->Process();
        g_Renderer3DModelProcessor->Process();

		// スナップショット
		g_CameraComponentProcessor->SnapShotSceneSRV(i);
    }

    Direct3D_Clear();
    
    for (int i = 0; i < g_CameraComponentProcessor->GetSize(); i++)
		g_CameraComponentProcessor->DrawFSQuad(i);

    // ビューポートのリセット
    Direct3D_ResetViewport();

    g_RendererImageProcessor->Process();
    g_RendererFontProcessor->Process();

    Direct3D_Present();
}

CameraComponentProcessor* GetCameraComponentProcessor()
{
    return g_CameraComponentProcessor;
}
Renderer3DCubeProcessor* GetRenderer3DCubeProcessor()
{
    return g_Renderer3DCubeProcessor;
}
Renderer3DModelProcessor* GetRenderer3DModelProcessor()
{
    return g_Renderer3DModelProcessor;
}
// 物理演算系プロセッサー取得
PhysicsProcessor* GetPhysicsProcessor()
{
    return g_PhysicsProcessor;
}
CollisionProcessor* GetCollisionProcessor()
{
    return g_CollisionProcessor;
}
DynamicsProcessor* GetDynamicsProcessor()
{
    return g_DynamicsProcessor;
}
// 2D描画系プロセッサー取得
RendererFontProcessor* GetRendererFontProcessor()
{
    return g_RendererFontProcessor;
}
RendererImageProcessor* GetRendererImageProcessor()
{
    return g_RendererImageProcessor;
}
