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
#include "game_object.h"

class IScene {
private:
    static constexpr int MAX_GAMEOBJECTS = 1024;
    std::vector<GameObject>     m_gameObjects = {};

    int         m_gameObjectCount = 0;

public:
    IScene() {
        m_gameObjects.reserve(MAX_GAMEOBJECTS);
    }
    virtual ~IScene() = default;

    virtual void    Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) = 0;
    virtual void    Finalize() = 0;

    virtual void    Update() = 0;
    void            UpdateGameObjects() {
        for (GameObject& gameObject : m_gameObjects) {
            gameObject.Update();
        }
    }
    virtual void    Draw() = 0;

    // GameObjectの生成
    // ・return: 生成したGameObjectへのポインタ
    GameObject* CreateGameObject() {
        assert(m_gameObjects.size() < MAX_GAMEOBJECTS && "IScene has reached its maximum GameObject capacity.");
        GameObject gameObject;
        gameObject.SetID(m_gameObjectCount++);
        m_gameObjects.push_back(gameObject);
        return &m_gameObjects.back();
    }
};

#endif