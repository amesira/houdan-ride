// ==============================================
// utility.h [ランニングマン]
// 
// 便利な関数をまとめたヘッダーファイル
// 
// 制作者:AT12D 08 牛丸隼人      日付:2025/06/24
// ==============================================
#pragma once

#include <Windows.h>
#define NOMINMAX

// ComPtrを使うためのヘッダー
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <optional>
#include <algorithm>
#include <memory>

#include <DirectXMath.h>

enum class Direction
{
	Up,
	Down,
	Right,
	Left,
};

//0~1のランダムな値を取得する
inline float GetNormRand()
{
	return (float)(rand() % 10000) / 10000.0f;
}

//線形補間を求める
inline float Lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

// イージング(イン・アウト・二次曲線)
inline float EaseInOutQuad(float x)
{
	return x < 0.5
		? 2.0f * x * x
		: 1.0f - static_cast<float>(pow(-2 * x + 2, 2)) / 2.0f;
}

inline float EaseOutSine(float x)
{
	return static_cast<float>(sin((x * DirectX::XM_PI) * 0.5f));
}

inline DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& v, float s)
{
	DirectX::XMVECTOR vec = DirectX::XMLoadFloat2(&v);
	DirectX::XMVECTOR result = DirectX::XMVectorScale(vec, s);

	DirectX::XMFLOAT2 out;
	DirectX::XMStoreFloat2(&out, result);
	return out;
}

inline DirectX::XMFLOAT2 operator+(const DirectX::XMFLOAT2& l, const DirectX::XMFLOAT2& r)
{
	return { l.x + r.x,l.y + r.y };
}

inline DirectX::XMFLOAT2& operator+=(DirectX::XMFLOAT2& l, const DirectX::XMFLOAT2& r)
{
	return l = l + r;
}
