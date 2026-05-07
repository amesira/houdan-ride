// processor_manager.cpp
#include "processor_manager.h"

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "Sources/System/direct3d.h"

#include "Sources/System/Graphic/shader.h"

#include "Sources/Scene/scene_interface.h"
#include "Sources/System/mi_fps.h"

#include "Utility/debug_renderer.h"

#include "particle_manager.h"

#include "Sources/GameParts/Processor/renderer_3dcube_processor.h"
#include "Sources/GameParts/Processor/renderer_3dmodel_processor.h"
#include "Sources/GameParts/Processor/PhysicsProcessors/physics_processor.h"
#include "Sources/GameParts/Processor/PhysicsProcessors/collision_processor.h"
#include "Sources/GameParts/Processor/PhysicsProcessors/dynamics_processor.h"
#include "Sources/GameParts/Processor/UiProcessors/renderer_font_processor.h"
#include "Sources/GameParts/Processor/UiProcessors/renderer_image_processor.h"
#include "Sources/GameParts/Processor/camera_processor.h"
#include "Sources/GameParts/Processor/light_processor.h"
#include "Sources/GameParts/Processor/UiProcessors/renderer_slider_processor.h"

#include "Sources/Core/game_object.h"
#include "Sources/GameParts/Component/UiComponents/image_component.h"

#include "Sources/Content/fade.h"

static Renderer3DCubeProcessor* g_Renderer3DCubeProcessor = nullptr;
static Renderer3DModelProcessor* g_Renderer3DModelProcessor = nullptr;

static PhysicsProcessor* g_PhysicsProcessor = nullptr;
static CollisionProcessor* g_CollisionProcessor = nullptr;
static DynamicsProcessor* g_DynamicsProcessor = nullptr;

static RendererFontProcessor* g_RendererFontProcessor = nullptr;
static RendererImageProcessor* g_RendererImageProcessor = nullptr;
static RendererSliderProcessor* g_RendererSliderProcessor = nullptr;

static CameraProcessor* g_CameraProcessor = nullptr;
static LightProcessor* g_LightProcessor = nullptr;

static ImageComponent* g_waterImageComp = nullptr;

void ProcessorM_Initialize()
{
    DebugRenderer_Initialize();
    ParticleM_Initialize();

    // Processorインスタンス化
    g_Renderer3DCubeProcessor = new Renderer3DCubeProcessor();
    g_Renderer3DModelProcessor = new Renderer3DModelProcessor();

    g_PhysicsProcessor = new PhysicsProcessor();
    g_CollisionProcessor = new CollisionProcessor();
    g_DynamicsProcessor = new DynamicsProcessor();

    g_RendererFontProcessor = new RendererFontProcessor();
    g_RendererImageProcessor = new RendererImageProcessor();
    g_RendererSliderProcessor = new RendererSliderProcessor();

    g_CameraProcessor = new CameraProcessor();
    g_LightProcessor = new LightProcessor();

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
        g_RendererSliderProcessor->Initialize();
    }
    {   // カメラ・ライト系プロセッサー初期化
        g_CameraProcessor->Initialize();
        g_LightProcessor->Initialize();
    }
}

void ProcessorM_Finalize()
{
    DebugRenderer_Finalize();
    ParticleM_Finalize();
    
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
        g_RendererSliderProcessor->Finalize();

        delete g_RendererFontProcessor;
        g_RendererFontProcessor = nullptr;
        delete g_RendererImageProcessor;
        g_RendererImageProcessor = nullptr;
        delete g_RendererSliderProcessor;
        g_RendererSliderProcessor = nullptr;
    }
    {
        g_CameraProcessor->Finalize();
        g_LightProcessor->Finalize();

        delete g_CameraProcessor;
        g_CameraProcessor = nullptr;
        delete g_LightProcessor;
        g_LightProcessor = nullptr;
    }

    g_waterImageComp = nullptr;
}

