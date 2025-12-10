#include "game_object.h"
#include "scene_interface.h"
#include "component_pool.h"

template<class T>
T* GameObject::AddComponent()
{
    T* component = nullptr;

    auto* compPool = m_pScene->GetComponentPool<T>();

    if (compPool) {
        component = compPool->Create(m_id);
    }
    else {
        compPool = m_pScene->AddComponentPool(ComponentPool<T>());
        component = compPool->Create(m_id);
    }

    // コンポーネントが生成できたらオーナーを設定して返す
    if (component){
        component->SetOwner(this);
        return component;
    }

    return nullptr;
}

template<class T>
T* GameObject::GetComponent() const
{
    T* component = nullptr;

    auto* compPool = m_pScene->GetComponentPool<T>();

    if (compPool) {
        return compPool->GetByGameObjectID(m_id);
    }

    return nullptr;
}
