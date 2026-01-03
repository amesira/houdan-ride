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
#include "model_component.h"
#include "camera_component.h"

// behavior
#include "player_behavior.h"
#include "tps_camera_behavior.h"

void Factory::CreateTpsCamera(GameObject* obj, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 atPosition)
{
    obj->SetName("TPSCamera");

    // component生成・登録
    TransformComponent* transform = obj->AddComponent<TransformComponent>();
    CameraComponent* camera = obj->AddComponent<CameraComponent>();
    
    // component設定
    transform->SetPosition(position);

    camera->SetAtPosition(atPosition);
    camera->SetFov(60.0f);
    camera->SetAspect(16.0f / 9.0f);
    camera->SetNearClip(0.1f);
    camera->SetFarClip(100.0f);

    // behavior生成・登録
    TpsCameraBehavior* tpsCameraBe = obj->AddBehavior<TpsCameraBehavior>();
}

void Factory::CreatePlayer(GameObject* player, DirectX::XMFLOAT3 position)
{
    player->SetName("Player");

    // component生成・登録
    TransformComponent* transform = player->AddComponent<TransformComponent>();
    ModelComponent* modelComp = player->AddComponent<ModelComponent>();

    SphereColliderComponent* collider = player->AddComponent<SphereColliderComponent>();
    RigidbodyComponent* rigidbody = player->AddComponent<RigidbodyComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetScaling({ 1.0f, 1.0f, 1.0f });

    collider->SetRadius(1.0f);
    rigidbody->SetMass(1.5f);
    rigidbody->SetFriction({ 0.98f, 1.0f, 0.98f });

    modelComp->LoadModel("asset\\Model\\ico_sphere.fbx");

    // behavior生成・登録
    PlayerBehavior* playerBe = player->AddBehavior<PlayerBehavior>();
}

void Factory::CreatePlayer_Chara(GameObject* player)
{
    player->SetName("Player_Chara");

    // component生成・登録
    TransformComponent* transform = player->AddComponent<TransformComponent>();
    ImageComponent* imageComp = player->AddComponent<ImageComponent>();

    // component設定
    transform->SetPosition({ 0.0f,0.0f,0.0f });
    transform->SetScaling({ 1.0f,1.3f,1.0f });
    imageComp->Load(L"asset\\Texture\\player.png");
    imageComp->SetIsBillboard(true);
}

void Factory::CreateBox(GameObject* cube, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT4 color)
{
    // component生成・登録
    TransformComponent* transform = cube->AddComponent<TransformComponent>();
    CubemeshComponent* cubemesh = cube->AddComponent<CubemeshComponent>();
    BoxColliderComponent* collider = cube->AddComponent<BoxColliderComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetEulerRotation(rotation);
    transform->SetScaling(scaling);

    cubemesh->SetColor(color);
    collider->SetScale(scaling);
}

void Factory::CreateUiText(GameObject* uiText, DirectX::XMFLOAT3 position, const char8_t* text, float fontSize, DirectX::XMFLOAT4 color, bool isCenter)
{
    // component生成・登録
    RectTransformComponent* rectTransform = uiText->AddComponent<RectTransformComponent>();
    TextComponent* textComponent = uiText->AddComponent<TextComponent>();
    ImageComponent* imageComponent = uiText->AddComponent<ImageComponent>();

    // component設定
    rectTransform->SetPosition(position);
    rectTransform->SetScaling({ 100.0f,100.0f,1.0f });

    textComponent->SetText(text);
    textComponent->SetFontSize((int)fontSize);
    textComponent->SetColor(color);
    textComponent->SetCenter(isCenter);

    imageComponent->SetColor({ 1.0f,1.0f,1.0f,1.0f }); // 透明にしておく
    imageComponent->SetUvRect({ 0.0f,0.0f,1.0f,1.0f });
    imageComponent->Load(L"asset\\Texture\\test.jpg");
}