//===================================================
// renderer_3dmodel_processor.cpp
// 
// Author：Hayato Ushimaru
// Date  ：2025/11/17
//===================================================
#include "renderer_3dmodel_processor.h"

#include <fstream>
#include "debug_ostream.h"

// directX
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
using namespace DirectX;

// component
#include "transform_component.h"
#include "model_component.h"

void Renderer3DModelProcessor::Initialize()
{
	m_pDevice = Direct3D_GetDevice();
	m_pContext = Direct3D_GetDeviceContext();

    // 3Dモデル用シェーダーの読み込み
	{
		HRESULT hr; // 戻り値格納用

		// ボーンモデル用頂点シェーダーの読み込み
		{
			std::ifstream ifs_vs("shader_vertex_3dmodel_skinning.cso", std::ios::binary);

			if (!ifs_vs) {
				MessageBox(nullptr, "頂点シェーダーの読み込みに失敗しました\n\nshader_vertex_3dmodel_skinning.cso", "エラー", MB_OK);
				return;
			}

			ifs_vs.seekg(0, std::ios::end); // ファイルポインタを末尾に移動
			std::streamsize filesize = ifs_vs.tellg(); // ファイルポインタの位置を取得（つまりファイルサイズ）
			ifs_vs.seekg(0, std::ios::beg); // ファイルポインタを先頭に戻す

			// バイナリデータを格納するためのバッファを確保
			unsigned char* vsbinary_pointer = new unsigned char[filesize];

			ifs_vs.read((char*)vsbinary_pointer, filesize); // バイナリデータを読み込む
			ifs_vs.close(); // ファイルを閉じる

			// 頂点シェーダーの作成
			hr = m_pDevice->CreateVertexShader(vsbinary_pointer, filesize, nullptr, &m_pSkinVertexShader);

			if (FAILED(hr)) {
				hal::dout << "renderer_3dmodel_processor Initialize() : 頂点シェーダーの作成に失敗しました" << std::endl;
				delete[] vsbinary_pointer; // メモリリークしないようにバイナリデータのバッファを解放
				return;
			}

			// 頂点レイアウトの定義
			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				// 頂点ごとのデータ
				{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BONEINDICES",	0, DXGI_FORMAT_R32G32B32A32_SINT,	0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BONEWEIGHTS",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },

				// インスタンスごとのワールド変換行列
				{ "INSTANCE_WORLD_ROW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				{ "INSTANCE_WORLD_ROW", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				{ "INSTANCE_WORLD_ROW", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				{ "INSTANCE_WORLD_ROW", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
			};

			UINT num_elements = ARRAYSIZE(layout); // 配列の要素数を取得

			// 頂点レイアウトの作成
			hr = m_pDevice->CreateInputLayout(layout, num_elements, vsbinary_pointer, filesize, &m_pSkinInputLayout);

			delete[] vsbinary_pointer; // バイナリデータのバッファを解放

			if (FAILED(hr)) {
				hal::dout << "renderer_3dmodel_processor Initialize() : 頂点レイアウトの作成に失敗しました" << std::endl;
				return;
			}
		}

		// 静的モデル用頂点シェーダーの読み込み
		{
			std::ifstream ifs_vs("shader_vertex_3dmodel_static.cso", std::ios::binary);

			if (!ifs_vs) {
				MessageBox(nullptr, "頂点シェーダーの読み込みに失敗しました\n\nshader_vertex_3dmodel_static.cso", "エラー", MB_OK);
				return;
			}

			ifs_vs.seekg(0, std::ios::end); // ファイルポインタを末尾に移動
			std::streamsize filesize = ifs_vs.tellg(); // ファイルポインタの位置を取得（つまりファイルサイズ）
			ifs_vs.seekg(0, std::ios::beg); // ファイルポインタを先頭に戻す

			// バイナリデータを格納するためのバッファを確保
			unsigned char* vsbinary_pointer = new unsigned char[filesize];

			ifs_vs.read((char*)vsbinary_pointer, filesize); // バイナリデータを読み込む
			ifs_vs.close(); // ファイルを閉じる

			// 頂点シェーダーの作成
			hr = m_pDevice->CreateVertexShader(vsbinary_pointer, filesize, nullptr, &m_pStaticVertexShader);

			if (FAILED(hr)) {
				hal::dout << "renderer_3dmodel_processor Initialize() : 頂点シェーダーの作成に失敗しました" << std::endl;
				delete[] vsbinary_pointer; // メモリリークしないようにバイナリデータのバッファを解放
				return;
			}

			// 頂点レイアウトの定義
			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				// 頂点ごとのデータ
				{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },

				// インスタンスごとのワールド変換行列
				{ "INSTANCE_WORLD_ROW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				{ "INSTANCE_WORLD_ROW", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				{ "INSTANCE_WORLD_ROW", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				{ "INSTANCE_WORLD_ROW", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
			};

			UINT num_elements = ARRAYSIZE(layout); // 配列の要素数を取得

			// 頂点レイアウトの作成
			hr = m_pDevice->CreateInputLayout(layout, num_elements, vsbinary_pointer, filesize, &m_pStaticInputLayout);

			delete[] vsbinary_pointer; // バイナリデータのバッファを解放

			if (FAILED(hr)) {
				hal::dout << "renderer_3dmodel_processor Initialize() : 頂点レイアウトの作成に失敗しました" << std::endl;
				return;
			}
		}

		// ピクセルシェーダーの読み込み
		{
			std::ifstream ifs_ps("shader_pixel_3dmodel.cso", std::ios::binary);
			if (!ifs_ps) {
				MessageBox(nullptr, "ピクセルシェーダーの読み込みに失敗しました\nhader_pixel_3dmodel.cso", "エラー", MB_OK);
				return;
			}

			ifs_ps.seekg(0, std::ios::end);
			std::streamsize filesize = ifs_ps.tellg();
			ifs_ps.seekg(0, std::ios::beg);

			unsigned char* psbinary_pointer = new unsigned char[filesize];
			ifs_ps.read((char*)psbinary_pointer, filesize);
			ifs_ps.close();

			// ピクセルシェーダーの作成
			hr = m_pDevice->CreatePixelShader(psbinary_pointer, filesize, nullptr, &m_pPixelShader);

			delete[] psbinary_pointer; // バイナリデータのバッファを解放

			if (FAILED(hr)) {
				hal::dout << "renderer_3dmodel_processor Initialize() : ピクセルシェーダーの作成に失敗しました" << std::endl;
				return;
			}
		}
	}

	// インスタンスデータ用頂点バッファの作成
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(InstanceData) * MaxInstanceCount;
		desc.Usage = D3D11_USAGE_DYNAMIC; 
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_pDevice->CreateBuffer(&desc, nullptr, &m_pInstanceVertexBuffer);
	}

	// 投影変換用定数バッファの作成
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(XMMATRIX);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_pDevice->CreateBuffer(&desc, nullptr, &m_pProjectionCB);
	}

	// ビュー用定数バッファの作成
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(XMMATRIX);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_pDevice->CreateBuffer(&desc, nullptr, &m_pViewCB);
	}

	// ボーン行列配列用定数バッファの作成
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(XMMATRIX) * 256;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_pDevice->CreateBuffer(&desc, nullptr, &m_pBoneMatricesCB);
	}

}