void ProcessorM_Update(IScene* pScene)
{
    // 物理演算制御プロセッサー処理
    g_PhysicsProcessor->Process(pScene);
    g_CollisionProcessor->Process(pScene);
    g_DynamicsProcessor->Process(pScene);

    // パーティクル更新
    ParticleM_Update(FPS_GetDeltaTime());

    if (g_waterImageComp == nullptr) {
        GameObject* waterObj = pScene->GetGameObjectByName("Water");
        if(waterObj) {
            g_waterImageComp = waterObj->GetComponent<ImageComponent>();
        }
    }
}

void ProcessorM_Draw(IScene* pScene)
{
    // カメラ設定
    g_CameraProcessor->Process(pScene);

    // ワールド配置設定
    g_RendererImageProcessor->SetDrawWorldImages(true);
    g_RendererImageProcessor->SetDrawUiImages(false);

    for(int i = 0; i < g_CameraProcessor->GetCameraCount(); i++) {
        if(g_waterImageComp){
            if (i == 0) {
                g_waterImageComp->SetEnable(true);
            }
            else {
                g_waterImageComp->SetEnable(false);
            }
        }
        // バッファのクリアとシーン描画用RTVのセット
        XMFLOAT4 clearColor = g_CameraProcessor->GetClearColor(i);
        Direct3D_BeginScene(clearColor.x, clearColor.y, clearColor.z);
        g_CameraProcessor->BindMatrix(i);

        // ライト設定
        g_LightProcessor->Process(pScene);

        // 各3D描画プロセッサーの実行
        SetDepthState(DEPTHSTATE_ENABLE);
        g_Renderer3DModelProcessor->Process(pScene);
        g_Renderer3DCubeProcessor->Process(pScene);

        // UI用ライト設定
        g_LightProcessor->SetUiLight();

        // パーティクル
        ParticleM_Draw();

        // world配置
        // -ワールド配置のUIは、必ず前面に描画される-
        SetBlendState(BLENDSTATE_ALFA);
        g_RendererImageProcessor->Process(pScene);

        // スナップショット撮影
        g_CameraProcessor->SnapShotCamera(i);
    }
    
    Direct3D_Clear();

    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_DISABLE);

    g_LightProcessor->SetUiLight();

    // カメラからのスナップショットをフルスクリーンに描画
    const float w = (float)Direct3D_GetBackBufferWidth();
    const float h = (float)Direct3D_GetBackBufferHeight();
    g_CameraProcessor->DrawSnapshot(0, w / 2.0f, h / 2.0f, w, h);
    SetBlendState(BLENDSTATE_ALFA);

    // マップUI描画
    if (g_CameraProcessor->GetCameraCount() > 1) {
        SetBlendState(BLENDSTATE_ALFA);
        Shader_SetPixelOption(XMFLOAT4(1.0f, 1.0f, 1.0f, 0.1f), 1.0f);
        XMFLOAT4 alphaColor = g_CameraProcessor->GetClearColor(1);
        alphaColor.w = 0.0f;
        Shader_SetPixelOptionAlphaRate(alphaColor);

        g_CameraProcessor->DrawSnapshot(1, 120.0f, 280.0f, 30.0f * 9.0f, 30.0f * 16.0f, false);

        Shader_SetPixelOption(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);
        Shader_SetPixelOptionAlphaRate(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // UI配置設定
    g_RendererImageProcessor->SetDrawWorldImages(false);
    g_RendererImageProcessor->SetDrawUiImages(true);

    // 各2D描画プロセッサーの実行
    g_RendererImageProcessor->Process(pScene);
    g_RendererSliderProcessor->Process(pScene);
    g_RendererFontProcessor->Process(pScene);

   

    /*g_CameraProcessor->BindMatrix(0);
    DebugRenderer_DrawFlush();*/

    // デバッグ描画用バッファリセット
    DebugRenderer_ResetBuffer();

    //Direct3D_Present();
}
