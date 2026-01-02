//===================================================
// polygon3d.cpp
// 
// ・3Dポリゴンのテスト用
//===================================================
#include "polygon3d.h"

// graphics, devices
#include "shader.h"
#include "sprite.h"
#include "keyboard.h"

static  ID3D11Device* g_pDevice = NULL;
static  ID3D11DeviceContext* g_pContext = NULL;

// 頂点バッファ
static  ID3D11Buffer* g_VertexBuffer = NULL;

// インデックスバッファ
static ID3D11Buffer* g_IndexBuffer = NULL;

#define NUM_VERTEX (100)

// テスト用3Dモデル
static Vertex vData[NUM_VERTEX] = {
    // -Z面
    {   // 頂点0（Left-Top）
        XMFLOAT3(-0.5f,0.5f,-0.5f),      // 位置座標
        XMFLOAT3(0.0f,0.0f,-1.0f),    // 法線ベクトル
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),   // 色
        XMFLOAT2(0.0f,0.0f),             // テクスチャ座標
    },
    {   // 頂点1（Right-Top）
        XMFLOAT3(0.5f,0.5f,-0.5f),
        XMFLOAT3(0.0f,0.0f,-1.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(1.0f,0.0f),
    },
    {   // 頂点2（Left-Bottom）
        XMFLOAT3(-0.5f,-0.5f,-0.5f),
        XMFLOAT3(0.0f,0.0f,-1.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(0.0f,1.0f),
    },
    {   // 頂点5（Right-Bottom）
        XMFLOAT3(0.5f,-0.5f,-0.5f),
        XMFLOAT3(0.0f,0.0f,-1.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(1.0f,1.0f),
    },

    // +X面
    {   // 頂点6（Left-Top）= 1
        XMFLOAT3(0.5f,0.5f,-0.5f),
        XMFLOAT3(1.0f,0.0f,0.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(0.0f,0.0f),
    },
    {   // 頂点7（Right-Top）
        XMFLOAT3(0.5f,0.5f,0.5f),
        XMFLOAT3(1.0f,0.0f,0.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(1.0f,0.0f),
    },
    {   // 頂点8（Left-Bottom）= 5
        XMFLOAT3(0.5f,-0.5f,-0.5f),
        XMFLOAT3(1.0f,0.0f,0.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(0.0f,1.0f),
    },
    {   // 頂点11（Right-Bottom）
        XMFLOAT3(0.5f,-0.5f,0.5f),
        XMFLOAT3(1.0f,0.0f,0.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(1.0f,1.0f),
    },

    // +Z面
    {   // 頂点12（Left-Top）= 1 +z
        XMFLOAT3(0.5f,0.5f,0.5f),
        XMFLOAT3(0.0f,0.0f,1.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(0.0f,0.0f),
    },
    {   // 頂点13（Right-Top）= 0 +z
        XMFLOAT3(-0.5f,0.5f,0.5f),
        XMFLOAT3(0.0f,0.0f,1.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(1.0f,0.0f),
    },
    {   // 頂点14（Left-Bottom）= 5 +z
        XMFLOAT3(0.5f,-0.5f,0.5f),
        XMFLOAT3(0.0f,0.0f,1.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(0.0f,1.0f),
    },
    {   // 頂点17（Right-Bottom）= 2 +z
        XMFLOAT3(-0.5f,-0.5f,0.5f),
        XMFLOAT3(0.0f,0.0f,1.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(1.0f,1.0f),
    },

    // -X面
    {   // 頂点18（Left-Top）= 13
        XMFLOAT3(-0.5f,0.5f,0.5f),
        XMFLOAT3(-1.0f,0.0f,0.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(0.0f,0.0f),
    },
    {   // 頂点19（Right-Top）= 0
        XMFLOAT3(-0.5f,0.5f,-0.5f),
        XMFLOAT3(-1.0f,0.0f,0.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(1.0f,0.0f),
    },
    {   // 頂点20（Left-Bottom）= 17
        XMFLOAT3(-0.5f,-0.5f,0.5f),
        XMFLOAT3(-1.0f,0.0f,0.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(0.0f,1.0f),
    },
    {   // 頂点23（Right-Bottom）= 2
        XMFLOAT3(-0.5f,-0.5f,-0.5f),
        XMFLOAT3(-1.0f,0.0f,0.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(1.0f,1.0f),
    },

    // +Y面
    {   // 頂点24（Left-Top）= 13,18
        XMFLOAT3(-0.5f,0.5f,0.5f),
        XMFLOAT3(0.0f,1.0f,0.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(0.0f,0.0f),
    },
    {   // 頂点25（Right-Top）= 10,12
        XMFLOAT3(0.5f,0.5f,0.5f),
        XMFLOAT3(0.0f,1.0f,0.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(1.0f,0.0f),
    },
    {   // 頂点26（Left-Bottom）= 0,19
        XMFLOAT3(-0.5f,0.5f,-0.5f),
        XMFLOAT3(0.0f,1.0f,0.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(0.0f,1.0f),
    },
    {   // 頂点29（Right-Bottom）= 1
        XMFLOAT3(0.5f,0.5f,-0.5f),
        XMFLOAT3(0.0f,1.0f,0.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(1.0f,1.0f),
    },

    // -Y面
    {   // 頂点30（Left-Top）= 2
        XMFLOAT3(-0.5f,-0.5f,-0.5f),
        XMFLOAT3(0.0f,-1.0f,0.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(0.0f,0.0f),
    },
    {   // 頂点31（Right-Top）= 5
        XMFLOAT3(0.5f,-0.5f,-0.5f),
        XMFLOAT3(0.0f,-1.0f,0.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(1.0f,0.0f),
    },
    {   // 頂点32（Left-Bottom）= 20
        XMFLOAT3(-0.5f,-0.5f,0.5f),
        XMFLOAT3(0.0f,-1.0f,0.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(0.0f,1.0f),
    },
    {   // 頂点35（Right-Bottom）= 14
        XMFLOAT3(0.5f,-0.5f,0.5f),
        XMFLOAT3(0.0f,-1.0f,0.0f),
        XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
        XMFLOAT2(1.0f,1.0f),
    },
};

static UINT idxData[6 * 6] = {
    0,  1,  2,  2,  1,  3,
    4,  5,  6,  6,  5,  7,
    8,  9,  10, 10, 9,  11,
    12, 13, 14, 14, 13, 15,
    16, 17, 18, 18, 17, 19,
    20, 21, 22, 22, 21, 23,
};

//===================================================
// Polygon3D初期化
//===================================================
void Polygon3D_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    // デバイスの保持
    g_pDevice = pDevice;
    g_pContext = pContext;

    // 頂点バッファ作成
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(Vertex) * NUM_VERTEX;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        g_pDevice->CreateBuffer(&bd, NULL, &g_VertexBuffer);
    }

    // インデックスバッファ作成
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(UINT) * 6 * 6;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        g_pDevice->CreateBuffer(&bd, NULL, &g_IndexBuffer);

        // インデックスバッファへ書き込み
        D3D11_MAPPED_SUBRESOURCE msr;
        pContext->Map(g_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
        UINT* index = (UINT*)msr.pData;

        // インデックスデータをバッファへコピー
        CopyMemory(&index[0], &idxData[0], sizeof(UINT) * 6 * 6);
        pContext->Unmap(g_IndexBuffer, 0);
    }
}

//===================================================
// Polygon3D終了
//===================================================
void Polygon3D_Finalize()
{
    if (g_VertexBuffer != NULL) {
        g_VertexBuffer->Release();
        g_VertexBuffer = NULL;
    }
    if (g_IndexBuffer != NULL) {
        g_IndexBuffer->Release();
        g_IndexBuffer = NULL;
    }
}

//===================================================
// Polygon3D更新
//===================================================
void Polygon3D_Update()
{
    
}

void Polygon3D_CubeRender(
    DirectX::XMFLOAT3 position, DirectX::XMVECTOR rotation, DirectX::XMFLOAT3 scaling, 
    ID3D11ShaderResourceView* tex, DirectX::XMFLOAT4 color)
{
    for (Vertex& v : vData) {
        v.color = color;
    }

    // スケーリング行列の作成
    XMMATRIX    scalingMatrix = XMMatrixScaling
    (
        scaling.x,
        scaling.y,
        scaling.z
    );

    // 平行移動行列の作成
    XMMATRIX    translationMatrix = XMMatrixTranslation
    (
        position.x,
        position.y,
        position.z
    );

    // 回転行列の作成
    XMMATRIX    rotationMatrix = XMMatrixRotationQuaternion(rotation);

    // ワールド行列（乗算の順番に注意）
    XMMATRIX    worldMatrix =
        scalingMatrix *
        rotationMatrix *
        translationMatrix;

    // プロジェクション行列取得
    XMMATRIX    projection = Direct3D_GetProjectionMatrix();

    // ビュー行列取得
    XMMATRIX    view = Direct3D_GetViewMatrix();

    //----------------------------------------------------
    // 最終的な変換行列の作成
    //----------------------------------------------------
    XMMATRIX    wvp = worldMatrix * view * projection;

    // 変換行列を頂点シェーダーへセット
    Shader_SetMatrix(wvp);

    // シェーダーを描画パイプラインへ設定
    Shader_Begin();

    // 頂点データを頂点バッファへコピーする
    D3D11_MAPPED_SUBRESOURCE    msr;
    g_pContext->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    Vertex* vertex = (Vertex*)msr.pData;

    // 頂点データをコピーする
    CopyMemory(&vertex[0], &vData[0], sizeof(Vertex) * NUM_VERTEX);

    // コピー完了（頂点バッファのロックを解除）
    g_pContext->Unmap(g_VertexBuffer, 0);

    // テクスチャをセット
    g_pContext->PSSetShaderResources(0, 1, &tex);

    // 頂点バッファをセット
    UINT    stride = sizeof(Vertex);
    UINT    offset = 0;
    g_pContext->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

    // インデックスバッファをセット
    g_pContext->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // 描画するポリゴンの種類をセット
    g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 描画命令
    g_pContext->DrawIndexed(6 * 6, 0, 0);
}
