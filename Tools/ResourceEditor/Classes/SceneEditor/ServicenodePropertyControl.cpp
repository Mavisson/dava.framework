#include "ServicenodePropertyControl.h"

ServicenodePropertyControl::ServicenodePropertyControl(const Rect & rect)
:   NodePropertyControl(rect)
{
    
}

void ServicenodePropertyControl::InitProperties()
{
    NodePropertyControl::InitProperties();
}

void ServicenodePropertyControl::SetDefaultValues()
{
    propertyList->SetStringPropertyValue("Name", "Service node");
}

void ServicenodePropertyControl::ReadFromNode(SceneNode *sceneNode)
{
    NodePropertyControl::ReadFromNode(sceneNode);

}

void ServicenodePropertyControl::ReadToNode(SceneNode *sceneNode)
{
    NodePropertyControl::ReadToNode(sceneNode);
}

