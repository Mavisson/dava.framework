/*==================================================================================
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the DAVA Consulting, LLC nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE DAVA CONSULTING, LLC AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL DAVA CONSULTING, LLC BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Revision History:
        * Created by Alexey 'Hottych' Prosin
=====================================================================================*/

#include "Scene3D/SpriteNode.h"
#include "Scene3D/Scene.h"
#include "Render/RenderManager.h"
#include "Render/2D/Sprite.h"
namespace DAVA 
{

SpriteNode::SpriteNode(Scene * _scene, const String &pathToSprite, int32 _frame
                       , const Vector2 &reqScale, const Vector2 &pivotPoint)
:   SceneNode(_scene)
{
    sprite = Sprite::Create(pathToSprite);
    sprScale = reqScale;
    sprPivot = pivotPoint;
    frame = _frame;
    CreateMeshFromSprite();
}

SpriteNode::SpriteNode(Scene * _scene, Sprite *spr, int32 _frame
           , const Vector2 &reqScale, const Vector2 &pivotPoint)
:   SceneNode(_scene)
{
    sprScale = reqScale;
    sprPivot = pivotPoint;
    sprite = SafeRetain(spr);
    frame = _frame;
    CreateMeshFromSprite();
}

SpriteNode::~SpriteNode()
{
    SafeRelease(sprite);
}

void SpriteNode::SetFrame(int32 newFrame)
{
    if (frame != newFrame) 
    {
        frame = newFrame;
        CreateMeshFromSprite();
    }
}

int32 SpriteNode::GetFrame()
{
    return frame;
}


void SpriteNode::CreateMeshFromSprite()
{//TODO: Add precreation of the all sprite frames
    float32 x0 = sprite->GetRectOffsetValueForFrame(frame, Sprite::X_OFFSET_TO_ACTIVE) - sprPivot.x;
    float32 y0 = sprite->GetRectOffsetValueForFrame(frame, Sprite::Y_OFFSET_TO_ACTIVE) - sprPivot.y;
    float32 x1 = x0 + sprite->GetRectOffsetValueForFrame(frame, Sprite::ACTIVE_WIDTH);
    float32 y1 = y0 + sprite->GetRectOffsetValueForFrame(frame, Sprite::ACTIVE_HEIGHT);
    x0 *= sprScale.x;
    x1 *= sprScale.y;
    y0 *= sprScale.x;
    y1 *= sprScale.y;

        //triangle 1
        //0, 0
    verts.push_back(x0);
    verts.push_back(0);
    verts.push_back(y0);
    
        //1, 0
    verts.push_back(x1);
    verts.push_back(0);
    verts.push_back(y0);
    
    
        //0, 1
    verts.push_back(x0);
    verts.push_back(0);
    verts.push_back(y1);
    
        //1, 1
    verts.push_back(x1);
    verts.push_back(0);
    verts.push_back(y1);

    
        //			tempVertices[0] = x1;
        //			tempVertices[1] = y1;
        //			tempVertices[2] = x2;
        //			tempVertices[3] = y1;
        //			tempVertices[4] = x1;
        //			tempVertices[5] = y2;
        //			tempVertices[6] = x2;
        //			tempVertices[7] = y2;
    
    
    
}


void SpriteNode::Draw()
{
	if (!visible)return;
    
	Matrix4 prevMatrix = RenderManager::Instance()->GetMatrix(RenderManager::MATRIX_MODELVIEW); 
	Matrix4 meshFinalMatrix = worldTransform * prevMatrix;
    

        //TODO: Add billboards mode
    /*
    {//billboarding
        Vector3 camDir = scene->GetCamera()->GetDirection();
        Vector3 right;
        Vector3 up(0.f,1.f,0.f);
        right = up.CrossProduct(camDir);
        up = camDir.CrossProduct(right);
        up.Normalize();
        right.Normalize();
        
        Matrix4 bm;
        
        bm._data[0][0] = right.x;
        bm._data[0][1] = right.y;
        bm._data[0][2] = right.z;
        bm._data[0][3] = 0.0f;
        
        bm._data[1][0] = up.x;
        bm._data[1][1] = up.y;
        bm._data[1][2] = up.z;
        bm._data[1][3] = 0.0f;
        
        bm._data[2][0] = camDir.x;
        bm._data[2][1] = camDir.y;
        bm._data[2][2] = camDir.z;
        bm._data[2][3] = 0.0f;
        
        bm._data[3][0] = 0.0f;
        bm._data[3][1] = 0.0f;
        bm._data[3][2] = 0.0f;
        bm._data[3][3] = 1.0f;
        
        meshFinalMatrix = meshFinalMatrix * bm;
    }
     */
    
    RenderManager::Instance()->SetMatrix(RenderManager::MATRIX_MODELVIEW, meshFinalMatrix);
    
    
    
    
    
    RenderManager::Instance()->SetColor(1, 1, 1, 1);
    RenderManager::Instance()->EnableBlending(true);
    RenderManager::Instance()->EnableTexturing(true);//TODO: Move all this code to the RenderState node
    RenderManager::Instance()->EnableDepthTest(false);
    RenderManager::Instance()->EnableDepthWrite(false);
    RenderManager::Instance()->SetTexture(sprite->GetTexture(frame));
	RenderManager::Instance()->FlushState();
    
    RenderManager::Instance()->SetVertexPointer(3, TYPE_FLOAT, 0, verts.data());
    RenderManager::Instance()->SetTexCoordPointer(2, TYPE_FLOAT, 0, sprite->GetTextureVerts(frame)); 
//	glColorPointer(4, GL_FLOAT, 0, colors.data());
//	glEnableClientState(GL_COLOR_ARRAY);
    RenderManager::Instance()->DrawArrays(PRIMITIVETYPE_TRIANGLESTRIP, 0, 4);

	
    
//    glDisableClientState(GL_COLOR_ARRAY);
    RenderManager::Instance()->EnableTexturing(true);
    RenderManager::Instance()->EnableBlending(false);
    RenderManager::Instance()->EnableDepthTest(true);
    RenderManager::Instance()->EnableDepthWrite(true);
	
    
    
    RenderManager::Instance()->SetMatrix(RenderManager::MATRIX_MODELVIEW, prevMatrix);
}

};









