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
#include "slider_component.h"

// behavior
#include "player_behavior.h"
#include "tps_camera_behavior.h"
#include "enemy_behavior.h"
#include "switch_sprite_behavior.h"
#include "ball_behavior.h"
#include "woodbox_behavior.h"
#include "train_behavior.h"
#include "pointer_behavior.h"
#include "liftup_behavior.h"

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

void Factory::CreateMapCamera(GameObject* obj, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 atPosition)
{
    obj->SetName("MapCamera");

    // component生成・登録
    TransformComponent* transform = obj->AddComponent<TransformComponent>();
    CameraComponent* camera = obj->AddComponent<CameraComponent>();

    // component設定
    transform->SetPosition(position);

    camera->SetAtPosition(atPosition);
    camera->SetFov(60.0f);
    camera->SetAspect(9.0f/ 16.0f);
    camera->SetNearClip(0.1f);
    camera->SetFarClip(100.0f);
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
    collider->SetLayer(ColliderComponent::Layer::Player);
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
    transform->SetScaling({ 3.0f, 3.0f, 3.0f });
    collider->SetLayer(ColliderComponent::Layer::Ball);
    collider->SetRadius(1.0f);
    rigidbody->SetMass(1.5f);
    rigidbody->SetFriction({ 0.98f, 1.0f, 0.98f });

    modelComp->LoadModel("asset\\Model\\cannon_ball.fbx");

    // behavior生成・登録
    BallBehavior* ballBe = obj->AddBehavior<BallBehavior>();
}

void Factory::CreateWoodbox(GameObject* obj, DirectX::XMFLOAT3 position)
{
    obj->SetName("Woodbox");

    // component生成・登録
    TransformComponent* transform = obj->AddComponent<TransformComponent>();
    RigidbodyComponent* rigidbody = obj->AddComponent<RigidbodyComponent>();
    BoxColliderComponent* collider = obj->AddComponent<BoxColliderComponent>();
    ModelComponent* modelComp = obj->AddComponent<ModelComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetScaling({ 2.0f,2.0f,2.0f });
    collider->SetCenter({ 0.0f, 1.0f, 0.0f });
    collider->SetScale({ 2.0f,2.4f,2.0f });
    modelComp->LoadModel("asset\\Model\\barrel.fbx");
    modelComp->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

    // behavior生成・登録
    WoodboxBehavior* woodboxBe = obj->AddBehavior<WoodboxBehavior>();
}

void Factory::CreateEnemy(GameObject* enemy, DirectX::XMFLOAT3 position)
{
    enemy->SetName("Enemy");

    // component生成・登録
    TransformComponent* transform = enemy->AddComponent<TransformComponent>();
    SphereColliderComponent* collider = enemy->AddComponent<SphereColliderComponent>();
    RigidbodyComponent* rigidbody = enemy->AddComponent<RigidbodyComponent>();
    ImageComponent* imageComp = enemy->AddComponent<ImageComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetScaling({ 1.0f,1.0f,1.0f });
    collider->SetLayer(ColliderComponent::Layer::Enemy);
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

    // behavior生成・登録
    TrainBehavior* trainBe = cube->AddBehavior<TrainBehavior>();

}

void Factory::CreateTrain(GameObject* obj, DirectX::XMFLOAT3 position)
{
    obj->SetName("Train");

    TransformComponent* transform = obj->AddComponent<TransformComponent>();
    ModelComponent* modelComp = obj->AddComponent<ModelComponent>();
    BoxColliderComponent* collider = obj->AddComponent<BoxColliderComponent>();
    BoxColliderComponent* collider2 = obj->AddComponent<BoxColliderComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetEulerRotation({ 0.0f, XMConvertToRadians(180.0f), 0.0f });
    transform->SetScaling({2.0f, 2.0f, 2.0f});
    modelComp->LoadModel("asset\\Model\\ship.fbx");

    collider->SetCenter({ 0.0f, 4.0f, -0.35f });
    collider->SetScale({ 9.6f, 1.0f, 7.4f });

    // behavior生成・登録
    TrainBehavior* trainBe = obj->AddBehavior<TrainBehavior>();
}

void Factory::CreateDodai(GameObject* obj, DirectX::XMFLOAT3 position)
{
    obj->SetName("Dodai");

    // component生成・登録
    TransformComponent* transform = obj->AddComponent<TransformComponent>();
    ModelComponent* modelComp = obj->AddComponent<ModelComponent>();
    BoxColliderComponent* collider = obj->AddComponent<BoxColliderComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetScaling({ 2.0f,3.0f,2.0f });
    modelComp->LoadModel("asset\\Model\\dodai.fbx");
    collider->SetCenter({ 0.0f, 3.0f, 0.0f });
    collider->SetScale({ 4.0f, 7.0f, 4.0f });
    collider->SetLayer(ColliderComponent::Layer::Field);

    LiftupBehavior* liftBe = obj->AddBehavior<LiftupBehavior>();
}

void Factory::CreateBoxCollider(GameObject* obj, XMFLOAT3 position, XMFLOAT3 rotation, DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 size)
{
    // component生成・登録
    TransformComponent* transform = obj->AddComponent<TransformComponent>();
    BoxColliderComponent* boxCollider = obj->AddComponent<BoxColliderComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetEulerRotation(rotation);

    boxCollider->SetCenter(center);
    boxCollider->SetScale(size);
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

void Factory::CreateUiSlider(GameObject* obj, XMFLOAT3 position,float rotation, XMFLOAT2 size)
{
    // component生成・登録
    RectTransformComponent* rectTransform = obj->AddComponent<RectTransformComponent>();
    SliderComponent* sliderComp = obj->AddComponent<SliderComponent>();

    // component設定
    rectTransform->SetPosition(position);
    rectTransform->SetRotation({ 0.0f, 0.0f, rotation });
    rectTransform->SetScaling({ size.x, size.y, 1.0f });
    sliderComp->SetValue(0.3f);
    sliderComp->SetBgColor({ 1.0f,1.0f,1.0f,1.0f });
    sliderComp->SetFillColor({ 0.0f,0.5f,1.0f,1.0f });
}

void Factory::CreateUiImage(GameObject* obj, XMFLOAT3 position, float rotation, XMFLOAT2 size, const wchar_t* texturePath)
{
    // component生成・登録
    RectTransformComponent* rectTransform = obj->AddComponent<RectTransformComponent>();
    ImageComponent* imageComp = obj->AddComponent<ImageComponent>();

    // component設定
    rectTransform->SetPosition(position);
    rectTransform->SetRotation({ 0.0f, 0.0f, rotation });
    rectTransform->SetScaling({ size.x, size.y, 1.0f });
    imageComp->Load(texturePath);
    imageComp->SetWorldSpaceType(WorldSpaceType::None);
}

void Factory::CreatePointer(GameObject* obj) {
    obj->SetName("Pointer");

    // component生成・登録
    TransformComponent* transform = obj->AddComponent<TransformComponent>();
    ImageComponent* imageComp = obj->AddComponent<ImageComponent>();

    // component設定
    transform->SetPosition({ 0.0f, 0.0f, 0.0f });
    transform->SetScaling({ 3.0f, 3.0f, 1.0f });
    imageComp->Load(L"asset\\Texture\\magic_03.png");
    imageComp->SetWorldSpaceType(WorldSpaceType::Billboard);

    // behavior生成・登録
    PointerBehavior* pointerBe = obj->AddBehavior<PointerBehavior>();
}