void Renderer3DModelProcessor::Finalize()
{
	// 全モデルのテクスチャ解放
	m_textureCache.clear();

	SAFE_RELEASE(m_pInstanceVertexBuffer);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pSkinInputLayout);
	SAFE_RELEASE(m_pSkinVertexShader);
	SAFE_RELEASE(m_pStaticInputLayout);
	SAFE_RELEASE(m_pStaticVertexShader);
}

void Renderer3DModelProcessor::Process()
{
	// シェーダーのバインド
	m_pContext->PSSetShader(m_pPixelShader, nullptr, 0);

	// インスタンスデータ用頂点バッファをバインド
	UINT stride = sizeof(InstanceData);
	UINT offset = 0;
	m_pContext->IASetVertexBuffers(1, 1, &m_pInstanceVertexBuffer, &stride, &offset);

	// 投影変換行列を更新
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		m_pContext->Map(m_pProjectionCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		XMMATRIX matrix = Direct3D_GetProjectionMatrix();
		matrix = XMMatrixTranspose(matrix);
		memcpy(mappedResource.pData, &matrix, sizeof(XMMATRIX));
		
		m_pContext->Unmap(m_pProjectionCB, 0);
		// 定数バッファをバインド
		m_pContext->VSSetConstantBuffers(0, 1, &m_pProjectionCB);
	}

	// ビュー変換行列を更新
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		m_pContext->Map(m_pViewCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		
		XMMATRIX matrix = Direct3D_GetViewMatrix();
		matrix = XMMatrixTranspose(matrix);
		memcpy(mappedResource.pData, &matrix, sizeof(XMMATRIX));

		m_pContext->Unmap(m_pViewCB, 0);
		// 定数バッファをバインド
		m_pContext->VSSetConstantBuffers(1, 1, &m_pViewCB);
	}

	// ボーン行列を仮で単位行列で初期化
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		m_pContext->Map(m_pBoneMatricesCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		XMMATRIX* pBoneMatrices = (XMMATRIX*)mappedResource.pData;
		for (int i = 0; i < 256; ++i) {
			pBoneMatrices[i] = XMMatrixIdentity();
		}
		m_pContext->Unmap(m_pBoneMatricesCB, 0);
		// 定数バッファをバインド
		m_pContext->VSSetConstantBuffers(2, 1, &m_pBoneMatricesCB);
	}

	// インスタンスデータマップをクリア
	for (auto& pair : m_RendererSkinBatches)
		pair.second.clear(); 
	
	for (auto& pair : m_RendererStaticBatches)
		pair.second.clear();

	// 全インスタンスでループ
	for (auto& comp : m_components)
	{
		// ワールド変換行列を計算
		XMFLOAT3 position = comp.m_transform->GetPosition();
		XMFLOAT3 rotation = comp.m_transform->GetRotation();
		XMFLOAT3 scaling = comp.m_transform->GetScaling();

		// ワールド行列（乗算の順番に注意）
		XMMATRIX    worldMatrix =
			XMMatrixScaling(scaling.x,scaling.y,scaling.z) *
			XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z)) *
			XMMatrixTranslation(position.x, position.y, position.z);
		
		// モデルごとのマップにインスタンスデータを追加

		// ボーンがあるかどうかでマップを切り替え
		if (comp.m_modelComp->GetModelData()->HasBones())
			m_RendererSkinBatches[comp.m_modelComp->GetModelData()].push_back({worldMatrix });
		else
			m_RendererStaticBatches[comp.m_modelComp->GetModelData()].push_back({ worldMatrix });

	}

	// ボーンなしモデルの描画
	{
		m_pContext->VSSetShader(m_pStaticVertexShader, nullptr, 0);
		m_pContext->IASetInputLayout(m_pStaticInputLayout);

		// 各モデルごとに描画
		for (auto& batch : m_RendererStaticBatches)
		{
			ModelData* modelData = batch.first;
			auto& instanceData = batch.second;

			if (modelData == nullptr || modelData->IsValid() == false) continue;

			if (instanceData.empty()) continue;

			// このモデルのインスタンスデータを頂点バッファへ転送
			{
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				m_pContext->Map(m_pInstanceVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
				InstanceData* pInstanceData = (InstanceData*)mappedResource.pData;
				CopyMemory(pInstanceData, instanceData.data(), sizeof(InstanceData) * instanceData.size());
				m_pContext->Unmap(m_pInstanceVertexBuffer, 0);
			}

			// トポロジの設定
			m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// 全メッシュの描画
			auto& meshes = modelData->GetMeshes();

			for (const auto& mesh : meshes)
			{
				const ModelData::Material& meshMaterial = modelData->GetMaterials()[mesh.materialIndex];

				// ベースカラーテクスチャのバインド
				m_pContext->PSSetShaderResources(0, 1, meshMaterial.baseColorSRV.GetAddressOf());

				// エミッションカラーテクスチャのバインド
				m_pContext->PSSetShaderResources(1, 1, meshMaterial.emissionColorSRV.GetAddressOf());

				// マテリアル用定数バッファのバインド
				m_pContext->PSSetConstantBuffers(3, 1, meshMaterial.buffer.GetAddressOf());

				// 頂点バッファを描画パイプラインに設定
				UINT stride = sizeof(ModelData::Vertex);
				UINT offset = 0;
				m_pContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);

				// インデックスバッファを描画パイプラインに設定
				m_pContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

				// 描画命令
				m_pContext->DrawIndexedInstanced(mesh.numIndices, instanceData.size(), 0, 0, 0);
			}
		}
	}

	// ボーンありモデルの描画
	{
		m_pContext->VSSetShader(m_pSkinVertexShader, nullptr, 0);
		m_pContext->IASetInputLayout(m_pSkinInputLayout);

		// 各モデルごとに描画
		for (auto& batch : m_RendererSkinBatches)
		{
			ModelData* modelData = batch.first;
			auto& instanceData = batch.second;

			if (modelData == nullptr || modelData->IsValid() == false) continue;

			if (instanceData.empty()) continue;

			// このモデルのインスタンスデータを頂点バッファへ転送
			{
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				m_pContext->Map(m_pInstanceVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
				InstanceData* pInstanceData = (InstanceData*)mappedResource.pData;
				CopyMemory(pInstanceData, instanceData.data(), sizeof(InstanceData) * instanceData.size());
				m_pContext->Unmap(m_pInstanceVertexBuffer, 0);
			}

			// トポロジの設定
			m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// 全メッシュの描画
			auto& meshes = modelData->GetMeshes();

			for (const auto& mesh : meshes)
			{
				const ModelData::Material& meshMaterial = modelData->GetMaterials()[mesh.materialIndex];

				// ベースカラーテクスチャのバインド
				m_pContext->PSSetShaderResources(0, 1, meshMaterial.baseColorSRV.GetAddressOf());

				// エミッションカラーテクスチャのバインド
				m_pContext->PSSetShaderResources(1, 1, meshMaterial.emissionColorSRV.GetAddressOf());

				// マテリアル用定数バッファのバインド
				m_pContext->PSSetConstantBuffers(3, 1, meshMaterial.buffer.GetAddressOf());

				// 頂点バッファを描画パイプラインに設定
				UINT stride = sizeof(ModelData::Vertex);
				UINT offset = 0;
				m_pContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);

				// インデックスバッファを描画パイプラインに設定
				m_pContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

				// 描画命令
				m_pContext->DrawIndexedInstanced(mesh.numIndices, instanceData.size(), 0, 0, 0);
			}
		}

	}

}

