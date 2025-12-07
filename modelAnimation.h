#pragma once

#include <vector>
#include <string>
#include <DirectXMath.h>
using namespace DirectX;

class ModelAnimation
{
public:

	struct Channel
	{
		struct VectorKey
		{
			double time; // このキーの時間 (秒)
			XMFLOAT3 value;
		};

		// 1つのキーフレーム (例: 回転キー)
		struct QuaternionKey
		{
			double time; // このキーの時間 (秒)
			XMFLOAT4 value; // クォータニオン
		};

		std::string nodeName;
		std::vector<VectorKey> positionKeys;
		std::vector<QuaternionKey> rotationKeys;
		std::vector<VectorKey> scalingKeys;
	};

	XMVECTOR SamplePosition(const std::vector<Channel::VectorKey>& keys, double animationTime) const;
	XMVECTOR SampleRotation(const std::vector<Channel::QuaternionKey>& keys, double animationTime) const;
	XMVECTOR SampleScaling(const std::vector<Channel::VectorKey>& keys, double animationTime) const;

	float m_ticksPerFrame;
	float m_animationDuration;

	// モーションデータのチャンネル
	std::vector<Channel> channels;

	// ノードインデックス->チャンネルインデックスのマップ
	std::vector<int> channelNodeIndices;

};