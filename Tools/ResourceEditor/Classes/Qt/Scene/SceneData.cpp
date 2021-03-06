#include "Scene/SceneData.h"
#include "Main/SceneGraphModel.h"

#include "../EditorScene.h"
#include "../SceneEditor/EditorSettings.h"
#include "../SceneEditor/SceneValidator.h"

#include "../SceneEditor/SceneEditorScreenMain.h"

#include "../Commands/SceneGraphCommands.h"
#include "../Commands/LibraryCommands.h"
#include "../Commands/CommandsManager.h"

#include "../LandscapeEditor/LandscapesController.h"

#include "Main/QtMainWindowHandler.h"


#include "Main/QtUtils.h"
#include "Main/PointerHolder.h"

#include "DockLibrary//LibraryModel.h"

#include <QTreeView>
#include <QFileSystemModel>
#include <QDir>
#include <QFileInfo>
#include <QMenu>
#include <QAction>

using namespace DAVA;

SceneData::SceneData()
    :   QObject(NULL)
    ,   scene(NULL)
    ,   sceneGraphModel(NULL)
{
    //libraryModel = NULL;
    //libraryView = NULL;
    sceneGraphView = NULL;
    
    landscapesController = new LandscapesController();
    
    sceneFilePathname = String("");
    sceneGraphModel = new SceneGraphModel();
    sceneGraphModel->SetScene(NULL);
    

    cameraController = new WASDCameraController(EditorSettings::Instance()->GetCameraSpeed());
    
    //skipLibraryPreview = false;
    
    connect(sceneGraphModel, SIGNAL(SceneNodeSelected(DAVA::SceneNode *)), this, SLOT(SceneNodeSelectedInGraph(DAVA::SceneNode *)));
}

SceneData::~SceneData()
{
    SafeRelease(scene);
    
    SafeRelease(landscapesController);
    
    SafeDelete(sceneGraphModel);
    SafeRelease(cameraController);
}


void SceneData::RebuildSceneGraph()
{
    sceneGraphModel->Rebuild();
}

EditorScene * SceneData::GetScene()
{
    return scene;
}

void SceneData::AddSceneNode(DAVA::SceneNode *node)
{
    scene->AddNode(node);
    
    LandscapeNode *landscape = dynamic_cast<LandscapeNode *>(node);
    if(landscape)
    {
        landscapesController->SaveLandscape(landscape);
    }
    
    RebuildSceneGraph();
}

void SceneData::RemoveSceneNode(DAVA::SceneNode *node)
{
    SceneNode * parent = node->GetParent();
    if (parent)
    {
        LandscapeNode *landscape = dynamic_cast<LandscapeNode *>(node);
        if(landscape)
        {
            landscapesController->SaveLandscape(NULL);
        }
        
        
        scene->ReleaseUserData(node);
        
        sceneGraphModel->SelectNode(NULL);
        scene->SetSelection(NULL);
        
        parent->RemoveNode(node);

        SceneValidator::Instance()->EnumerateSceneTextures();
    }
    
    RebuildSceneGraph();
}

void SceneData::SelectNode(DAVA::SceneNode *node)
{
    sceneGraphModel->SelectNode(node);
}

void SceneData::SceneNodeSelectedInGraph(SceneNode *node)
{
    if(scene)   scene->SetSelection(node);
    
    cameraController->SetSelection(node);

    //TODO: remove code at full-qt version
    SceneEditorScreenMain *screen = dynamic_cast<SceneEditorScreenMain *>(UIScreenManager::Instance()->GetScreen());
    if(screen)
    {
        screen->SelectNodeQt(node);
    }
    //EndOfTODO
    
    
    Camera * cam = dynamic_cast<Camera*>(node);
    if (cam)
    {
        if (IsKeyModificatorPressed(DVKEY_ALT))
        {
            scene->SetClipCamera(cam);
        }
        else
        {
            scene->SetCurrentCamera(cam);
        }
    }

	emit SceneNodeSelected(node);
}



DAVA::SceneNode * SceneData::GetSelectedNode()
{
    return sceneGraphModel->GetSelectedNode();
}

