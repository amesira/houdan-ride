//===================================================
// renderer_3dcube_processor.h
// 
// AuthorÅFMiu Kitamura
// Date  ÅF2025/10/27
//===================================================
#ifndef RENDERER_3DCUBE_PROCESSOR_H
#define RENDERER_3DCUBE_PROCESSOR_H

#include <vector>
#include "processor.h"

class TransformComponent;
class CubemeshComponent;

class Renderer3DCubeProcessor : public Processor{
private:
    struct Components {
        TransformComponent* m_transform;
        CubemeshComponent*  m_cubemesh;
    };
    std::vector<Components> m_components;

public:
    void    Initialize()override;
    void    Finalize()override;

    void    Process()override;

    void    Entry
    (
        TransformComponent* transform,
        CubemeshComponent* cubemesh
    ) {

        Components cmps = {
            transform,cubemesh
        };
        m_components.push_back(cmps);
    }
};

#endif