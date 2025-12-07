//===================================================
// game_object.h [ゲームオブジェクト]
// 
// ・Component, Behavior のリストを保持する箱。
// ・Player, Enemy などはすべてこの箱に何を入れるかで構成される。
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//===================================================
#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <vector>
#include "component.h"
#include "behavior.h"

class GameObject {
private:
    bool    m_active = true;
    
    std::vector<Component*> m_pComponents = {};
    std::vector<Behavior*>  m_pBehaviors = {};

public:
    void    Update() {
        // GameObjectの更新毎、Behavior.Update()を呼び出す。
        for (Behavior* be : m_pBehaviors) {
            if (!be->GetEnable())continue;
            be->Update();
        }
    }
    void    Finalize() {
        // Componentをdelete
        int length = m_pComponents.size();
        for (int i = 0; i < length; i++) {
            Component* cmp = m_pComponents[length - (i + 1)];
            delete cmp;
        }
        m_pComponents.clear();

        // Behaviorをdelete
        length = m_pBehaviors.size();
        for (int i = 0; i < length; i++) {
            Behavior* be = m_pBehaviors[length - (i + 1)];
            delete be;
        }
        m_pBehaviors.clear();
    }

    void    SetActive(bool active) { m_active = active; }
    bool    GetActive() { return m_active; }

    // Componentの追加・取得
    void    AddComponent(Component* pCmp) {
        pCmp->SetOwner(this);
        m_pComponents.push_back(pCmp);
    }
    template<class T>
    T* GetComponent() const {
        for (Component* pCmp : m_pComponents) {
            // 取得したいComponentかどうかをタイプチェック
            if (pCmp->GetType() == T::GetTypeStatic()) {
                T* t = static_cast<T*>(pCmp);
                if (t)return t;
            }
        }
        return nullptr;
    }

    // Behaviorの追加・取得
    void    AttachBehavior(Behavior* pBe) {
        pBe->SetOwner(this);
        m_pBehaviors.push_back(pBe);
    }
    template<class T>
    T* GetBehavior() const {
        for (Behavior* pBe : m_pBehaviors) {
            // 取得したいComponentかどうかをタイプチェック
            if (pBe->GetType() == T::GetTypeStatic()) {
                T* t = static_cast<T*>(pBe);
                if (t)return t;
            }
        }
        return nullptr;
    }
};

#endif