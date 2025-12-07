//----------------------------------------------------
// renderer_image_processor.h
// 
// Author：Miu Kitamura
// Date  ：2025/11/18
//----------------------------------------------------
#ifndef RENDERER_IMAGE_PROCESSOR_H
#define RENDERER_IMAGE_PROCESSOR_H

#include <vector>
#include "processor.h"

class RectTransformComponent;
class ImageComponent;

class RendererImageProcessor : public Processor {
private:
    struct Components {
        RectTransformComponent*   m_rectTransform = nullptr;
        ImageComponent*           m_imageComponent = nullptr;
    };
    std::vector<Components> m_components;

public:
    void    Initialize()override;
    void    Finalize()override;
    void    Process()override;

    void    Entry(
        RectTransformComponent* pRectTransform,
        ImageComponent* pImageComponent
    ) {
        m_components.push_back({ pRectTransform, pImageComponent });
    }
};

#endif