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

void Factory::CreateTestPlayer(GameObject* player, DirectX::XMFLOAT3 position)
{
    // component生成・登録
    TransformComponent* transform = player->AddComponent<TransformComponent>();
    CubemeshComponent* cubemesh = player->AddComponent<CubemeshComponent>();
    BoxColliderComponent* collider = player->AddComponent<BoxColliderComponent>();
    RigidbodyComponent* rigidbody = player->AddComponent<RigidbodyComponent>();

    // component設定
    transform->SetPosition(position);
    cubemesh->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

    // processor登録
    GetPhysicsProcessor()->Entry(transform,rigidbody);
    GetCollisionProcessor()->Entry(transform, collider);
    GetDynamicsProcessor()->Entry(transform,collider,rigidbody);

    // behavior生成・登録
    PlayerBehavior* playerBe = new PlayerBehavior(player);
    player->AttachBehavior(playerBe);
}

void Factory::CreateBox(GameObject* cube, DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 scaling)
{
    // component生成・登録
    TransformComponent* transform = cube->AddComponent<TransformComponent>();
    CubemeshComponent* cubemesh = cube->AddComponent<CubemeshComponent>();
    BoxColliderComponent* collider = cube->AddComponent<BoxColliderComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetScaling(scaling);
    cubemesh->SetColor(color);

    // processor登録
    GetCollisionProcessor()->Entry(transform, collider);
}

void Factory::CreateUiText(GameObject* uiText, DirectX::XMFLOAT3 position, const char8_t* text, float fontSize, DirectX::XMFLOAT4 color, bool isCenter)
{
    // component生成・登録
    RectTransformComponent* rectTransform = uiText->AddComponent<RectTransformComponent>();
    TextComponent* textComponent = uiText->AddComponent<TextComponent>();
    ImageComponent* imageComponent = uiText->AddComponent<ImageComponent>();

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
}