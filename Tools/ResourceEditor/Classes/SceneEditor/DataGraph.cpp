#include "DataGraph.h"
#include "ControlsFactory.h"

#include "../EditorScene.h"
#include "EditorSettings.h"
#include "PropertyControlCreator.h"

DataGraph::DataGraph(GraphBaseDelegate *newDelegate, const Rect &rect)
    :   GraphBase(newDelegate, rect)
    ,   workingNode(NULL)
{
    CreateGraphPanel(rect);
}

DataGraph::~DataGraph()
{
}

void DataGraph::SelectNode(BaseObject *node)
{
    workingNode = dynamic_cast<DataNode *>(node);
    UpdatePropertyPanel();
    
    RefreshGraph();
}


void DataGraph::CreateGraphPanel(const Rect &rect)
{
    GraphBase::CreateGraphPanel(rect);
    
    Rect graphRect = graphPanel->GetRect();
    graphRect.dy -= (ControlsFactory::BUTTON_HEIGHT);
    graphTree = new UIHierarchy(graphRect);
    ControlsFactory::CusomizeListControl(graphTree);
    ControlsFactory::SetScrollbar(graphTree);
    graphTree->SetCellHeight(ControlsFactory::CELL_HEIGHT);
    graphTree->SetDelegate(this);
    graphTree->SetClipContents(true);
    graphPanel->AddControl(graphTree);

    
    int32 leftSideWidth = EditorSettings::Instance()->GetLeftPanelWidth();
    UIButton * refreshButton = ControlsFactory::CreateButton(Rect(0, graphRect.dy, 
                                                                  leftSideWidth,ControlsFactory::BUTTON_HEIGHT), 
                                                             LocalizedString(L"panel.refresh"));
    refreshButton->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &DataGraph::OnRefreshGraph));
    graphPanel->AddControl(refreshButton);
    SafeRelease(refreshButton);
}

void DataGraph::FillCell(UIHierarchyCell *cell, void *node)
{
    DataNode *n = (DataNode *)node;
    cell->text->SetText(StringToWString(n->GetName()));
    if(n == workingNode)
    {
        cell->SetSelected(true, false);
    }
    else
    {
        cell->SetSelected(false, false);
    }
}

void DataGraph::SelectHierarchyNode(UIHierarchyNode * node)
{
    DataNode * dataNode = dynamic_cast<DataNode*>((BaseObject*)node->GetUserNode());
    if (dataNode)
    {
        workingNode = dataNode;
        UpdatePropertyPanel();
    }
}


void DataGraph::UpdatePropertyPanel()
{
    if(workingNode && (NULL != graphTree->GetParent()))
    {
		RecreatePropertiesPanelForNode(workingNode);
        if(!propertyControl->GetParent())
        {
            propertyPanel->AddControl(propertyControl);
        }
        RefreshProperties();
    }
    else
    {
        if(propertyControl && propertyControl->GetParent())
        {
            propertyPanel->RemoveControl(propertyControl);
        }
    }
}

void DataGraph::RecreatePropertiesPanelForNode(DataNode * node)
{
	if(propertyControl && propertyControl->GetParent())
	{
		propertyPanel->RemoveControl(propertyControl);
	}
	SafeRelease(propertyControl);
    
	propertyControl = PropertyControlCreator::Instance()->CreateControlForNode(node, propertyPanelRect, false);
    SafeRetain(propertyControl);
	propertyControl->SetDelegate(this);
	propertyControl->SetWorkingScene(workingScene);
    propertyControl->ReadFrom(node);
}


void DataGraph::OnRefreshGraph(BaseObject * obj, void *, void *)
{
    RefreshGraph();
}

void DataGraph::RefreshGraph()
{
    bool force = true;
    if(force || (NULL != graphPanel->GetParent()))
    {
//        workingNode = NULL;
        dataNodes.clear();
        
        if(workingScene->GetSelection())
        {
            workingScene->GetSelection()->GetDataNodes(dataNodes);
        }
        
        graphTree->Refresh();
    }
}


#pragma mark --UIHierarchyDelegate
bool DataGraph::IsNodeExpandable(UIHierarchy *forHierarchy, void *forNode)
{
    if (forNode) 
    {
        return ((DataNode*)forNode)->GetChildrenCount() > 0;
    }
    
    return dataNodes.size() > 0;
}

int32 DataGraph::ChildrenCount(UIHierarchy *forHierarchy, void *forParent)
{
    if (forParent) 
    {
        return ((DataNode*)forParent)->GetChildrenCount();
    }
    
    return dataNodes.size();
}

void * DataGraph::ChildAtIndex(UIHierarchy *forHierarchy, void *forParent, int32 index)
{
    if (forParent) 
    {
        return ((DataNode*)forParent)->GetChild(index);
    }
    
    
    Set<DataNode *>::const_iterator it = dataNodes.begin();
    Set<DataNode *>::const_iterator endIt = dataNodes.end();
    for(int32 i = 0; it != endIt; ++it, ++i)
    {
        if(i == index)
        {
            return (*it);
        }
    }
    
    return NULL;
}