void SceneData::LockAtSelectedNode()
{
    if(cameraController)
    {
        cameraController->LookAtSelection();
    }
}

CameraController * SceneData::GetCameraController()
{
    return cameraController;
}

void SceneData::SetScene(EditorScene *newScene)
{
    ReleaseScene();
    
    scene = SafeRetain(newScene);
    sceneGraphModel->SetScene(scene);
    cameraController->SetScene(scene);
    landscapesController->SetScene(scene);
}

void SceneData::ReleaseScene()
{
    cameraController->SetScene(NULL);
    sceneGraphModel->SetScene(NULL);
    landscapesController->SetScene(NULL);
    
    SafeRelease(scene);
}

void SceneData::CreateScene(bool createEditorCameras)
{
    ReleaseScene();

    cameraController->SetSpeed(EditorSettings::Instance()->GetCameraSpeed());
    
    EditorScene *createdScene = new EditorScene();

    // Camera setup
    if(createEditorCameras)
    {
        Camera * cam = new Camera();
        cam->SetName("editor.main-camera");
        cam->SetUp(Vector3(0.0f, 0.0f, 1.0f));
        cam->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
        cam->SetTarget(Vector3(0.0f, 1.0f, 0.0f));
        
        cam->Setup(70.0f, 320.0f / 480.0f, 1.0f, 5000.0f);
        
        createdScene->AddNode(cam);
        createdScene->AddCamera(cam);
        createdScene->SetCurrentCamera(cam);
        
        SafeRelease(cam);
        
        Camera * cam2 = new Camera();
        cam2->SetName("editor.debug-camera");
        cam2->SetUp(Vector3(0.0f, 0.0f, 1.0f));
        cam2->SetPosition(Vector3(0.0f, 0.0f, 200.0f));
        cam2->SetTarget(Vector3(0.0f, 250.0f, 0.0f));
        
        cam2->Setup(70.0f, 320.0f / 480.0f, 1.0f, 5000.0f);
        
        createdScene->AddNode(cam2);
        createdScene->AddCamera(cam2);
        
        SafeRelease(cam2);
    }
    
    SetScene(createdScene);
    SafeRelease(createdScene);
}

void SceneData::AddScene(const String &scenePathname)
{
    String extension = FileSystem::Instance()->GetExtension(scenePathname);
    DVASSERT((".sc2" == extension) && "Wrong file extension.");
    
    SceneNode * rootNode = scene->GetRootNode(scenePathname)->Clone();
    
    KeyedArchive * customProperties = rootNode->GetCustomProperties();
    customProperties->SetString("editor.referenceToOwner", scenePathname);
    
    rootNode->SetSolid(true);
    scene->AddNode(rootNode);
    
    Camera *currCamera = scene->GetCurrentCamera();
    if(currCamera)
    {
        Vector3 pos = currCamera->GetPosition();
        Vector3 direction  = currCamera->GetDirection();
        
        Vector3 nodePos = pos + 10 * direction;
        nodePos.z = 0;
        
        LandscapeNode * ls = scene->GetLandScape(scene);
        if(ls)
        {
            Vector3 result;
            bool res = ls->PlacePoint(nodePos, result);
            if(res)
            {
                nodePos = result;
            }
        }
        
        Matrix4 mod;
        mod.CreateTranslation(nodePos);
        rootNode->SetLocalTransform(rootNode->GetLocalTransform() * mod);
    }

	List<LandscapeNode *> landscapes;
	rootNode->GetChildNodes(landscapes);

	bool needUpdateLandscapeController = !landscapes.empty();

    SafeRelease(rootNode);

    //TODO: need save scene automatically?
    bool changesWereMade = SceneValidator::Instance()->ValidateSceneAndShowErrors(scene);
    SceneValidator::Instance()->EnumerateSceneTextures();

	if(needUpdateLandscapeController)
	{
		landscapesController->SetScene(scene);
	}
    
    RebuildSceneGraph();
}

