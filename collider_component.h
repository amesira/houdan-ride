//===================================================
// collider_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//===================================================
#ifndef COLLIDER_COMPONENT_H
#define COLLIDER_COMPONENT_H

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"
using namespace DirectX;

#include "component.h"

class ColliderComponent :public Component {
public:
    enum class Shape {
        Box,
    };
    struct CollisionData {
        ColliderComponent*  m_other;    // 衝突相手
        DirectX::XMFLOAT3   m_mtv;      // 最小移動ベクトル

        bool    m_isCollision;          // 当たり判定フラグ
        bool    m_wasCollision;         // 前フレームの当たり判定フラグ

        bool    GetCollisionEnter() { return m_isCollision && !m_wasCollision; }
        bool    GetCollisionStay() { return m_isCollision; }
        bool    GetCollisionExit() { return !m_isCollision && m_wasCollision; }
    };

    // 衝突情報の最大登録数
    static const int MAX_COLLISION_DATA = 8;

protected:
    Shape m_shape;

private:
    CollisionData m_collisionData[MAX_COLLISION_DATA];
    
public:
    ~ColliderComponent() = default;

    // コライダーの形状取得
    Shape   GetShape()const { return m_shape; }

    void    UpdateCollisionData() {
        for (int i = 0; i < MAX_COLLISION_DATA; i++) {
            CollisionData& data = m_collisionData[i];
            if (data.m_other) {
                data.m_wasCollision = data.m_isCollision;
                data.m_isCollision = false;
                if (!data.m_wasCollision)data.m_other = nullptr;
            }
        }
    }

    // 衝突情報登録
    int     RegisterCollisionData(ColliderComponent* other, DirectX::XMFLOAT3 mtv) {
        CollisionData* slot = nullptr;

        for(int i = 0; i < MAX_COLLISION_DATA; i++){
            CollisionData& data = m_collisionData[i];
            if(data.m_other == other) {
                slot = &data;
                break;
            }
            else if (data.m_other == nullptr) {
                slot = &data;
            }
        }

        if (slot) {
            slot->m_other = other;
            slot->m_mtv = mtv;
            slot->m_isCollision = true;
            return 1;
        }

        return 0;
    }

    // 最小移動ベクトルの最小値取得
    DirectX::XMFLOAT3& GetMinMtv() {
        DirectX::XMFLOAT3 minMtv = { 0.0f,0.0f,0.0f };
        for (int i = 0; i < MAX_COLLISION_DATA; i++) {
            CollisionData& data = m_collisionData[i];
            if (data.m_isCollision && data.m_other) {
                if (data.m_mtv.x < minMtv.x) {
                    minMtv.x = data.m_mtv.x;
                }
                if (data.m_mtv.y < minMtv.y) {
                    minMtv.y = data.m_mtv.y;
                }
                if (data.m_mtv.z < minMtv.z) {
                    minMtv.z = data.m_mtv.z;
                }
            }
        }
        return minMtv;
    }

    // 最小移動ベクトルの最大値取得
    DirectX::XMFLOAT3& GetMaxMtv() {
        DirectX::XMFLOAT3 maxMtv = { 0.0f,0.0f,0.0f };
        for (int i = 0; i < MAX_COLLISION_DATA; i++) {
            CollisionData& data = m_collisionData[i];
            if (data.m_isCollision && data.m_other) {
                if (data.m_mtv.x > maxMtv.x) {
                    maxMtv.x = data.m_mtv.x;
                }
                if (data.m_mtv.y > maxMtv.y) {
                    maxMtv.y = data.m_mtv.y;
                }
                if (data.m_mtv.z > maxMtv.z) {
                    maxMtv.z = data.m_mtv.z;
                }
            }
        }
        return maxMtv;
    }
};

class BoxColliderComponent :public ColliderComponent {
private:
    DirectX::XMFLOAT3   m_anchor = { 0.0f,0.0f,0.0f };
    DirectX::XMFLOAT3   m_scale = { 1.0f,1.0f,1.0f };

public:
    BoxColliderComponent() { m_shape = ColliderComponent::Shape::Box; }

    void    SetAnchor(DirectX::XMFLOAT3 anchor) { m_anchor = anchor; }
    void    SetScale(DirectX::XMFLOAT3 scale) { m_scale = scale; }

    DirectX::XMFLOAT3   GetAnchor() { return m_anchor; }
    DirectX::XMFLOAT3   GetScale() { return m_scale; }

};


#endif