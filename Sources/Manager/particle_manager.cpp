//===================================================
// particle_manager.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/01/06
//===================================================
#include "particle_manager.h"
#include "Sources/System/Graphic/sprite.h"

#include "Utility/mi_math.h"
#include "Sources/System/Graphic/shader.h"
#include "Sources/System/direct3d.h"

#include "Sources/GameParts/Component/transform_component.h"
#include "Sources/GameParts/Behavior/player_behavior.h"

static ID3D11ShaderResourceView* g_defaultTexture = nullptr;
static std::vector<Emitter>  g_emitters;

static ID3D11DeviceContext* g_pDeviceContext = nullptr;

static TransformComponent* g_playerTransform = nullptr;
static PlayerBehavior* g_playerBehavior = nullptr;

void ParticleM_Initialize()
{
    g_pDeviceContext = Direct3D_GetDeviceContext();
    g_playerBehavior = nullptr;
    g_playerTransform = nullptr;

    LoadTexture(&g_defaultTexture, L"asset\\Texture\\white.bmp");
}

void ParticleM_Finalize()
{
    g_emitters.clear();
}

void ParticleM_Update(float dt)
{
    for(Emitter& emitter : g_emitters) {
        for(Particle& particle : emitter.particles) {
            if(particle.enabled) {
                // ライフタイム更新
                particle.lifeTime -= dt;
                particle.accumTime += dt;
                if(particle.lifeTime <= 0.0f) {
                    particle.enabled = false;
                    continue;
                }

                // ExpEffect専用の挙動
                if(emitter.name == "ExpEffect" && g_playerTransform != nullptr) {
                    particle.lifeTime += dt; // ライフタイム減少停止
                    if(particle.accumTime > 0.8f){
                        particle.settings.velocity = { 0.0f, 0.0f, 0.0f };
                        particle.settings.gravity = { 0.0f, 0.0f, 0.0f };

                        particle.data.position = MiMath::Lerp(
                            particle.data.position,
                            g_playerTransform->GetPosition(),
                            dt * 7.0f
                        );

                        float diff = MiMath::Distance(
                            particle.data.position,
                            g_playerTransform->GetPosition()
                        );
                        if(diff < 0.5f) {
                            particle.enabled = false;
                            if(g_playerBehavior) {
                                g_playerBehavior->AddScore(50);
                            }
                        }
                    }
                }

                // 位置更新
                particle.data.position.x += particle.settings.velocity.x * dt;
                particle.data.position.y += particle.settings.velocity.y * dt;
                particle.data.position.z += particle.settings.velocity.z * dt;

                // 重力影響
                particle.settings.velocity.x += particle.settings.gravity.x * dt;
                particle.settings.velocity.y += particle.settings.gravity.y * dt;
                particle.settings.velocity.z += particle.settings.gravity.z * dt;

                // サイズフェード
                if(particle.settings.fadeSize) {
                    particle.data.scaling = MiMath::Lerp(
                        particle.data.scaling,
                        XMFLOAT3(0.0f, 0.0f, 0.0f),
                        dt * 1.0f
                    );
                }

                // アルファフェード
                if(particle.settings.fadeAlpha) {
                    particle.data.color.w *= (1.0f - dt * 1.0f);
                }
            }
        }
    }
}

void ParticleM_Draw()
{
    // ビュープロジェクション行列設定
    XMMATRIX view = Direct3D_GetViewMatrix();
    Shader_SetMatrix(view * Direct3D_GetProjectionMatrix());

    for(Emitter& emitter : g_emitters) {
        // インスタンス描画準備
        PrepareDrawInstance();

        // 有効なパーティクル分インスタンスデータ追加
        for(Particle& particle : emitter.particles) {
            if(particle.enabled) {
                XMMATRIX worldMatrix = XMMatrixScaling(
                    particle.data.scaling.x,
                    particle.data.scaling.y,
                    particle.data.scaling.z);
                worldMatrix *= XMMatrixRotationRollPitchYaw(0.0f, 0.0f, XMConvertToRadians(180.0f));
                worldMatrix *= XMMatrixScaling(-1.0f, 1.0f, 1.0f);
                
                XMMATRIX transMatrix = view;
                {
                    transMatrix.r[3].m128_f32[0] = 0.0f;
                    transMatrix.r[3].m128_f32[1] = 0.0f;
                    transMatrix.r[3].m128_f32[2] = 0.0f;
                    transMatrix.r[3].m128_f32[3] = 1.0f;
                }
                transMatrix = XMMatrixTranspose(transMatrix); // 転置行列にする
                {
                    transMatrix.r[3].m128_f32[0] = particle.data.position.x;
                    transMatrix.r[3].m128_f32[1] = particle.data.position.y;
                    transMatrix.r[3].m128_f32[2] = particle.data.position.z;
                    transMatrix.r[3].m128_f32[3] = 1.0f;
                }
                worldMatrix *= transMatrix;

                AddInstanceData(
                    worldMatrix,
                    particle.data.color,
                    particle.data.uvRect);
            }
        }

        // テクスチャ設定
        ID3D11ShaderResourceView* texture = emitter.texture ? emitter.texture : g_defaultTexture;
        g_pDeviceContext->PSSetShaderResources(0, 1, &texture);
        DrawInstance();
    }

    EndDrawInstance();
}

