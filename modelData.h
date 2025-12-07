#pragma once

#include "assimp/Importer.hpp"      // Assimpのインポータ
#include "assimp/scene.h"           // aiScene などのデータ構造
#pragma comment (lib, "assimp-vc143-mt.lib")

#include "direct3d.h"
#include <map>
#include <unordered_map>

#include "modelAnimation.h"

// Comオブジェクト用のスマートポインタ
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class ModelData
{
public:

	ModelData() = delete;
	ModelData(const char* modelPath)
	{
		Initialize(modelPath);
	}

	// マテリアル構造体
	struct Material
	{
		ComPtr<ID3D11ShaderResourceView> baseColorSRV = nullptr;
		ComPtr<ID3D11ShaderResourceView> emissionColorSRV = nullptr;

		ComPtr<ID3D11Buffer> buffer = nullptr;

		struct BufferData
		{
			XMFLOAT4 baseColorFactor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		} bufferData;

	};

	struct Mesh
	{
		ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
		ComPtr<ID3D11Buffer> indexBuffer = nullptr;

		UINT numIndices = 0;
		UINT materialIndex = 0;
	};

	// ノード構造体
	struct Node
	{
		int index; // 自身のindex
		std::string m_name;

		// 毎フレーム更新する計算結果
		XMMATRIX m_globalTransform;

		// Tポーズ（初期姿勢）のデータ
		DirectX::XMVECTOR m_tposeScale;
		DirectX::XMVECTOR m_tposeRotation;
		DirectX::XMVECTOR m_tposePosition;

		// ツリー構造 (インデックス)
		int m_parentIndex = -1;
		std::vector<int> m_childrenIndices;
	};

	// 頂点構造体
	struct Vertex
	{
		DirectX::XMFLOAT3 position; // 座標
		DirectX::XMFLOAT3 normal;   // 法線
		DirectX::XMFLOAT2 uv;       // UV座標

		// ボーン情報
		int   boneIndices[4]; // 影響を受けるボーンのID (最大4つ)
		float boneWeights[4]; // 影響の強さ (ウェイト)

	};

	// モデルデータが有効かどうかを取得します
	//
	// 戻り値 : 有効ならtrue、無効ならfalseを返します。
	bool IsValid() const { return m_isValidate; }

	// モデルデータがボーンを持っているかどうかを取得します
	// 
	// 戻り値 : ボーンを持っていればtrue、持っていなければfalseを返します。
	bool HasBones() const { return m_boneInverseMatrices.size() != 0; }

	// モデルデータを初期化します
	//
	// modelPath : モデルファイルのパス
	void Initialize(const char* modelPath);

	// アニメーションを読み込みます
	// 
	// animationName :	キーとなる名前です。自由に設定してください。すでに存在する名前を指定した場合は何もしません。
	// path : アニメーションファイルのパスです。
	void LoadAnimation(const char* animationName, const char* path);

	// アニメーションマップを取得します
	//
	// LoadMotionで読み込んだアニメーションを参照できます。
	// 戻り値 : アニメーション名->アニメーションデータのマップへの参照を返します。
	const std::map<std::string, ModelAnimation>& GetAnimationMap() const { return m_animations; }

	// テクスチャリストを取得します
	const std::unordered_map<std::string, ComPtr<ID3D11ShaderResourceView>>& GetTextures() const { return m_textures; }

	// マテリアルリストを取得します
	const std::vector<Material>& GetMaterials() const { return m_materials; }

	// メッシュリストを取得します
	// 
	// モデルインスタンスの描画時に利用します。
	const std::vector<Mesh>& GetMeshes() const { return m_meshes; }

	// ノードリストを取得します
	const std::vector<Node>& GetNodes() const { return m_nodes; }

	// 各ボーンの「初期姿勢の逆行列 (Inverse Bind Pose Matrix)」を取得します
	const std::vector<DirectX::XMMATRIX>& GetBoneInverseMatrices() const { return m_boneInverseMatrices; }

	// 各ボーンのNodeリストのインデックス配列を取得します
	const std::vector<int>& GetBoneNodeIndices() const { return m_boneNodeIndices; }

private:
	
	bool m_isValidate = false;

	int BuildNodeTree(aiNode* assimpNode, int parentIndex, std::map<std::string, int>& nodeMap);

	std::string GetCoreNodeName(const std::string& fullChannelName);

	// テクスチャリスト
	std::unordered_map<std::string, ComPtr<ID3D11ShaderResourceView>> m_textures;

	// マテリアルリスト
	std::vector<Material> m_materials;

	// メッシュリスト
	std::vector<Mesh> m_meshes;

	// ノードリスト(ツリー構造)
	std::vector<Node> m_nodes;

	// ノード名->ノードインデックスのマップ(テーブル)
	std::map<std::string, int> m_nodeIndicesMap;
	
	// 各ボーンの「初期姿勢の逆行列 (Inverse Bind Pose Matrix)」を保存する配列
	std::vector<DirectX::XMMATRIX> m_boneInverseMatrices;

	// 各ボーンのNodeリストのインデックス配列
	std::vector<int> m_boneNodeIndices;

	// このモデルのアニメーション群
	std::map<std::string, ModelAnimation> m_animations;
	
};