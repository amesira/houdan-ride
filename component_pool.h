//---------------------------------------------------
// component_pool.h
// 
// ・型ごとにComponentを管理するプール。
// 
// Author：Miu Kitamura
// Date  ：2025/12/07
//---------------------------------------------------
#ifndef COMPONENT_POOL_H
#define COMPONENT_POOL_H

#include <vector>
#include <assert.h>

#include "type_id.h"
#include "component_pool_interface.h"

class GameObject;

template <class T>
class ComponentPool : public IComponentPool {
private:
    static constexpr size_t COMPONENTS_MAX = 1024;

    // Componentリスト
    std::vector<T>              m_components = {};

    // GameObjectのIDリスト
    // ・m_componentsとインデックスを対応させる
    std::vector<unsigned int>   m_gameObjectIDs = {};

    // 空きスロット管理用リスト
    std::vector<size_t>         m_freeIndices = {};

public:
    ComponentPool() : IComponentPool(ComponentTypeID::getTypeID<T>()) {
        m_components.reserve(COMPONENTS_MAX);
        m_gameObjectIDs.reserve(COMPONENTS_MAX);
        m_freeIndices.clear();
    }

    // Componentを生成してComponentPoolに追加
    // ・pGameObject: Componentを所有するGameObjectへのポインタ
    T*      Create(unsigned int gameObjectID) {
        // 空きスロットがあればそこに追加
       /* if (m_freeIndices.size() > 0) {
            size_t index = m_freeIndices.back();
            m_freeIndices.pop_back();
            m_components[index] = T();
            m_gameObjectIDs[index] = gameObjectID;

            return &m_components[index];
        }*/

        assert(m_components.size() < COMPONENTS_MAX && "ComponentPool has reached its maximum capacity.");
        
        m_components.emplace_back();    // Tのデフォルトコンストラクタを呼び出して追加
        m_gameObjectIDs.push_back(gameObjectID);

        return &m_components.back();
    }

    // ComponentPoolからComponentを削除
    // ・gameObjectID: 削除するComponentを所有するGameObjectのID
    void    Remove(unsigned int gameObjectID) {
        for (int i = 0; i < m_components.size(); i++) {
            unsigned int id = m_gameObjectIDs[i];
            // 指定されたGameObjectIDと一致したら削除
            if (id == gameObjectID) {
                m_components.erase(m_components.begin() + i);
                m_gameObjectIDs.erase(m_gameObjectIDs.begin() + i);
                
                // 空きスロットとして管理リストに追加
                m_freeIndices.push_back(i);

                //Component* comp = &m_components[i];
                //comp->SetEnable(false); // 無効化しておく

                return;
            }
        }
    }

    // GameObjectIDからComponentを取得
    // ・gameObjectID: 取得するComponentを所有するGameObjectのID
    T*  GetByGameObjectID(unsigned int gameObjectID) {
        for (int i = 0; i < m_components.size(); i++) {
            unsigned int id = m_gameObjectIDs[i];
            // 指定されたGameObjectIDと一致したらComponentを返す
            if (id == gameObjectID) {
                return &m_components[i];
            }
        }
        return nullptr;
    }

    // ComponentPool内のComponentリストを取得
    std::vector<T>&     GetList() { return m_components; }
};

#endif