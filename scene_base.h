//---------------------------------------------------
// scene_base.h
// 
// Author：Miu Kitamura
// Date  ：2025/12/11
//---------------------------------------------------
#ifndef SCENE_BASE_H
#define SCENE_BASE_H

// directx
#include <d3d11.h>
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;

#include "scene_interface.h"
#include "game_object.h"

class SceneBase : public IScene {
    static constexpr int MAX_GAMEOBJECTS = 1024;
    std::vector<GameObject>     m_gameObjects = {};

    int         m_gameObjectCount = 0;

public:
    SceneBase() {
        m_gameObjects.reserve(MAX_GAMEOBJECTS);
    }

    virtual void   Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) override = 0;
    virtual void   Finalize() override = 0;
    virtual void   Update() override = 0;
    virtual void   Draw() override = 0;

    std::vector<GameObject>& GetGameObjects() { return m_gameObjects; }

    // GameObjectの生成
   // ・return: 生成したGameObjectへのポインタ
    GameObject* CreateGameObject() {
        assert(m_gameObjects.size() < MAX_GAMEOBJECTS && "IScene has reached its maximum GameObject capacity.");
        GameObject gameObject;
        gameObject.SetID(m_gameObjectCount++);
        gameObject.SetScene(this);
        m_gameObjects.push_back(gameObject);
        return &m_gameObjects.back();
    }
};

#endif