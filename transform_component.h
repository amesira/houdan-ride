//===================================================
// transform_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//===================================================
#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"

#include "component.h"

class TransformComponent : public Component {
private:
    DirectX::XMFLOAT3   m_position = { 0.0f,0.0f,0.0f };
    DirectX::XMFLOAT3   m_rotation = { 0.0f,0.0f,0.0f };
    DirectX::XMFLOAT3   m_scaling = { 1.0f,1.0f,1.0f };

    DirectX::XMFLOAT3   m_prevPosition = { 0.0f,0.0f,0.0f };

public:
    void    SetPosition(DirectX::XMFLOAT3 position) { m_position = position; }
    void    SetRotation(DirectX::XMFLOAT3 rotation) { m_rotation = rotation; }
    void    SetScaling(DirectX::XMFLOAT3 scaling) { m_scaling = scaling; }

    DirectX::XMFLOAT3   GetPosition()const { return m_position; }
    DirectX::XMFLOAT3   GetRotation()const { return m_rotation; }
    DirectX::XMFLOAT3   GetScaling()const { return m_scaling; }

    void    SetPrevPosition(DirectX::XMFLOAT3 prevPosition){ m_prevPosition = prevPosition; }
    DirectX::XMFLOAT3   GetPrevPosition()const { return m_prevPosition; }

};


#endif