void SceneData::EditScene(const String &scenePathname)
{
    String extension = FileSystem::Instance()->GetExtension(scenePathname);
    DVASSERT((".sc2" == extension) && "Wrong file extension.");

    SceneNode * rootNode = scene->GetRootNode(scenePathname);
    if(rootNode)
    {
        SetScenePathname(scenePathname);
		Vector<SceneNode*> tempV;
		tempV.reserve(rootNode->GetChildrenCount());

		for (int32 ci = 0; ci < rootNode->GetChildrenCount(); ++ci)
		{
			tempV.push_back(rootNode->GetChild(ci));
		}
        for (int32 ci = 0; ci < (int32)tempV.size(); ++ci)
        {
            //рут нода это сама сцена в данном случае
            scene->AddNode(tempV[ci]);
        }
    }

    
    //TODO: need save scene automatically?
    bool changesWereMade = SceneValidator::Instance()->ValidateSceneAndShowErrors(scene);
    SceneValidator::Instance()->EnumerateSceneTextures();
   
    landscapesController->SetScene(scene);

	scene->Update(0);
	emit SceneChanged(scene);

    RebuildSceneGraph();
}

void SceneData::AddReferenceScene(const DAVA::String &scenePathname)
{
	String extension = FileSystem::Instance()->GetExtension(scenePathname);
	DVASSERT((".sc2" == extension) && "Wrong file extension.");

	SceneNode * rootNode = scene->GetRootNode(scenePathname);

	DVASSERT(rootNode->GetChildrenCount() == 1);
	ReferenceNode * refNode = new ReferenceNode();
	SceneNode * clone = rootNode->GetChild(0)->Clone();
	refNode->AddNode(clone);
	refNode->SetName(rootNode->GetName());
	SafeRelease(clone);

	KeyedArchive * customProperties = refNode->GetCustomProperties();
	customProperties->SetString("reference.path", scenePathname);

	refNode->SetSolid(true);
	scene->AddNode(refNode);

	Camera *currCamera = scene->GetCurrentCamera();
	if(currCamera)
	{
		Vector3 pos = currCamera->GetPosition();
		Vector3 direction  = currCamera->GetDirection();

		Vector3 nodePos = pos + 10 * direction;
		nodePos.z = 0;

		LandscapeNode * ls = scene->GetLandScape(scene);
		if(ls)
		{
			Vector3 result;
			bool res = ls->PlacePoint(nodePos, result);
			if(res)
			{
				nodePos = result;
			}
		}

		Matrix4 mod;
		mod.CreateTranslation(nodePos);
		refNode->SetLocalTransform(refNode->GetLocalTransform() * mod);
	}
	SafeRelease(refNode);

	RebuildSceneGraph();

    //TODO: need save scene automatically?
    bool changesWereMade = SceneValidator::Instance()->ValidateSceneAndShowErrors(scene);
	SceneValidator::Instance()->EnumerateSceneTextures();
}

void SceneData::SetScenePathname(const String &newPathname)
{
    sceneFilePathname = newPathname;
    if(scene)
    {
        String filename, path;
        FileSystem::Instance()->SplitPath(sceneFilePathname, path, filename);
        scene->SetName(filename);
    }
}

String SceneData::GetScenePathname() const
{
    return sceneFilePathname;
}

void SceneData::Activate(QTreeView *graphview/*, QTreeView *_libraryView, LibraryModel *libModel*/)
{
    sceneGraphView = graphview;
	//libraryView = _libraryView;
		
    sceneGraphModel->Activate(sceneGraphView);
    
    //connect(libraryView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(LibraryContextMenuRequested(const QPoint &)));
    connect(sceneGraphView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(SceneGraphContextMenuRequested(const QPoint &)));
    
    //libraryModel = libModel;
    //connect(libraryModel->GetSelectionModel(), SIGNAL(FileSelected(const QString &, bool)), this, SLOT(FileSelected(const QString &, bool)));

    //QtMainWindowHandler::Instance()->ShowStatusBarMessage(sceneFilePathname);
}

void SceneData::Deactivate()
{
	/*
    if(libraryModel)
    {
        disconnect(libraryModel->GetSelectionModel(), SIGNAL(FileSelected(const QString &, bool)), this, SLOT(FileSelected(const QString &, bool)));
        libraryModel = NULL;
    }
    
    disconnect(libraryView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(LibraryContextMenuRequested(const QPoint &)));
	*/
    disconnect(sceneGraphView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(SceneGraphContextMenuRequested(const QPoint &)));
    
    sceneGraphModel->Deactivate();
}