void Renderer3DModelProcessor::Entry(TransformComponent* transform, ModelComponent* modelComponent)
{
	m_components.push_back({ transform,modelComponent });
}

ComPtr<ID3D11ShaderResourceView> Renderer3DModelProcessor::LoadModelTexture(const std::wstring& filePath)
{
	// すでにロード済みならキャッシュを返す
	auto it = m_textureCache.find(filePath);
	if (it != m_textureCache.end()) {
		return it->second;
	}

	// 新しくロード
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(filePath.c_str(), WIC_FLAGS_NONE, &metadata, image);

	ComPtr<ID3D11ShaderResourceView> texture;
	CreateShaderResourceView(m_pDevice,
		image.GetImages(),
		image.GetImageCount(),
		metadata,
		texture.GetAddressOf());

	// キャッシュに保存して返す
	m_textureCache[filePath] = texture;
	return texture;
}

ModelData* Renderer3DModelProcessor::LoadModel(const std::string& modelPath)
{
	// すでに読み込まれている場合は何もしない
	auto it = m_modelList.find(modelPath);

	if (it != m_modelList.end())
		return &it->second;

	// モデルデータを読み込む
 	auto result = m_modelList.try_emplace(modelPath, modelPath.c_str());
	return &result.first->second;
}

ModelData* Renderer3DModelProcessor::GetModelData(const std::string& modelPath)
{
	auto it = m_modelList.find(modelPath);

	if(it == m_modelList.end())
		return LoadModel(modelPath);
	else
		return &it->second;
}

