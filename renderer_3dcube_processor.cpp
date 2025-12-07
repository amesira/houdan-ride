//===================================================
// renderer_3dcube_processor.cpp
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//===================================================
#include "renderer_3dcube_processor.h"

// directX
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
using namespace DirectX;

// graphics, devices
#include "shader.h"
#include "sprite.h"
#include "keyboard.h"
#include "polygon3d.h"

// component
#include "transform_component.h"
#include "cubemesh_component.h"

static ID3D11ShaderResourceView* g_DefTexture = NULL;

void Renderer3DCubeProcessor::Initialize()
{
    LoadTexture(&g_DefTexture, L"asset\\Texture\\sample.png");
}

void Renderer3DCubeProcessor::Finalize()
{

}

void Renderer3DCubeProcessor::Process()
{
    for (Components& cmps : m_components) {
        ID3D11ShaderResourceView* tex = cmps.m_cubemesh->GetTexture();
        if (!tex)tex = g_DefTexture;

        Polygon3D_CubeRender(
            cmps.m_transform->GetPosition(),
            cmps.m_transform->GetRotation(),
            cmps.m_transform->GetScaling(),
            tex,
            cmps.m_cubemesh->GetColor()
        );
    }
}