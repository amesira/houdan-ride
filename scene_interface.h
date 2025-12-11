//---------------------------------------------------
// scene_interface.h
// 
// ・シーンのインターフェース。
// ・データは保持せず、アクセス用のメソッドのみを持つ。
// 
// Author：Miu Kitamura
// Date  ：2025/12/08
//---------------------------------------------------
#ifndef SCENE_INTERFACE_H
#define SCENE_INTERFACE_H

#include <vector>
#include "component_pool.h"
#include "type_id.h"

class IScene {
protected:
    virtual std::vector<IComponentPool*>& ComponentPools() = 0;

public:
    virtual ~IScene() = default;

    virtual void    Initialize() = 0;
    virtual void    Finalize() = 0;

    virtual void    Update() = 0;
    virtual void    Draw() = 0;

    // 型TのComponentPoolを取得
    template<class T>
    ComponentPool<T>* GetComponentPool() {
        auto& m_componentPools = ComponentPools();
        for (IComponentPool* pool : m_componentPools) {
            if(pool->GetTypeID() == TypeID::getTypeID<T>()) {
                return static_cast<ComponentPool<T>*>(pool);
            }
        }
        return nullptr;
    }

    // 型TのComponentPoolを追加
    template<class T>
    ComponentPool<T>*   AddComponentPool() {
        // すでに存在する場合はそれを返す
        auto* pool = GetComponentPool<T>();
        if (pool) return pool;

        // 新規に生成
        auto& m_componentPools = ComponentPools();
        m_componentPools.push_back(new ComponentPool<T>());
        return static_cast<ComponentPool<T>*>(m_componentPools.back());
    }
};

#endif