/*
void SceneData::ShowLibraryMenu(const QModelIndex &index, const QPoint &point)
{
    if(!index.isValid())
    {
        return;
    }
    
    QFileInfo fileInfo = libraryModel->fileInfo(index);
    if(fileInfo.isFile())
    {
        String filePathname = PathnameToDAVAStyle(fileInfo.filePath());
        QMenu menu;

        String extension = FileSystem::Instance()->GetExtension(filePathname);
        if(0 == CompareCaseInsensitive(String(".sc2"), extension))
        {
            AddActionToMenu(&menu, QString("Add"), new CommandAddScene(filePathname));
            AddActionToMenu(&menu, QString("Edit"), new CommandEditScene(filePathname));
            AddActionToMenu(&menu, QString("Add reference"), new CommandAddReferenceScene(filePathname));
            AddActionToMenu(&menu, QString("Reload"), new CommandReloadScene(filePathname));
        }
        else if(0 == CompareCaseInsensitive(String(".dae"), extension))
        {
            AddActionToMenu(&menu, QString("Convert"), new CommandConvertScene(filePathname));
        }

        connect(&menu, SIGNAL(triggered(QAction *)), this, SLOT(LibraryMenuTriggered(QAction *)));
        menu.exec(point);
    }
}
*/

void SceneData::ReloadRootNode(const DAVA::String &scenePathname)
{
	SelectNode(NULL);

    scene->ReleaseRootNode(scenePathname);
    
    ReloadNode(scene, scenePathname);
    
    for (int32 i = 0; i < (int32)nodesToAdd.size(); i++)
    {
        scene->ReleaseUserData(nodesToAdd[i].nodeToRemove);
        nodesToAdd[i].parent->RemoveNode(nodesToAdd[i].nodeToRemove);
        nodesToAdd[i].parent->AddNode(nodesToAdd[i].nodeToAdd);
        SafeRelease(nodesToAdd[i].nodeToAdd);
    }
    nodesToAdd.clear();
    
    
    SceneEditorScreenMain *screen = dynamic_cast<SceneEditorScreenMain *>(UIScreenManager::Instance()->GetScreen());
    if(screen)
    {
        screen->OnReloadRootNodesQt();
    }
    
    RebuildSceneGraph();
	landscapesController->SetScene(scene);
}

void SceneData::ReloadNode(SceneNode *node, const String &nodePathname)
{//если в рут ноды сложить такие же рут ноды то на релоаде все накроет пиздой
    KeyedArchive *customProperties = node->GetCustomProperties();
    if (customProperties->GetString("editor.referenceToOwner", "") == nodePathname)
    {
        SceneNode *newNode = scene->GetRootNode(nodePathname)->Clone();
        newNode->SetLocalTransform(node->GetLocalTransform());
        newNode->GetCustomProperties()->SetString("editor.referenceToOwner", nodePathname);
        newNode->SetSolid(true);
        
        SceneNode *parent = node->GetParent();
        AddedNode addN;
        addN.nodeToAdd = newNode;
        addN.nodeToRemove = node;
        addN.parent = parent;
        
        nodesToAdd.push_back(addN);
        return;
    }
    
    int32 csz = node->GetChildrenCount();
    for (int ci = 0; ci < csz; ++ci)
    {
        SceneNode * child = node->GetChild(ci);
        ReloadNode(child, nodePathname);
    }
}

/*
void SceneData::ReloadLibrary()
{
    libraryModel->Reload();
}
*/

void SceneData::BakeNode(DAVA::SceneNode *node)
{
    if(node->GetSolid())
    {
        node->BakeTransforms();
        return;
    }
    
    for(int32 i = 0; i < node->GetChildrenCount(); ++i)
    {
        BakeNode(node->GetChild(i));
    }
}

