//===================================================
// renderer_3dmodel_processor.h
// 
// Author：Hayato Ushimaru
// Date  ：2025/11/17
//===================================================
#ifndef RENDERER_3DMODEL_PROCESSOR_H
#define RENDERER_3DMODEL_PROCESSOR_H

#include <vector>
#include "processor.h"

// directX
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
using namespace DirectX;

#include <unordered_map>
#include <string>
#include "modelData.h"

// Comオブジェクト用のスマートポインタ
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class TransformComponent;
class ModelComponent;

class Renderer3DModelProcessor : public Processor {
private:
   
	// インスタンスデータ構造体
    struct InstanceData {
		DirectX::XMMATRIX worldMatrix; // ワールド行列
    };

    ID3D11Device* m_pDevice = nullptr; 
	ID3D11DeviceContext* m_pContext = nullptr;

    // 3Dモデル描画用シェーダー
    ID3D11VertexShader* m_pSkinVertexShader = nullptr;
	ID3D11InputLayout* m_pSkinInputLayout = nullptr;
    ID3D11VertexShader* m_pStaticVertexShader = nullptr;
	ID3D11InputLayout* m_pStaticInputLayout = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;	// ピクセルシェーダー
    ID3D11Buffer* m_pProjectionCB = nullptr;        // 投影変換行列
	ID3D11Buffer* m_pViewCB = nullptr;              // ビュー変換行列
	ID3D11Buffer* m_pBoneMatricesCB = nullptr;      // ボーン行列配列用定数バッファ

    // インスタンスデータ用頂点バッファ
    ID3D11Buffer* m_pInstanceVertexBuffer = nullptr;

	// モデル用一時テクスチャのキャッシュ
    std::unordered_map<std::wstring, ComPtr<ID3D11ShaderResourceView>> m_textureCache;

    // 全モデルのリスト
    std::unordered_map<std::string, ModelData> m_modelList;

    // バッチ処理用の全インスタンスのデータリスト
    std::map<ModelData*, std::vector<InstanceData>> m_RendererSkinBatches; // ボーンあり
    std::map<ModelData*, std::vector<InstanceData>> m_RendererStaticBatches; // ボーンなし


    struct Components {
        TransformComponent* m_transform;
        ModelComponent* m_modelComp;
    };

    std::vector<Components> m_components;

public:

	static constexpr UINT MaxInstanceCount = 1000; // 1つのモデルにおける最大インスタンス数

    void    Initialize() override;
    void    Finalize() override;

    void    Process() override;

    void    Entry
    (
        TransformComponent* transform,
        ModelComponent* modelComponent
    );

	// モデルテクスチャ読み込み（キャッシュ付き）
    // 
	// modelDataクラス内で、モデル読み込み時にテクスチャを読み込む際に使用されます。
	// すでに同じパスのテクスチャが読み込まれている場合はキャッシュから返します。
    ComPtr<ID3D11ShaderResourceView> LoadModelTexture(const std::wstring& filePath);

    // モデルを読み込みます。
    //
	// モデルを読み込み、モデルリストに追加します。既に同じ名前のモデルが読み込まれているなら何もしません。
    // パスが無効の場合は何もしません
	// modelPath : モデルファイルのパス
	// 戻り値： 読み込んだモデルデータへのポインタを返します。
    ModelData* LoadModel(const std::string& modelPath);

    // モデルデータを取得します。
    //
	// modelName : モデル名（LoadModelで指定したパス）
	// もしモデルが見つからなかった場合は自動的にモデルを読み込みます。
	// (モデルの読み込みに失敗した場合はnullptrを返します。)
	// 戻り値： モデルデータへのポインタを返します。
    ModelData* GetModelData(const std::string& modelPath);
};

#endif