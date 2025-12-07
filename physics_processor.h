//----------------------------------------------------
// physics_processor.h [物理演算制御プロセッサー]
// 
// ・重力、摩擦などをvelocity（速度）に設定し、位置への適用を行う
// ・物理演算群の1番目にあたる。
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//----------------------------------------------------
#ifndef PHYSICS_PROCESSOR_H
#define PHYSICS_PROCESSOR_H

#include <vector>
#include "processor.h"

class TransformComponent;
class RigidbodyComponent;

class PhysicsProcessor : public Processor {
private:
    struct Components {
        TransformComponent* m_transform;
        RigidbodyComponent* m_rigidbody;
    };
    std::vector<Components> m_components;

public:
    void    Initialize()override;
    void    Finalize()override;

    void    Process()override;

    void    Entry(TransformComponent* transform,RigidbodyComponent* rigidbody) {
        Components cmps = { transform,rigidbody };
        m_components.push_back(cmps);
    }
};


#endif