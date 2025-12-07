//===================================================
// rect_transform_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/11/18
//===================================================
#ifndef RECT_TRANSFORM_COMPONENT_H
#define RECT_TRANSFORM_COMPONENT_H

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"

#include "component.h"

class RectTransformComponent : public Component {
private:
    DirectX::XMFLOAT3   m_position = { 0.0f,0.0f,0.0f };
    DirectX::XMFLOAT3   m_rotation = { 0.0f,0.0f,0.0f };
    DirectX::XMFLOAT3   m_scaling = { 1.0f,1.0f,1.0f };

    // アンカー（-1.0～1.0）
    // ・初期状態では中央に設定されている。
    /*DirectX::XMFLOAT2   m_anchorMin = { 0.0f,0.0f };
    DirectX::XMFLOAT2   m_anchorMax = { 0.0f,0.0f };*/

public:
    // 型チェック用関数（GetComponent用）
    static Type GetTypeStatic() {
        return Type::RectTransform;
    }
    Type GetType() const override {
        return GetTypeStatic();
    }

    void    SetPosition(DirectX::XMFLOAT3 position) { m_position = position; }
    void    SetRotation(DirectX::XMFLOAT3 rotation) { m_rotation = rotation; }
    void    SetScaling(DirectX::XMFLOAT3 scaling) { m_scaling = scaling; }

    DirectX::XMFLOAT3   GetPosition()const { return m_position; }
    DirectX::XMFLOAT3   GetRotation()const { return m_rotation; }
    DirectX::XMFLOAT3   GetScaling()const { return m_scaling; }

   /* void    SetAnchor(DirectX::XMFLOAT2 anchorMin, DirectX::XMFLOAT2 anchorMax) {
        m_anchorMin = anchorMin;
        m_anchorMax = anchorMax;
    }

    DirectX::XMFLOAT2   GetAnchorMin()const { return m_anchorMin; }
    DirectX::XMFLOAT2   GetAnchorMax()const { return m_anchorMax; }*/
};


#endif