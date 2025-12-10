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
#include <string>

#include "component.h"
#include "component_pool.h"

#include "behavior.h"

class IScene;

class GameObject {
private:
    IScene* m_pScene = nullptr;

    unsigned int    m_id = 0;
    std::string     m_name = "None";

    bool    m_active = true;
    bool    m_isDestroy = false;
    
    std::vector<Behavior*>  m_pBehaviors = {};

public:
    void    Update() {
        // GameObjectの更新毎、Behavior.Update()を呼び出す。
        for (Behavior* be : m_pBehaviors) {
            if (!be->GetEnable())continue;
            be->Update();
        }
    }

    void    SetID(unsigned int id) { m_id = id; }
    void    SetName(const std::string& name) { m_name = name; }

    unsigned int    GetID() const { return m_id; }
    std::string     GetName() const { return m_name; }

    void    SetActive(bool active) { m_active = active; }
    bool    GetActive() { return m_active; }

    // GameObjectの破棄予約
    void    Destroy() {
        if(m_isDestroy)return;
        m_isDestroy = true;
    }

    // Componentの追加（このGameObjectのためのComponentを生成する）
    template<class T>
    T*    AddComponent();

    template<class T>
    T* GetComponent() const;

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


