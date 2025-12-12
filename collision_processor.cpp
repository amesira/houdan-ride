//===================================================
// collision_processor.cpp [当たり判定プロセッサー]
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//===================================================
#include "collision_processor.h"

#include "game_object.h"
#include "scene_interface.h"
#include "transform_component.h"
#include "collider_component.h"

void CollisionProcessor::Initialize()
{
    m_checkType = CheckType::AABB;
}

void CollisionProcessor::Finalize()
{

}

void CollisionProcessor::Process(IScene* pScene)
{
    auto* boxColliderPool = pScene->GetComponentPool<BoxColliderComponent>();
    auto* transformPool = pScene->GetComponentPool<TransformComponent>();

    auto& boxColliderList = boxColliderPool->GetList();

    // 衝突情報の更新
    for (BoxColliderComponent& c : boxColliderList) {
        c.UpdateCollisionData();
    }

    // 当たり判定処理
    for (int i = 0; i < boxColliderList.size(); i++) {
        for (int j = 0; j < boxColliderList.size() - (i + 1); j++) {
            BoxColliderComponent* colliderA = &boxColliderList[i];
            BoxColliderComponent* colliderB = &boxColliderList[i + (j + 1)];
            TransformComponent* transformA = transformPool->GetByGameObjectID(colliderA->GetOwner()->GetID());
            TransformComponent* transformB = transformPool->GetByGameObjectID(colliderB->GetOwner()->GetID());

            CollisionResult result = {};
            result.isCollision = false;

            // 判定方法がAABBである
            if (m_checkType == CheckType::AABB) {
                Bounds a = ConvertToBounds(transformA, colliderA);
                Bounds b = ConvertToBounds(transformB, colliderB);
                result = CheckAABB(a, b);
            }

            // 衝突している場合
            // ・衝突情報を登録
            if (result.isCollision) {
                colliderA->RegisterCollisionData(colliderB, result.mtv);
                colliderB->RegisterCollisionData(colliderA, {
                    -result.mtv.x,
                    -result.mtv.y,
                    -result.mtv.z });
            }
        }
    }
}

//===================================================
// AABB境界情報の計算
//===================================================
CollisionProcessor::Bounds CollisionProcessor::ConvertToBounds(
    TransformComponent* transform, BoxColliderComponent* collider)
{
    Bounds bounds = {};

    // ワールド座標系での中心座標・サイズを計算
    DirectX::XMFLOAT3 pos = {
    transform->GetPosition().x + collider->GetAnchor().x,
    transform->GetPosition().y + collider->GetAnchor().y,
    transform->GetPosition().z + collider->GetAnchor().z
    };
    DirectX::XMFLOAT3 size = {
    transform->GetScaling().x * collider->GetScale().x,
    transform->GetScaling().y * collider->GetScale().y,
    transform->GetScaling().z * collider->GetScale().z };

    // 頂点の内、最も小さいもの大きいものを計算
    bounds.minX = pos.x - size.x / 2.0f;
    bounds.maxX = pos.x + size.x / 2.0f;
    bounds.minY = pos.y - size.y / 2.0f;
    bounds.maxY = pos.y + size.y / 2.0f;
    bounds.minZ = pos.z - size.z / 2.0f;
    bounds.maxZ = pos.z + size.z / 2.0f;

    return bounds;
}

//===================================================
// AABB同士の衝突判定
//===================================================
CollisionProcessor::CollisionResult CollisionProcessor::CheckAABB(Bounds a, Bounds b)
{
    CollisionResult result = {};

    // 衝突判定
    result.isCollision= (
        a.minX <= b.maxX &&
        a.maxX >= b.minX &&
        a.minY <= b.maxY &&
        a.maxY >= b.minY &&
        a.minZ <= b.maxZ &&
        a.maxZ >= b.minZ
        );

    // Aの最小移動ベクトル
    // ・BとAが重なっている場合、AをBの外に出すためにAが移動すべき最小のベクトル
    result.mtv = { 0.0f,0.0f,0.0f };
    if(result.isCollision){
        float dx = (a.minX < b.minX) ? (b.minX - a.maxX) : (b.maxX - a.minX);
        float dy = (a.minY < b.minY) ? (b.minY - a.maxY) : (b.maxY - a.minY);
        float dz = (a.minZ < b.minZ) ? (b.minZ - a.maxZ) : (b.maxZ - a.minZ);

        // XYZの中で必要な押し出し量が最も小さい軸を採用
        if (abs(dx) < abs(dy) && abs(dx) < abs(dz)) {
            result.mtv.x = dx;
        }
        else if (abs(dy) < abs(dz)) {
            result.mtv.y = dy;
        }
        else {
            result.mtv.z = dz;
        }
    }

    return result;
}
