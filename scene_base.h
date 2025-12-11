//---------------------------------------------------
// scene_base.h
// 
// ・Sceneの基底クラス。
// ・データの実体やアドレスを管理する。
// 
// Author：Miu Kitamura
// Date  ：2025/12/11
//---------------------------------------------------
#ifndef SCENE_BASE_H
#define SCENE_BASE_H

#include "scene_interface.h"
#include "game_object.h"

class SceneBase : public IScene {
private:
    static constexpr int    MAX_GAMEOBJECTS = 1024; // シーン内の最大GameObject数
    int                     m_gameObjectCount = 0;  // 生成したGameObject数カウンタ

    // GameObjectリスト
    std::vector<GameObject>     m_gameObjects = {};

    // ComponentPoolリスト
    std::vector<IComponentPool*> m_componentPools;

protected: // IScene implementation
    std::vector<IComponentPool*>& ComponentPools() override {
        return m_componentPools;
    }

public:
    SceneBase() {
        m_gameObjects.reserve(MAX_GAMEOBJECTS);
        m_componentPools.reserve(32);
    }

    virtual void   Initialize() override = 0;
    virtual void   Finalize() override = 0;
    virtual void   Update() override = 0;
    virtual void   Draw() override = 0;

    // GameObjectの生成
    // ・return: 生成したGameObjectへのポインタ
    GameObject* CreateGameObject() {
        assert(m_gameObjects.size() < MAX_GAMEOBJECTS && "IScene has reached its maximum GameObject capacity.");

        GameObject* pGameObject = &m_gameObjects.emplace_back();
        pGameObject->SetID(m_gameObjectCount++);
        pGameObject->SetScene(this);

        return pGameObject;
    }

    // GameObjectリストの取得
    std::vector<GameObject>& GetGameObjects() { return m_gameObjects; }

};

#endif