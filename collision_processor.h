//----------------------------------------------------
// collision_processor.h [当たり判定プロセッサー]
// 
// ・当たり判定を取る。
// ・物理処理群の２番目。
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//----------------------------------------------------
#ifndef COLLISION_PROCESSOR_H
#define COLLISION_PROCESSOR_H

#include <vector>
#include "processor.h"

#include "direct3d.h"
#include <DirectXMath.h>

class TransformComponent;
class ColliderComponent;
class BoxColliderComponent;

class CollisionProcessor : public Processor {
public:
    enum class CheckType {
        AABB,
        OBB,
    };

private:
    CheckType   m_checkType;

public:
    void    Initialize()override;
    void    Finalize()override;

    void    Process(IScene* pScene)override;

private:
    // AABB境界情報
    struct Bounds {
        float   minX, maxX;
        float   minY, maxY;
        float   minZ, maxZ;
    };
    // 衝突判定結果
    struct CollisionResult {
        bool                isCollision;
        DirectX::XMFLOAT3   mtv;
    };

    Bounds  ConvertToBounds(TransformComponent* transform, BoxColliderComponent* collider);
    CollisionResult    CheckAABB(Bounds a, Bounds b);
};


#endif