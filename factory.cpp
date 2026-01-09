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
#include "light_component.h"

// behavior
#include "player_behavior.h"
#include "tps_camera_behavior.h"
#include "enemy_behavior.h"
#include "switch_sprite_behavior.h"
#include "ball_behavior.h"
#include "woodbox_behavior.h"

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

void Factory::CreateLight(GameObject* obj, DirectX::XMFLOAT4 direction, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 ambient)
{
    // component生成・登録
    LightComponent* lightComp = obj->AddComponent<LightComponent>();
    
    // component設定
    lightComp->SetDirection(direction);
    lightComp->SetDiffuse(diffuse);
    lightComp->SetAmbient(ambient);
}

void Factory::CreatePlayer(GameObject* player, DirectX::XMFLOAT3 position)
{
    player->SetName("Player");

    // component生成・登録
    TransformComponent* transform = player->AddComponent<TransformComponent>();
    SphereColliderComponent* collider = player->AddComponent<SphereColliderComponent>();
    RigidbodyComponent* rigidbody = player->AddComponent<RigidbodyComponent>();
    ImageComponent* imageComp = player->AddComponent<ImageComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetScaling({ 1.3f, 1.5f, 1.3f });

    collider->SetRadius(0.65f);
    rigidbody->SetMass(1.5f);
    rigidbody->SetFriction({ 0.98f, 1.0f, 0.98f });

    imageComp->Load(L"asset\\Texture\\player.png");
    imageComp->SetWorldSpaceType(WorldSpaceType::HD2D);

    // behavior生成・登録
    PlayerBehavior* playerBe = player->AddBehavior<PlayerBehavior>();
    SwitchSpriteBehavior* switchSpriteBe = player->AddBehavior<SwitchSpriteBehavior>();
}

void Factory::CreateBall(GameObject* obj, DirectX::XMFLOAT3 position)
{
    obj->SetName("Ball");

    // component生成・登録
    TransformComponent* transform       = obj->AddComponent<TransformComponent>();
    ModelComponent* modelComp           = obj->AddComponent<ModelComponent>();
    SphereColliderComponent* collider   = obj->AddComponent<SphereColliderComponent>();
    RigidbodyComponent* rigidbody       = obj->AddComponent<RigidbodyComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetScaling({ 1.0f, 1.0f, 1.0f });

    collider->SetRadius(1.0f);
    rigidbody->SetMass(1.5f);
    rigidbody->SetFriction({ 0.98f, 1.0f, 0.98f });

    modelComp->LoadModel("asset\\Model\\ico_sphere.fbx");

    // behavior生成・登録
    BallBehavior* ballBe = obj->AddBehavior<BallBehavior>();
}

void Factory::CreateWoodbox(GameObject* obj, DirectX::XMFLOAT3 position)
{
    obj->SetName("Woodbox");

    // component生成・登録
    TransformComponent* transform = obj->AddComponent<TransformComponent>();
    BoxColliderComponent* collider = obj->AddComponent<BoxColliderComponent>();
    ModelComponent* modelComp = obj->AddComponent<ModelComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetScaling({ 1.0f,1.0f,1.0f });
    collider->SetScale({ 2.0f,2.0f,2.0f });
    modelComp->LoadModel("asset\\Model\\cube.fbx");
    modelComp->SetColor({ 0.55f, 0.27f, 0.07f, 1.0f });

    // behavior生成・登録
    WoodboxBehavior* woodboxBe = obj->AddBehavior<WoodboxBehavior>();
}

void Factory::CreateEnemy(GameObject* enemy, DirectX::XMFLOAT3 position)
{
    // component生成・登録
    TransformComponent* transform = enemy->AddComponent<TransformComponent>();
    SphereColliderComponent* collider = enemy->AddComponent<SphereColliderComponent>();
    RigidbodyComponent* rigidbody = enemy->AddComponent<RigidbodyComponent>();
    ImageComponent* imageComp = enemy->AddComponent<ImageComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetScaling({ 1.0f,1.0f,1.0f });
    collider->SetRadius(0.5f);
    imageComp->Load(L"asset\\Texture\\enemy1.png");
    imageComp->SetWorldSpaceType(WorldSpaceType::HD2D);
    imageComp->SetUvRect({ 0.0f,0.0f,1.0f/3.0f,1.0f/4.0f });

    // behavior生成・登録
    EnemyBehavior* enemyBe = enemy->AddBehavior<EnemyBehavior>();
    SwitchSpriteBehavior* switchSpriteBe = enemy->AddBehavior<SwitchSpriteBehavior>();
}

void Factory::CreateBox(GameObject* cube, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT4 color)
{
    // component生成・登録
    TransformComponent* transform = cube->AddComponent<TransformComponent>();
    BoxColliderComponent* collider = cube->AddComponent<BoxColliderComponent>();
    ModelComponent* modelComp = cube->AddComponent<ModelComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetEulerRotation(rotation);
    transform->SetScaling(scaling);
    collider->SetScale({
        scaling.x * 2.0f,
        scaling.y * 2.0f,
        scaling.z * 2.0f
        });
    modelComp->LoadModel("asset\\Model\\cube.fbx");
    modelComp->SetColor(color);
}

void Factory::CreateUiText(GameObject* uiText, DirectX::XMFLOAT3 position, const char8_t* text, float fontSize, DirectX::XMFLOAT4 color, bool isCenter)
{
    // component生成・登録
    RectTransformComponent* rectTransform = uiText->AddComponent<RectTransformComponent>();
    TextComponent* textComponent = uiText->AddComponent<TextComponent>();

    // component設定
    rectTransform->SetPosition(position);
    rectTransform->SetScaling({ 100.0f,100.0f,1.0f });

    textComponent->SetText(text);
    textComponent->SetFontSize((int)fontSize);
    textComponent->SetColor(color);
    textComponent->SetCenter(isCenter);
}

void Factory::CreateUiImageWorld(GameObject* obj, XMFLOAT3 position, XMFLOAT3 euler, XMFLOAT3 scale, const wchar_t* texturePath)
{
    // component生成・登録
    TransformComponent* transform = obj->AddComponent<TransformComponent>();
    ImageComponent* imageComp = obj->AddComponent<ImageComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetEulerRotation(euler);
    transform->SetScaling(scale);
    imageComp->Load(texturePath);
    imageComp->SetWorldSpaceType(WorldSpaceType::None);
}
