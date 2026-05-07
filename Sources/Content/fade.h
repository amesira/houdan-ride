//----------------------------------------------------
// fade.h [�t�F�[�h�N���X]
// 
// Author�FMiu Kitamura
// Date  �F2025/09/08
//----------------------------------------------------
#ifndef FADE_H
#define FADE_H

// directX
#include "Sources/System/direct3d.h"

#include "Sources/System/Graphic/sprite.h"
#include "Sources/Manager/manager.h"

enum FADE_STATE {
    FADE_NONE = 0,
    FADE_IN,
    FADE_OUT,
};

struct FadeObject {
    FADE_STATE  state;
    float       count;
    float       frame;
    XMFLOAT4    fadeColor;
    SCENE       scene;
};

// ���C������
void Fade_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Fade_Finalize();
void Fade_Update();
void Fade_Draw();

void SetFade(int fadeFrame, XMFLOAT4 color,
    FADE_STATE state, SCENE scene);
FADE_STATE GetFadeState();

#endif