void ParticleM_RegisterEmitter(const std::string& emitterName, ID3D11ShaderResourceView* texture)
{
    for(Emitter& emitter : g_emitters) {
        if(emitter.name == emitterName) {
            // 既に登録されている場合は何もしない
            return;
        }
    }

    Emitter newEmitter;
    newEmitter.name = emitterName;
    newEmitter.texture = texture ? texture : g_defaultTexture;
    for(Particle& particle : newEmitter.particles) {
        particle.enabled = false;
    }
    g_emitters.push_back(newEmitter);
}

void ParticleM_SetPlayer(TransformComponent* transform, PlayerBehavior* playerBehavior)
{
    g_playerTransform = transform;
    g_playerBehavior = playerBehavior;
}


bool ParticleEmit::Emit(const std::string& emitterName, Particle::Data data, Particle::Settings settings, float lifeTime)
{
    Emitter* targetEmitter = nullptr; // 対象のエミッターを探す

    // エミッター名で既存のエミッターを検索
    for (Emitter& emitter : g_emitters) {
        if (emitter.name == emitterName) {
            targetEmitter = &emitter;
        }
    }

    if (!targetEmitter)return false;

    // 空いているパーティクルスロットを探して初期化
    for (Particle& particle : targetEmitter->particles) {
        if (!particle.enabled) {
            particle.enabled = true;
            particle.lifeTime = lifeTime;
            particle.data = data;
            particle.settings = settings;
            particle.accumTime = 0.0f;
            return true;
        }
    }

    // 空きスロットがない場合、新しいパーティクルを追加（最大数まで）
    if (targetEmitter->particles.size() < MAX_PARTICLES) {
        Particle newParticle;
        newParticle.enabled = true;
        newParticle.lifeTime = lifeTime;
        newParticle.data = data;
        newParticle.settings = settings;
        newParticle.accumTime = 0.0f;
        targetEmitter->particles.push_back(newParticle);
        return true;
    }

    return false;
}

bool ParticleEmit::EmitExplosion(
    const std::string& emitterName, 
    Particle::Data baseData,
    Particle::Settings baseSettings, 
    float lifeTime, int particleCount)
{
    // 黄金角の計算
    static const float phiAngle = XM_2PI / ((1.0f + sqrt(5.0f)) / 2.0f);

    // velocityの大きさを計算
    float baseSpeed = MiMath::Length(baseSettings.velocity);

    // 指定された数のパーティクルを発生させる
    for(int i = 0; i < particleCount; i++) {
        Particle::Settings settings = baseSettings;

        // 速度方向を黄金角スパイラルで分布させる
        float y = 1.0f - (i / float(particleCount - 1)) * 2.0f;
        float r = sqrt(1.0f * 1.0f - y * y);
        float theta = phiAngle * i;
        float x = cos(theta) * r;
        float z = sin(theta) * r;
        XMFLOAT3 dir = { x, y, z };
        dir = MiMath::Normalize(dir);

        // 速度ベクトルを設定
        settings.velocity = {
            dir.x * baseSpeed,
            dir.y * baseSpeed,
            dir.z * baseSpeed
        };

        // パーティクル発生
        bool h = ParticleEmit::Emit(
            emitterName,
            baseData,
            settings,
            lifeTime
        );
        if(!h)return false;
    }

    return true;
}
