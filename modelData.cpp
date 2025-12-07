#include "modelData.h"
#include "assimp/postprocess.h"
#include "shader.h"
#include "keyboard.h"

#include "modelAnimation.h"
#include "processor_manager.h"
#include "renderer_3dmodel_processor.h"

ID3D11Device* g_pDevice = nullptr; 
ID3D11DeviceContext* g_pContext = nullptr;

void ModelData::Initialize(const char* modelPath)
{
	g_pDevice = Direct3D_GetDevice();
	g_pContext = Direct3D_GetDeviceContext();

    Renderer3DModelProcessor* pModelProcessor = GetRenderer3DModelProcessor();

    // 3Dモデル読み込み
    {
        // 読み込みフラグ
        const unsigned int modelFlags =
            aiProcess_Triangulate |           // ポリゴンをすべて三角形にする
            aiProcess_JoinIdenticalVertices | // 頂点を最適化
            aiProcess_LimitBoneWeights |      // 1頂点あたりのボーン影響数を4つに制限 (シェーダーで扱いやすくするため)
            aiProcess_ValidateDataStructure | // データ構造が壊れていないかチェック
            aiProcess_ConvertToLeftHanded;    // DirectX用に左手座標系に変換 (※)

        // ファイル読み込み！
        Assimp::Importer importer;
        const aiScene* modelScene = importer.ReadFile(modelPath, modelFlags);

        // エラーチェックとデバッグ
        if (!modelScene || modelScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !modelScene->mRootNode)
        {
            // デバッグ表示
            OutputDebugStringA(importer.GetErrorString());
            return;
        }

        m_meshes.reserve(modelScene->mNumMeshes);

        // ボーン名(ノード名)->ボーンのID(int)の連想配列
        std::map<std::string, UINT> m_bone_map;

        // ノード情報の取り出し
        BuildNodeTree(modelScene->mRootNode, -1, m_nodeIndicesMap);

        // モデル内の全テクスチャの取得
        for(UINT i = 0; i < modelScene->mNumTextures; i++)
		{
			aiTexture* texture = modelScene->mTextures[i];
            
            ComPtr<ID3D11ShaderResourceView> srv;
            TexMetadata metadata;
            ScratchImage image;
            LoadFromWICMemory(texture->pcData, texture->mWidth, WIC_FLAGS_NONE, &metadata, image);
            CreateShaderResourceView(g_pDevice, image.GetImages(), image.GetImageCount(), metadata, srv.GetAddressOf());
            assert(texture);
            
            m_textures[texture->mFilename.data] = srv;
		}

        // モデル内の全マテリアルの取得
        m_materials.resize(modelScene->mNumMaterials);
        for (UINT i = 0; i < modelScene->mNumMaterials; ++i)
        {
            aiMaterial* aiMat = modelScene->mMaterials[i];
			Material& mat = m_materials[i];

            aiString texturePath;
            aiColor4D color;

            // ベースカラー
            {
                // テクスチャ
                if (aiMat->GetTexture(aiTextureType_BASE_COLOR, 0, &texturePath) == aiReturn_SUCCESS)
                    mat.baseColorSRV = m_textures[texturePath.C_Str()];
                else
                {
                    mat.baseColorSRV = pModelProcessor->LoadModelTexture(L"asset/Texture/white.bmp"); // 白テクスチャを代わりに使う

                    // 係数
                    if (aiMat->Get("$raw.Maya|baseColor", 0, 0, color) == AI_SUCCESS)
                        mat.bufferData.baseColorFactor = { color.r, color.g, color.b, color.a };
                }
            }

            // 発光色
            {
                // テクスチャ
                if (aiMat->GetTexture(aiTextureType_EMISSION_COLOR, 0, &texturePath) == aiReturn_SUCCESS)
                    mat.emissionColorSRV = m_textures[texturePath.C_Str()];
                else
                    mat.emissionColorSRV = pModelProcessor->LoadModelTexture(L"asset/Texture/black.bmp"); // 黒テクスチャを代わりに使う
            }

            // マテリアル用定数バッファの作成
            {
                D3D11_BUFFER_DESC desc{};
                desc.Usage = D3D11_USAGE_DYNAMIC;
                desc.ByteWidth = sizeof(Material::BufferData);
                desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

				D3D11_SUBRESOURCE_DATA initData{};
				initData.pSysMem = &mat.bufferData;
                g_pDevice->CreateBuffer(&desc, &initData, mat.buffer.GetAddressOf());
			}
        }

        // メッシュ情報取り出し
        for (UINT i = 0; i < modelScene->mNumMeshes; i++)
        {
            aiMesh* mesh = modelScene->mMeshes[i];

            Mesh meshData;

            // メッシュ内の全頂点のボーン情報を一時的に保存する配列
            std::vector<std::vector<std::pair<int, float>>> temp_bone_data(mesh->mNumVertices);
            
            // メッシュ内のボーン情報の取り出し
            {

                // メッシュ内のボーン情報を読み込み、マップと一時データを埋める
                for (UINT b = 0; b < mesh->mNumBones; b++)
                {
                    aiBone* bone = mesh->mBones[b];
                    std::string bone_name = bone->mName.C_Str();
                    UINT bone_index = 0; // このボーンの「スケルトン全体でのID」

                    // マップにこのボーンがまだ登録されていなければ、新規登録する
                    if (m_bone_map.find(bone_name) == m_bone_map.end())
                    {
                        bone_index = static_cast<UINT>(m_bone_map.size()); // 新しいID (0, 1, 2, ...)
                        m_bone_map[bone_name] = bone_index;

                        aiMatrix4x4 mat = bone->mOffsetMatrix;
                        XMMATRIX offset_matrix = XMMatrixSet(
                            mat.a1, mat.a2, mat.a3, mat.a4, // 1行目
                            mat.b1, mat.b2, mat.b3, mat.b4, // 2行目
                            mat.c1, mat.c2, mat.c3, mat.c4, // 3行目
                            mat.d1, mat.d2, mat.d3, mat.d4  // 4行目
                        );

                        XMVECTOR position;
                        XMVECTOR rotation;
                        XMVECTOR scale;
                        XMMatrixDecompose(&scale, &rotation, &position, offset_matrix);

                        m_boneInverseMatrices.push_back(offset_matrix);

						// ボーンに対応するノードのインデックスを取得して保存
                        int bone_node_index = m_nodeIndicesMap[bone_name];
                        m_boneNodeIndices.push_back(bone_node_index);

						// ボーンの初期姿勢データをノードに保存
                        m_nodes[bone_node_index].m_globalTransform = XMMatrixInverse(nullptr, offset_matrix);
                    }
                    else
                    {
                        bone_index = m_bone_map[bone_name];
                    }

                    // このボーンが影響する「頂点」をすべて調べる
                    for (UINT w = 0; w < bone->mNumWeights; w++)
                    {
                        aiVertexWeight weight = bone->mWeights[w];
                        unsigned int local_vertex_id = weight.mVertexId; // このメッシュ内での頂点ID
                        float bone_weight = weight.mWeight;

                        // 一時データに (ボーンID, ウェイト) を格納
                        temp_bone_data[local_vertex_id].push_back({ bone_index, bone_weight });
                    }
                }
            }
            

            // 頂点データの読み込み
            {
                std::vector<Vertex> mesh_vertices;
                mesh_vertices.reserve(mesh->mNumVertices);

                for (UINT j = 0; j < mesh->mNumVertices; j++)
                {
                    Vertex v;

                    // 座標
                    v.position.x = mesh->mVertices[j].x;
                    v.position.y = mesh->mVertices[j].y;
                    v.position.z = mesh->mVertices[j].z;

                    // 法線
                    if (mesh->HasNormals())
                    {
                        v.normal.x = mesh->mNormals[j].x;
                        v.normal.y = mesh->mNormals[j].y;
                        v.normal.z = mesh->mNormals[j].z;
                    }

                    // UV座標
                    if (mesh->HasTextureCoords(0))
                    {
                        v.uv.x = mesh->mTextureCoords[0][j].x;
                        v.uv.y = mesh->mTextureCoords[0][j].y;
                    }

                    // ボーン情報
                    auto& bone_data = temp_bone_data[j];

                    for (int k = 0; k < 4; k++)
                    {
                        // ボーンデータがある場合
                        if (k < bone_data.size())
                        {
                            v.boneIndices[k] = bone_data[k].first;
                            v.boneWeights[k] = bone_data[k].second;
                        }
                        else
                        {
                            v.boneIndices[k] = 0;
                            v.boneWeights[k] = 0.0f;
                        }
                    }

                    mesh_vertices.push_back(v);
                }

                //頂点バッファの作成
                D3D11_BUFFER_DESC bd{};
                bd.Usage = D3D11_USAGE_IMMUTABLE;
                bd.ByteWidth = sizeof(Vertex) * mesh->mNumVertices; // 三角形
                bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                D3D11_SUBRESOURCE_DATA initData{};
                initData.pSysMem = mesh_vertices.data();
                g_pDevice->CreateBuffer(&bd, &initData, meshData.vertexBuffer.GetAddressOf());
            }

            // インデックスデータの読み込み
            {
                std::vector<UINT> mesh_indices;
                meshData.numIndices = mesh->mNumFaces * 3;
                mesh_indices.reserve(meshData.numIndices);

                for (UINT j = 0; j < mesh->mNumFaces; j++)
                {
                    aiFace face = mesh->mFaces[j];

                    mesh_indices.push_back(face.mIndices[0]);
                    mesh_indices.push_back(face.mIndices[1]);
                    mesh_indices.push_back(face.mIndices[2]);
                }

                // インデックスバッファの作成
                D3D11_BUFFER_DESC bd{};
                bd.Usage = D3D11_USAGE_IMMUTABLE;
                bd.ByteWidth = sizeof(UINT) * mesh->mNumFaces * 3;
                bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
                D3D11_SUBRESOURCE_DATA initData{};
                initData.pSysMem = mesh_indices.data();
                g_pDevice->CreateBuffer(&bd, &initData, meshData.indexBuffer.GetAddressOf());

            }

            //マテリアルインデックスの取得
            meshData.materialIndex = mesh->mMaterialIndex;

            m_meshes.push_back(std::move(meshData));
        }

    }

    m_isValidate = true;
}