void SceneData::RemoveIdentityNodes(DAVA::SceneNode *node)
{
    for(int32 i = 0; i < node->GetChildrenCount(); ++i)
    {
        SceneNode *removedChild = node->GetChild(i);
        
        if(
           (removedChild->GetFlags() & SceneNode::NODE_LOCAL_MATRIX_IDENTITY)
           &&   (typeid(SceneNode) == typeid(*removedChild))
           &&   (typeid(LodNode) != typeid(*node))
           &&   (removedChild->GetChildrenCount() == 1))
        {
            //SceneNode *child = SafeRetain(removedChild->GetChild(0));
            //removedChild->RemoveNode(child);
            node->AddNode(removedChild->GetChild(0));
            //SafeRelease(child);
            
            node->RemoveNode(removedChild);
            
            i = -1;
        }
        else
        {
            RemoveIdentityNodes(removedChild);
        }
    }
}

void SceneData::FindIdentityNodes(DAVA::SceneNode *node)
{
    for(int32 i = 0; i < node->GetChildrenCount(); ++i)
    {
        SceneNode *child = node->GetChild(i);
        
        if(child->GetSolid())
        {
            RemoveIdentityNodes(child);
        }
        else
        {
            FindIdentityNodes(child);
        }
    }
}


void SceneData::BakeScene()
{
    if(scene)
    {
        SelectNode(NULL);
        
        BakeNode(scene);
        FindIdentityNodes(scene);
        
        RebuildSceneGraph();
    }
}
/*
void SceneData::LibraryContextMenuRequested(const QPoint &point)
{
    QModelIndex itemIndex = libraryView->indexAt(point);
    ShowLibraryMenu(itemIndex, QCursor::pos());
}
*/

void SceneData::ProcessContextMenuAction(QAction *action)
{
    Command *command = PointerHolder<Command *>::ToPointer(action->data());
    Execute(command);
}
/*
void SceneData::LibraryMenuTriggered(QAction *action)
{
    ProcessContextMenuAction(action);
    
    SceneEditorScreenMain *screen = dynamic_cast<SceneEditorScreenMain *>(UIScreenManager::Instance()->GetScreen());
    if(screen)
    {
        screen->HideScenePreview();
    }
}

void SceneData::FileSelected(const QString &filePathname, bool isFile)
{
    //TODO: need best way to display scene preview
    SceneEditorScreenMain *screen = dynamic_cast<SceneEditorScreenMain *>(UIScreenManager::Instance()->GetScreen());
    if(screen && !skipLibraryPreview)
    {
        String extension = FileSystem::Instance()->GetExtension(QSTRING_TO_DAVASTRING(filePathname));
        if(0 == CompareCaseInsensitive(extension, String(".sc2")) && isFile)
        {
            screen->ShowScenePreview(PathnameToDAVAStyle(filePathname));
        }
        else
        {
            screen->HideScenePreview();
        }
    }
    
    skipLibraryPreview = false;
}
*/

void SceneData::Execute(Command *command)
{
    CommandsManager::Instance()->Execute(command);
    SafeRelease(command);
}


void SceneData::SceneGraphMenuTriggered(QAction *action)
{
    ProcessContextMenuAction(action);
}

void SceneData::SceneGraphContextMenuRequested(const QPoint &point)
{
    QModelIndex itemIndex = sceneGraphView->indexAt(point);
    ShowSceneGraphMenu(itemIndex, QCursor::pos());
}

void SceneData::ShowSceneGraphMenu(const QModelIndex &index, const QPoint &point)
{
    if(!index.isValid())
    {
        return;
    }
    
    QMenu menu;
    
    AddActionToMenu(&menu, QString("Remove Root Nodes"), new CommandRemoveRootNodes());
    AddActionToMenu(&menu, QString("Look at Object"), new CommandLockAtObject());
    AddActionToMenu(&menu, QString("Remove Object"), new CommandRemoveSceneNode());

    AddActionToMenu(&menu, QString("Debug Flags"), new CommandDebugFlags());
    AddActionToMenu(&menu, QString("Bake Matrices"), new CommandBakeMatrixes());
    AddActionToMenu(&menu, QString("Build Quad Tree"), new CommandBuildQuadTree());
    
	SceneNode *node = static_cast<SceneNode *>(sceneGraphModel->ItemData(index));
	if(node)
	{
		KeyedArchive *properties = node->GetCustomProperties();
		if(properties && properties->IsKeyExists(String("editor.referenceToOwner")))
		{
            
			String filePathname = properties->GetString(String("editor.referenceToOwner"));
            AddActionToMenu(&menu, QString("Edit Model"), new CommandEditScene(filePathname));
            AddActionToMenu(&menu, QString("Reload Model"), new CommandReloadScene(filePathname));
		}
	}
    
    connect(&menu, SIGNAL(triggered(QAction *)), this, SLOT(SceneGraphMenuTriggered(QAction *)));
    menu.exec(point);
}

