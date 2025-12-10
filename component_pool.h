//---------------------------------------------------
// component_pool.h
// ・型ごとにComponentを管理するプール。
// 
// Author：Miu Kitamura
// Date  ：2025/12/07
//---------------------------------------------------
#ifndef COMPONENT_POOL_H
#define COMPONENT_POOL_H

#include <vector>
#include <assert.h>

class GameObject;

// ComponentPoolのインターフェース
class IComponentPool {
    int m_iComponentId = -1;

    IComponentPool() = default;

public:
    int     GetIComponentID() const { return m_iComponentId; }
};

// ComponentPoolクラス
template <class T>
class ComponentPool : public IComponentPool {
private:
    static constexpr size_t COMPONENTS_MAX = 1024;

    // Componentリスト
    std::vector<T>              m_components = {};

    // GameObjectのIDリスト
    // ・m_componentsとインデックスを対応させる
    std::vector<unsigned int>   m_gameObjectIDs = {};

    static int m_componentId;

public:
    ComponentPool() {
        static int idCounter = 0;
        m_iComponentId = idCounter++;
        m_componentId = m_iComponentId;

        m_components.reserve(COMPONENTS_MAX);
        m_gameObjectIDs.reserve(COMPONENTS_MAX);
    }

    static int GetComponentID() {
        return m_componentId;
    }

    // Componentを生成してComponentPoolに追加
    // ・pGameObject: Componentを所有するGameObjectへのポインタ
    T*      Create(unsigned int gameObjectID) {
        assert(m_components.size() < COMPONENTS_MAX && "ComponentPool has reached its maximum capacity.");
        T component;
        m_components.push_back(component);
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