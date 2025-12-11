//---------------------------------------------------
// scene_interface.h
// 
// Author：Miu Kitamura
// Date  ：2025/12/08
//---------------------------------------------------
#ifndef SCENE_INTERFACE_H
#define SCENE_INTERFACE_H


// directx
#include <d3d11.h>
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;

#include <vector>
#include "component_pool.h"

class IScene {
private:
    
    std::vector<IComponentPool*> m_componentPools = {};

public:
    IScene() {
        
    }
    virtual ~IScene() = default;

    virtual void    Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) = 0;
    virtual void    Finalize() = 0;

    virtual void    Update() = 0;
    virtual void    Draw() = 0;

    template<class T>
    ComponentPool<T>* GetComponentPool() {
        for (IComponentPool* pool : m_componentPools) {
            if(pool->GetIComponentID() == ComponentPool<T>::GetComponentID()) {
                return static_cast<ComponentPool<T>*>(pool);
            }
        }
        return nullptr;
    }

    template<class T>
    ComponentPool<T>*   AddComponentPool(ComponentPool<T>* pool) {
        m_componentPools.push_back(pool);
        return static_cast<ComponentPool<T>*>(m_componentPools.back());
    }
};

#endif