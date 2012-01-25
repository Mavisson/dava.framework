#include "SceneInfoControl.h"
#include "ControlsFactory.h"
#include "PropertyList.h"

SceneInfoControl::SceneInfoControl(const Rect &rect)
    :   UIControl(rect)
{
    workingScene = NULL;
    SetInputEnabled(false);
    
    sceneInfo = new PropertyList(Rect(0, 0, rect.dx, rect.dy), NULL);
    sceneInfo->SetInputEnabled(false);
    sceneInfo->GetBackground()->SetColor(Color(1.0f, 1.0f, 1.0f, 0.0f));
    AddControl(sceneInfo);
}


SceneInfoControl::~SceneInfoControl()
{
    SafeRelease(sceneInfo);
}

void SceneInfoControl::WillAppear()
{
    UpdateInfo(NULL, NULL, NULL);
}

void SceneInfoControl::UpdateInfo(BaseObject * owner, void * userData, void * callerData)
{
    if(workingScene)
    {
        SetIntInfoValue("Text.Count", Texture::TexturesCount());
        SetIntInfoValue("Text.Memory", Texture::AllocatedMemorySize());

        SetFloatInfoValue("Float Example", 1.3f);
        
        //Set cells background
        List<UIControl*> cells = sceneInfo->GetVisibleCells();
        for(List<UIControl*>::iterator it = cells.begin(); it != cells.end(); ++it)
        {
            UIControl *cell = (*it);
            ControlsFactory::CusomizeTransparentControl(cell, 0.1f);
        }
    }
    
    Animation * anim = WaitAnimation(10.f); //every 10 seconds
    anim->AddEvent(Animation::EVENT_ANIMATION_END, Message(this, &SceneInfoControl::UpdateInfo));
}

void SceneInfoControl::SetIntInfoValue(const String &key, int32 newValue)
{
    if(!sceneInfo->IsPropertyAvaliable(key))
    {
        sceneInfo->AddIntProperty(key, PropertyList::PROPERTY_IS_READ_ONLY);
    }
    
    sceneInfo->SetIntPropertyValue(key, newValue);
}

void SceneInfoControl::SetFloatInfoValue(const String &key, float32 newValue)
{
    if(!sceneInfo->IsPropertyAvaliable(key))
    {
        sceneInfo->AddFloatProperty(key, PropertyList::PROPERTY_IS_READ_ONLY);
    }
    
    sceneInfo->SetFloatPropertyValue(key, newValue);
}


void SceneInfoControl::SetWorkingScene(Scene *scene)
{
    workingScene = scene;
}