#pragma once
// processor_manager.h
class IScene;

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
void    ProcessorM_Update(IScene* pScene);
void    ProcessorM_Draw(IScene* pScene);

PhysicsProcessor*           GetPhysicsProcessor();
CollisionProcessor*         GetCollisionProcessor();
DynamicsProcessor*          GetDynamicsProcessor();
RendererFontProcessor*      GetRendererFontProcessor();
RendererImageProcessor*     GetRendererImageProcessor();