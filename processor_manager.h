#pragma once
// processor_manager.h

class CameraComponentProcessor;
class Renderer3DCubeProcessor;
class Renderer3DModelProcessor;
class PhysicsProcessor;
class CollisionProcessor;
class DynamicsProcessor;
class RendererFontProcessor;
class RendererImageProcessor;

void    ProcessorM_Initialize();
void    ProcessorM_Finalize();
void    ProcessorM_Update();
void    ProcessorM_Draw();

CameraComponentProcessor*   GetCameraComponentProcessor();
Renderer3DCubeProcessor*    GetRenderer3DCubeProcessor();
Renderer3DModelProcessor*	GetRenderer3DModelProcessor();
PhysicsProcessor*           GetPhysicsProcessor();
CollisionProcessor*         GetCollisionProcessor();
DynamicsProcessor*          GetDynamicsProcessor();
RendererFontProcessor*      GetRendererFontProcessor();
RendererImageProcessor*     GetRendererImageProcessor();