//===================================================
// model_component.h
// 
// Author：Hayato Ushimaru
// Date  ：2025/11/17
//===================================================
#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"

#include "component.h"
#include "modelData.h"

class ModelComponent : public Component {
private:
    ModelData* m_pModelData;

public:
    // 型チェック用関数（GetComponent用）
    static Type GetTypeStatic() {
        return Type::Model;
    }
    Type GetType() const override {
        return GetTypeStatic();
    }

	// モデルの設定・取得
    void SetModelData(ModelData* pModelData) { m_pModelData = pModelData; }
    ModelData* GetModelData() { return m_pModelData; }
};

#endif