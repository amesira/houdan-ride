//===================================================
// camera_component.h
// 
// Author：Hayato Ushimaru
// Date  ：2025/11/18
//===================================================
#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

// Comオブジェクト用のスマートポインタ
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"

#include "component.h"

class CameraComponent : public Component {

public:

	// ビューポート矩形構造体
	// x, y: ビューポートの左下座標(原点(0,0)は画面左下)（0.0～1.0）
	// width, height: ビューポートの幅と高さ（0.0～1.0）
    struct ViewportRect
    {
		float x;        // ビューポートの左下X座標
		float y;        // ビューポートの左下Y座標
		float width;    // ビューポートの幅
		float height;   // ビューポートの高さ
    };

private:

    ComPtr<ID3D11ShaderResourceView> m_shaderResouceView;    // シェーダーリソースビュー

    DirectX::XMMATRIX m_viewMatrix;             // ビュー行列
    DirectX::XMMATRIX m_projectionMatrix;       // 投影変換行列

    float m_fieldOfView = 60.0f;                // 視野角
    float m_aspectRatio = 1920.0f / 1080.0f;    // アスペクト比
    float m_nearClip = 0.5f;                    // ニアクリップ距離
    float m_farClip = 1000.0f;                  // ファークリップ距離

	float m_depth = 0.0f;                       // カメラの深度（描画順序）
	ViewportRect m_viewportRect = { 0.0f, 0.0f, 1.0f, 1.0f }; // ビューポート矩形

public:
    // 型チェック用関数（GetComponent用）
    static Type GetTypeStatic() {
        return Type::Camera;
    }
    Type GetType() const override {
        return GetTypeStatic();
    }

    // ビュー変換行列を取得します
    //
    // camera_component_processor.cppで更新されています
    // 戻り値：ビュー変換行列
    const DirectX::XMMATRIX& GetViewMatrix() const { return m_viewMatrix; }

    // 投影変換行列を取得します
    //
    // camera_component_processor.cppで更新されています
    // 戻り値：投影変換行列
    const DirectX::XMMATRIX& GetProjectionMatrix() const { return m_projectionMatrix; }

    void    SetViewMatrix(const DirectX::XMMATRIX& viewMatrix) { m_viewMatrix = viewMatrix; }
    void    SetProjectionMatrix(const DirectX::XMMATRIX& projectionMatrix) { m_projectionMatrix = projectionMatrix; }

    float   GetFieldOfView() const { return m_fieldOfView; }
    float   GetAspectRatio() const { return m_aspectRatio; }
    float   GetNearClip() const { return m_nearClip; }
    float   GetFarClip() const { return m_farClip; }

    void    SetFieldOfView(float fov) { m_fieldOfView = fov; }
    void    SetAspectRatio(float aspect) { m_aspectRatio = aspect; }
    void    SetNearClip(float nearClip) { m_nearClip = nearClip; }
    void    SetFarClip(float farClip) { m_farClip = farClip; }

	float   GetDepth() const { return m_depth; }
	void    SetDepth(float depth) { m_depth = depth; }

	ViewportRect   GetViewportRect() const { return m_viewportRect; }
	void    SetViewportRect(const ViewportRect& rect) { m_viewportRect = rect; }

	// シェーダーリソースビューの取得・設定
    ID3D11ShaderResourceView* GetShaderResourceView() const { return m_shaderResouceView.Get(); }
	void    SetShaderResourceView(ComPtr<ID3D11ShaderResourceView>& srv) { m_shaderResouceView = srv; }
};


#endif