void ModelData::LoadAnimation(const char* animationName, const char* path)
{
	auto it = m_animations.find(animationName);

	// すでに同じ名前のアニメーションがある場合は何もしない
    if(it != m_animations.end())
		return;

    ModelAnimation& animation = m_animations[animationName];

    // モーション読み込み
    {
        // 読み込みフラグ
        const unsigned int modelFlags =
            aiProcess_Triangulate |           // ポリゴンをすべて三角形にする
            aiProcess_JoinIdenticalVertices | // 頂点を最適化
            aiProcess_LimitBoneWeights |      // 1頂点あたりのボーン影響数を4つに制限 (シェーダーで扱いやすくするため)
            aiProcess_ValidateDataStructure | // データ構造が壊れていないかチェック
            aiProcess_ConvertToLeftHanded;    // DirectX用に左手座標系に変換 (※)

        // ファイル読み込み
        Assimp::Importer impoter;
        const aiScene* motionScene = impoter.ReadFile(path, modelFlags);

        if (motionScene->HasAnimations())
        {
            animation.channelNodeIndices.resize(m_nodes.size(), -1);

            // 一時的にマップを使ってチャンネルを "名前" でグルーピングする
            std::map<std::string, ModelAnimation::Channel> channelMap;

            auto aiAnimation = motionScene->mAnimations[0];

            // 60FPS換算でのティック数を計算して保存
            animation.m_ticksPerFrame = static_cast<float>(aiAnimation->mTicksPerSecond) / 60;

            animation.m_animationDuration = static_cast<float>(aiAnimation->mDuration);

            // 全ノードのキーフレームをループ
            for (UINT i = 0; i < aiAnimation->mNumChannels; i++)
            {
                aiNodeAnim* nodeAnim = aiAnimation->mChannels[i];
                std::string rawName = nodeAnim->mNodeName.C_Str();
                std::string coreName = GetCoreNodeName(rawName);
                ModelAnimation::Channel& channel = channelMap[coreName];

                // (初めてこの名前が見つかった場合、nodeName をセット)
                if (channel.nodeName.empty())
                {
                    channel.nodeName = coreName;
                }

                // Position
                for (UINT p = 0; p < nodeAnim->mNumPositionKeys; p++)
                {
                    double time = nodeAnim->mPositionKeys[p].mTime;

                    // タイムスタンプが 0.0 以上かチェック
                    if (time >= 0.0)
                    {
                        aiVector3D pos = nodeAnim->mPositionKeys[p].mValue;
                        channel.positionKeys.push_back({ time, {pos.x, pos.y, pos.z} });
                    }
                }

                // Rotation
                for (UINT r = 0; r < nodeAnim->mNumRotationKeys; r++)
                {
                    double time = nodeAnim->mRotationKeys[r].mTime;

                    // タイムスタンプが 0.0 以上かチェック
                    if (time >= 0.0)
                    {
                        aiQuaternion rot = nodeAnim->mRotationKeys[r].mValue;
                        channel.rotationKeys.push_back({ time, {rot.x, rot.y, rot.z, rot.w} });
                    }
                }

                // Scaling
                for (UINT s = 0; s < nodeAnim->mNumScalingKeys; s++)
                {
                    double time = nodeAnim->mScalingKeys[s].mTime;

                    // タイムスタンプが 0.0 以上かチェック
                    if (time >= 0.0)
                    {
                        aiVector3D scale = nodeAnim->mScalingKeys[s].mValue;
                        channel.scalingKeys.push_back({ time, {scale.x, scale.y, scale.z} });
                    }
                }

                // もしモデルと同一のノード名があれば、NodeのIndexと、ChannelのIndexのペアを登録しておく
                auto it = m_nodeIndicesMap.find(coreName);
                if (it != m_nodeIndicesMap.end())
                    animation.channelNodeIndices[it->second] = static_cast<int>(i);

            }

            // 最後にmapからvectorに移し替える
            animation.channels.clear();
            for (auto& pair : channelMap)
            {
                animation.channels.push_back(pair.second);
            }

        }

    }

}

