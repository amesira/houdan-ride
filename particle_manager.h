//---------------------------------------------------
// particle_manager.h
// 
// Author：Miu Kitamura
// Date  ：2026/01/06
//---------------------------------------------------
#ifndef PARTICLE_MANAGER_H
#define PARTICLE_MANAGER_H

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include <string>
#include <vector>

#define MAX_PARTICLES (1024)

// パーティクル構造体
struct Particle {
    struct Data { // パーティクルデータ
        XMFLOAT3    position;
        XMFLOAT3    scaling;
        XMFLOAT4    color;
        XMFLOAT4    uvRect;
    };
    struct Settings { // パーティクル設定
        XMFLOAT3    velocity;
        XMFLOAT3    gravity;
        bool        fadeSize;
        bool        fadeAlpha;
    };
    bool        enabled;
    float       lifeTime;
    Data        data;
    Settings    settings;
};

// エミッター構造体
struct Emitter {
    std::string                 name;
    ID3D11ShaderResourceView*   texture;
    std::vector<Particle>       particles = std::vector<Particle>(MAX_PARTICLES);
};

void ParticleM_Initialize();
void ParticleM_Finalize();

void ParticleM_Update(float dt);
void ParticleM_Draw();

void ParticleM_RegisterEmitter(
    const std::string& emitterName,
    ID3D11ShaderResourceView* texture = nullptr
);

// パーティクル発生用名前空間
namespace ParticleEmit {
    // 単一パーティクルを発生させる
    bool Emit(
        const std::string& emitterName,
        Particle::Data data,
        Particle::Settings settings,
        float lifeTime
    );
    // 爆発エフェクトを発生させる
    bool EmitExplosion(
        const std::string& emitterName,
        Particle::Data baseData,
        Particle::Settings baseSettings,
        float lifeTime,
        int particleCount
    );
}

#endif