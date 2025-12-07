//===================================================
// camera_component_processor.h
// 
// Author：Hayato Ushimaru
// Date  ：2025/11/18
//===================================================
#ifndef CAMERA_COMPONENT_PROCESSOR_H
#define CAMERA_COMPONENT_PROCESSOR_H

#include <vector>
#include "processor.h"


class TransformComponent;
class CameraComponent;
struct ID3D11ShaderResourceView;

class CameraComponentProcessor : public Processor {
private:
    struct Components {
        TransformComponent* m_transform;
        CameraComponent* m_camera;
    };
    std::vector<Components> m_components;

public:
    void    Initialize()override;
    void    Finalize()override;

    void    Process()override;

    void    Entry(TransformComponent* transform, CameraComponent* camera) {
        Components cmps = {
            transform,camera
        };
        m_components.push_back(cmps);
    }

    int GetSize() const {
        return static_cast<int>(m_components.size());
	}

	// カメラのビュー・投影変換行列をパイプラインに紐づけます
    //
	// componentIndex：m_componentsのインデックス
    void BindMatrix(int componentIndex);

	// シーンテクスチャのSRVにスナップショットを取ります
	// 
    // componentIndex：m_componentsのインデックス
	void SnapShotSceneSRV(int componentIndex);

	ID3D11ShaderResourceView* GetCameraSRV(int componentIndex);

	// 画面にカメラのシーンを描画します
	// ビューポートの内容に従って描画されます
    // 
	//componentIndex：m_componentsのインデックス
    void DrawFSQuad(int componentIndex);
};

#endif