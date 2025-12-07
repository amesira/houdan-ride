//----------------------------------------------------
// factory.cpp [ゲームオブジェクト工場]
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//----------------------------------------------------
#include "factory.h"

// component
#include "transform_component.h"
#include "cubemesh_component.h"
#include "collider_component.h"
#include "rigidbody_component.h"
#include "rect_transform_component.h"
#include "text_component.h"
#include "image_component.h"

// behavior
#include "player_behavior.h"

// processor
#include "processor_manager.h"
#include "renderer_3dcube_processor.h"
#include "physics_processor.h"
#include "collision_processor.h"
#include "dynamics_processor.h"
#include "renderer_font_processor.h"
#include "renderer_image_processor.h"

GameObject* Factory::CreateTestPlayer(DirectX::XMFLOAT3 position)
{
    GameObject* player = new GameObject();

    // component生成・登録
    TransformComponent* transform = new TransformComponent();
    CubemeshComponent* cubemesh = new CubemeshComponent();
    BoxColliderComponent* collider = new BoxColliderComponent();
    RigidbodyComponent* rigidbody = new RigidbodyComponent();
    player->AddComponent(transform);
    player->AddComponent(cubemesh);
    player->AddComponent(collider);
    player->AddComponent(rigidbody);

    // component設定
    transform->SetPosition(position);
    cubemesh->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

    // processor登録
    GetRenderer3DCubeProcessor()->Entry(transform, cubemesh);
    GetPhysicsProcessor()->Entry(transform,rigidbody);
    GetCollisionProcessor()->Entry(transform, collider);
    GetDynamicsProcessor()->Entry(transform,collider,rigidbody);

    // behavior生成・登録
    PlayerBehavior* playerBe = new PlayerBehavior(player);
    player->AttachBehavior(playerBe);

    return player;
}

GameObject* Factory::CreateBox(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 scaling)
{
    GameObject* cube = new GameObject();

    // component生成・登録
    TransformComponent* transform = new TransformComponent();
    CubemeshComponent* cubemesh = new CubemeshComponent();
    BoxColliderComponent* collider = new BoxColliderComponent();
    cube->AddComponent(transform);
    cube->AddComponent(cubemesh);
    cube->AddComponent(collider);

    // component設定
    transform->SetPosition(position);
    transform->SetScaling(scaling);
    cubemesh->SetColor(color);

    // processor登録
    GetRenderer3DCubeProcessor()->Entry(transform, cubemesh);
    GetCollisionProcessor()->Entry(transform, collider);

    return cube;
}

GameObject* Factory::CreateUiText(DirectX::XMFLOAT3 position, const char8_t* text, float fontSize, DirectX::XMFLOAT4 color, bool isCenter)
{
    GameObject* uiText = new GameObject();

    // component生成・登録
    RectTransformComponent* rectTransform = new RectTransformComponent();
    TextComponent* textComponent = new TextComponent();
    ImageComponent* imageComponent = new ImageComponent();
    uiText->AddComponent(rectTransform);
    uiText->AddComponent(textComponent);
    uiText->AddComponent(imageComponent);

    // component設定
    rectTransform->SetPosition(position);

    textComponent->SetText(text);
    textComponent->SetFontSize((int)fontSize);
    textComponent->SetColor(color);
    textComponent->SetCenter(isCenter);

    imageComponent->SetColor({ 1.0f,1.0f,0.0f,1.0f }); // 透明にしておく
    imageComponent->SetUvRect({ 0.0f,0.0f,1.0f,1.0f });
    imageComponent->Load(L"asset\\Texture\\test.jpg");

    // processor登録
    GetRendererFontProcessor()->Entry(rectTransform, textComponent);
    GetRendererImageProcessor()->Entry(rectTransform, imageComponent);

    return uiText;
}