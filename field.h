#pragma once
//===================================================
// field.h [フィールド]
// 
// Author：Miu Kitamura
// Date  ：2025/06/16
//===================================================
#include <d3d11.h>

void field_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void field_Finalize(void);
void field_Draw(void);