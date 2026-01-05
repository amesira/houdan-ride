//===================================================
// light_processor.cpp
// 
// Author：Miu Kitamura
// Date  ：2025/01/05
//===================================================
#include "light_processor.h"
#include "scene_interface.h"

#include "shader.h"
#include "light_component.h"

void LightProcessor::Initialize()
{

}

void LightProcessor::Finalize()
{

}

void LightProcessor::Process(IScene* pScene)
{
    int lightIndex = 0;

    // コンポーネントプール取得
    auto* lightCompPool = pScene->GetComponentPool<LightComponent>();
    if (!lightCompPool) return;

    // ライトコンポーネントリストを取得して処理
    auto& lightCompList = lightCompPool->GetList();

    for (LightComponent& light : lightCompList) {
        if (!light.GetEnable()) continue;

        Shader_SetLight(
            lightIndex,
            light.GetDirection(),
            light.GetDiffuse(),
            light.GetAmbient()
        );
        lightIndex++;
        if (lightIndex >= MAX_LIGHT) break;
    }

    // 残りのライトを無効化
    for (int i = 0; i < MAX_LIGHT - lightIndex; i++) {
        int index = lightIndex + i;
        Shader_SetLightEnable(index, false);
    }
}

void LightProcessor::SetUiLight()
{
    // デフォルトライト設定
    Shader_SetLight(
        0,
        XMFLOAT4{ 0.0f, -1.0f, 0.0f, 0.0f },
        XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f },
        XMFLOAT4{ 0.2f, 0.2f, 0.2f, 1.0f }
    );

    // 残りのライトを無効化
    for (int i = 1; i < MAX_LIGHT; i++) {
        Shader_SetLightEnable(i, false);
    }
}
