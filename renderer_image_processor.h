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

public:
    void    Initialize()override;
    void    Finalize()override;
    void    Process(IScene* pScene)override;

};

#endif