void SceneData::AddActionToMenu(QMenu *menu, const QString &actionTitle, Command *command)
{
    QAction *action = menu->addAction(actionTitle);
    action->setData(PointerHolder<Command *>::ToQVariant(command));
}



void SceneData::ToggleNotPassableLandscape()
{
    UIScreen *screen = UIScreenManager::Instance()->GetScreen();
    if (IsPointerToExactClass<SceneEditorScreenMain>(screen))
    {
        if(((SceneEditorScreenMain *)screen)->TileMaskEditorEnabled())
        {
            return;
        }

    }
    
    landscapesController->ToggleNotPassableLandscape();
    RebuildSceneGraph();
}


bool SceneData::CanSaveScene()
{
    SceneEditorScreenMain *screen = dynamic_cast<SceneEditorScreenMain *>(UIScreenManager::Instance()->GetScreen());
    if(!screen->SaveIsAvailable())
    {
        return false;
    }
    
    if(landscapesController->EditorLandscapeIsActive())
    {
        return false;
    }
    
    
    return true;
}


LandscapesController * SceneData::GetLandscapesController()
{
    return landscapesController;
}

/*
void SceneData::OpenLibraryForFile(const DAVA::String &filePathname)
{
    skipLibraryPreview = true;
	bool fileWillBeSelected = libraryModel->SelectFile(filePathname);
	if(!fileWillBeSelected)
	{
		skipLibraryPreview = false;
	}
}
*/

void SceneData::ResetLandsacpeSelection()
{
	LandscapeNode *selectedNode = dynamic_cast<LandscapeNode *>	(GetSelectedNode());
	if(selectedNode)
	{
		SelectNode(NULL);
	}
}


void SceneData::RestoreTexture(const DAVA::String &descriptorPathname, DAVA::Texture *texture)
{
	//materials
	Vector<Material*> materials;
	scene->GetDataNodes(materials);
	for(int32 m = 0; m < (int32)materials.size(); ++m)
	{
		for(int32 t = 0; t < Material::TEXTURE_COUNT; ++t)
		{
			if(materials[m]->GetTextureName((Material::eTextureLevel)t) == descriptorPathname)
			{
				materials[m]->SetTexture((Material::eTextureLevel)t, texture);
			}
		}
	}

	//landscapes
	Vector<LandscapeNode *> landscapes;
	scene->GetChildNodes(landscapes);
	for(int32 l = 0; l < (int32)landscapes.size(); ++l)
	{
		for(int32 t = 0; t < LandscapeNode::TEXTURE_COUNT; t++)
		{
			if(landscapes[l]->GetTextureName((LandscapeNode::eTextureLevel)t) == descriptorPathname)
			{
				landscapes[l]->SetTexture((LandscapeNode::eTextureLevel)t, texture);
			}
		}
	}

	//lightmaps
	Vector<MeshInstanceNode *> meshInstances;
	scene->GetChildNodes(meshInstances);
	for(int32 m = 0; m < (int32)meshInstances.size(); ++m)
	{
		for (int32 li = 0; li < meshInstances[m]->GetLightmapCount(); ++li)
		{
			MeshInstanceNode::LightmapData * ld = meshInstances[m]->GetLightmapDataForIndex(li);
			if (ld)
			{
				SafeRelease(ld->lightmap);
				ld->lightmap = SafeRetain(texture);
			}
		}
	}
}

