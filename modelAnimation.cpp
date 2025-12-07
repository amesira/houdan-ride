#include "modelAnimation.h"

#include <DirectXMath.h>
using namespace DirectX;

XMVECTOR ModelAnimation::SamplePosition(const std::vector<Channel::VectorKey>& keys, double animationTime) const
{
    // 現在のフレームインデックスを計算
    // (例: 1.3 / 0.4 = 3.25 -> 3)
    int frameIndex = static_cast<int>(floor(animationTime / m_ticksPerFrame));

    // 配列の範囲内に収める (ループ対応)
    if (frameIndex >= static_cast<int>(keys.size()))
        frameIndex = static_cast<int>(keys.size()) - 1; // (または frameIndex % keys.size() )
    if (frameIndex < 0)
        frameIndex = 0;

    // フレームの値を「補間せず」そのまま返す！
    return XMLoadFloat3(&keys[frameIndex].value);
}

XMVECTOR ModelAnimation::SampleRotation(
    const std::vector<Channel::QuaternionKey>& keys, double animationTime) const
{
    int frameIndex = static_cast<int>(floor(animationTime / m_ticksPerFrame));
    if (frameIndex >= static_cast<int>(keys.size()))
        frameIndex = static_cast<int>(keys.size()) - 1;
    if (frameIndex < 0)
        frameIndex = 0;

    return XMLoadFloat4(&keys[frameIndex].value);
}

XMVECTOR ModelAnimation::SampleScaling(const std::vector<Channel::VectorKey>& keys, double animationTime) const
{
    // 1. 現在のフレームインデックスを計算
    // (例: 1.3 / 0.4 = 3.25 -> 3)
    int frameIndex = static_cast<int>(floor(animationTime / m_ticksPerFrame));

    // 2. 配列の範囲内に収める (ループ対応)
    if (frameIndex >= static_cast<int>(static_cast<int>(keys.size())))
        frameIndex = static_cast<int>(static_cast<int>(keys.size())) - 1; // (または frameIndex % keys.size() )
    if (frameIndex < 0)
        frameIndex = 0;

    // 3. ★そのフレームの値を「補間せず」そのまま返す！★
    return XMLoadFloat3(&keys[frameIndex].value);
}

