#ifndef __DAVAENGINE_SCENE3D_RENDER_COMPONENT_H__
#define __DAVAENGINE_SCENE3D_RENDER_COMPONENT_H__

#include "Base/BaseTypes.h"
#include "Entity/Component.h"
#include "Render/Highlevel/RenderObject.h"

namespace DAVA 
{

class RenderComponent : public Component
{
public:
    RenderComponent(RenderObject * _object = 0);
    ~RenderComponent();
    
    IMPLEMENT_COMPONENT_TYPE(RENDER_COMPONENT);
    virtual Component * Clone();

    void SetRenderObject(RenderObject * object);
    RenderObject * GetRenderObject();
private:
    RenderObject * renderObject;
};


};

#endif //__DAVAENGINE_SCENE3D_RENDER_COMPONENT_H__