int ModelData::BuildNodeTree(aiNode* assimpNode, int parentIndex, std::map<std::string, int>& nodeMap)
{
    Node myNode;

    // ローカル行列を変換して保存 (aiMatrix4x4 -> XMMATRIX)
    aiMatrix4x4 matrix = assimpNode->mTransformation;
    DirectX::XMMATRIX localTransform = XMMatrixSet(
        matrix.a1, matrix.a2, matrix.a3, matrix.a4,
        matrix.b1, matrix.b2, matrix.b3, matrix.b4,
        matrix.c1, matrix.c2, matrix.c3, matrix.c4,
        matrix.d1, matrix.d2, matrix.d3, matrix.d4
    );

    XMMatrixDecompose(
        &myNode.m_tposeScale,
        &myNode.m_tposeRotation,
        &myNode.m_tposePosition,
        localTransform
    );

    myNode.m_name = assimpNode->mName.C_Str();
    myNode.m_parentIndex = parentIndex;

	int myIndex = static_cast<int>(m_nodes.size());
	myNode.index = myIndex;

    // ★ m_nodes にコピーして追加
    m_nodes.push_back(myNode);

    // ノード名->ポインタの連想配列を埋める
	nodeMap[myNode.m_name] = myIndex;

    // 再帰処理
    // 自分のすべての子ノード (mChildren) に対して
    for (UINT i = 0; i < assimpNode->mNumChildren; i++)
    {
        int childIndex = BuildNodeTree(assimpNode->mChildren[i], myIndex, nodeMap);

        // 自分の m_childrenIndices に、子のインデックスを追加
        m_nodes[myIndex].m_childrenIndices.push_back(childIndex);
    }

    // 自分のインスタンスを親に返す
    return myIndex;
}

std::string ModelData::GetCoreNodeName(const std::string& fullChannelName)
{
    std::string name = fullChannelName;

    // Prefixを削除 ("モデル名":)
    size_t prefixPos = name.find(':');
    if (prefixPos != std::string::npos)
    {
        name = name.substr(prefixPos + 1);
    }

    return name;
}

