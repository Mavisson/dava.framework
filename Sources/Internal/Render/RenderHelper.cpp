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
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
#include "Render/RenderHelper.h"
#include "Render/RenderManager.h"
#include "Render/Texture.h"

namespace DAVA
{
	
void RenderHelper::DrawPoint(const Vector2 & pt, float32 ptSize)
{
	RenderManager::Instance()->EnableTexturing(false);
	RenderManager::Instance()->FlushState();
	glPointSize(ptSize);
	
	glVertexPointer(2, GL_FLOAT, 0, &pt);
	
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_POINTS, 0, 1);
	RenderManager::Instance()->EnableTexturing(true);
	glPointSize(1.0f);
}
	
void RenderHelper::DrawPoint(const Vector3 & pt, float32 ptSize)
{
	RenderManager::Instance()->EnableTexturing(false);
	RenderManager::Instance()->FlushState();
	glPointSize(ptSize);
	
	glVertexPointer(3, GL_FLOAT, 0, &pt);

	
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_POINTS, 0, 1);
	RenderManager::Instance()->EnableTexturing(true);
	glPointSize(1.0f);
}
	
void RenderHelper::DrawCircle(const Vector2 & center, float32 radius)
{
	Vector<Vector2> pts;
	int ptsCount = 15;
	
	for (int k = 0; k < ptsCount; ++k)
	{
		float32 angle = ((float)k / (ptsCount - 1)) * 2 * PI;
		float32 sinA = sinf(angle);
		float32 cosA = cosf(angle);
		Vector2 pos = center - Vector2(sinA * radius, cosA * radius);
		
		pts.push_back(pos);
	}
	
	RenderManager::Instance()->EnableTexturing(false);
	RenderManager::Instance()->FlushState();
	
	glVertexPointer(2, GL_FLOAT, 0, &pts.front());
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_LINE_STRIP, 0, ptsCount);
	RenderManager::Instance()->EnableTexturing(true);	
}

void RenderHelper::DrawCircle(const Vector3 & center, float32 radius)
{
	Vector<Vector3> pts;
	int ptsCount = 15;

	for (int k = 0; k < ptsCount; ++k)
	{
		float32 angle = ((float)k / (ptsCount - 1)) * 2 * PI;
		float32 sinA = sinf(angle);
		float32 cosA = cosf(angle);
		Vector3 pos = center - Vector3(sinA * radius, cosA * radius, 0);

		pts.push_back(pos);
	}

	RenderManager::Instance()->EnableTexturing(false);
	RenderManager::Instance()->FlushState();

	glVertexPointer(3, GL_FLOAT, 0, &pts.front());
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_LINE_STRIP, 0, ptsCount);
	RenderManager::Instance()->EnableTexturing(true);	
	
}

void RenderHelper::DrawPolygonPoints(Polygon2 & polygon, bool closed)
{
	int ptCount = polygon.pointCount;
	//if (closed)ptCount++;
	//	for (int pi = 0; pi < ptCount; ++pi)
	//	{
	//		
	//		
	//		
	//	}
	if (ptCount >= 1)
	{
		RenderManager::Instance()->EnableTexturing(false);
		RenderManager::Instance()->FlushState();
		
		glPointSize(3.0f);
		glVertexPointer(2, GL_FLOAT, 0, polygon.GetPoints());
		glEnableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDrawArrays(GL_POINTS, 0, ptCount);
		RenderManager::Instance()->EnableTexturing(true);
		glPointSize(1.0f);
	}
	
}
	
void RenderHelper::DrawPolygonPoints(Polygon3 & polygon, bool closed)
{
	int ptCount = polygon.pointCount;
	//if (closed)ptCount++;
	
	//	for (int pi = 0; pi < ptCount; ++pi)
	//	{
	//		
	//		
	//		
	//	}
	if (ptCount >= 1)
	{
		//RenderManager_setBlendMode(GL_ONE, GL_ZERO);
		RenderManager::Instance()->EnableTexturing(false);
		RenderManager::Instance()->FlushState();
		
		glPointSize(3.0f);
		glVertexPointer(3, GL_FLOAT, 0, polygon.GetPoints());
		glEnableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDrawArrays(GL_POINTS, 0, ptCount);
		RenderManager::Instance()->EnableTexturing(true);
		glPointSize(1.0f);
	}
	
}
	
void RenderHelper::DrawPolygon(Polygon3 & polygon, bool closed)
{
	int ptCount = polygon.pointCount;
	//if (closed)ptCount++;
	
	//	for (int pi = 0; pi < ptCount; ++pi)
	//	{
	//		
	//		
	//		
	//	}
	if (ptCount >= 2)
	{
		//RenderManager_setBlendMode(GL_ONE, GL_ZERO);
		RenderManager::Instance()->EnableTexturing(false);
		RenderManager::Instance()->FlushState();
		
		RenderManager::Instance()->SetVertexPointer(3, TYPE_FLOAT, 0, polygon.GetPoints());
		RenderManager::Instance()->EnableVertexArray(true);
	
		// glEnableClientState(GL_VERTEX_ARRAY);
		// glDisableClientState(GL_COLOR_ARRAY);
		
		if (closed)
		{
			RenderManager::Instance()->DrawArrays(PRIMITIVETYPE_LINESTRIP, 0, ptCount);
			Vector3 line[2] = {Vector3(polygon.GetPoints()[0]), Vector3(polygon.GetPoints()[ptCount-1])};
			RenderManager::Instance()->SetVertexPointer(3, TYPE_FLOAT, 0, line);
			RenderManager::Instance()->DrawArrays(PRIMITIVETYPE_LINESTRIP, 0, 2);
		}else RenderManager::Instance()->DrawArrays(PRIMITIVETYPE_LINESTRIP, 0, ptCount);
		
		RenderManager::Instance()->EnableTexturing(true);
	}
	
}
	
void RenderHelper::DrawPolygon(Polygon2 & polygon, bool closed)
{
	int ptCount = polygon.pointCount;
	//if (closed)ptCount++;
	//	for (int pi = 0; pi < ptCount; ++pi)
	//	{
	//		
	//		
	//		
	//	}
	if (ptCount >= 2)
	{
		//RenderManager_setBlendMode(GL_ONE, GL_ZERO);
		RenderManager::Instance()->EnableTexturing(false);
		RenderManager::Instance()->FlushState();

		RenderManager::Instance()->SetVertexPointer(2, TYPE_FLOAT, 0, polygon.GetPoints());
		RenderManager::Instance()->EnableVertexArray(true);

		// glEnableClientState(GL_VERTEX_ARRAY);
		// glDisableClientState(GL_COLOR_ARRAY);

		if (closed)
		{
			RenderManager::Instance()->DrawArrays(PRIMITIVETYPE_LINESTRIP, 0, ptCount);
			Vector2 line[2] = {Vector2(polygon.GetPoints()[0]), Vector2(polygon.GetPoints()[ptCount-1])};
			RenderManager::Instance()->SetVertexPointer(2, TYPE_FLOAT, 0, line);
			RenderManager::Instance()->DrawArrays(PRIMITIVETYPE_LINESTRIP, 0, 2);
			
			//DrawLine();
		}
		else RenderManager::Instance()->DrawArrays(PRIMITIVETYPE_LINESTRIP, 0, ptCount);

		RenderManager::Instance()->EnableTexturing(true);
	}
	
}
void RenderHelper::DrawPolygonTransformed(Polygon2 & polygon, bool closed, const Matrix3 & transform)
{
	Polygon2 copyPoly = polygon;
	copyPoly.Transform(transform);
	RenderHelper::DrawPolygon(copyPoly, closed);
}

void RenderHelper::DrawLine(const Vector2 & pt1, const Vector2 & pt2)
{
	RenderManager::Instance()->EnableTexturing(false);
	RenderManager::Instance()->FlushState();
	
	Vector2 line[2] = {pt1, pt2};
	
	glVertexPointer(2, GL_FLOAT, 0, line);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_LINES, 0, 2);
	
	RenderManager::Instance()->EnableTexturing(true);
}

void RenderHelper::DrawLine(const Vector3 & pt1, const Vector3 & pt2)
{
	RenderManager::Instance()->EnableTexturing(false);
	RenderManager::Instance()->FlushState();
	
	Vector3 line[2] = {pt1, pt2};
	
	glVertexPointer(3, GL_FLOAT, 0, line);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_LINES, 0, 2);
	
	RenderManager::Instance()->EnableTexturing(true);
}

void RenderHelper::DrawLineWithEndPoints(const Vector3 & pt1, const Vector3 & pt2)
{
	RenderManager::Instance()->EnableTexturing(false);
	RenderManager::Instance()->FlushState();
	
	Vector3 line[2] = {pt1, pt2};

	glVertexPointer(3, GL_FLOAT, 0, line);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_LINES, 0, 2);
	
	glPointSize(5.0f);
	glDrawArrays(GL_POINTS, 0, 2);
	glPointSize(1.0f);
	RenderManager::Instance()->EnableTexturing(true);
}

static Vector2 tmpPolyline[600];	
static Vector2 tmpPolylineTexCoords[600];

void RenderHelper::DrawStrippedLine(Polygon2 & polygon, float lineLen, float spaceLen, float halfWidth, Texture * texture, float initialPos)
{	
//	Vector2 texCoords[4] = 
//	{		
//			Vector2(0.0f, 0.0f)
//		,	Vector2(0.0f, 5.0f / 8.0f)
//		,	Vector2(3.0f / 8.0f, 0.0f)
//		,	Vector2(3.0f / 8.0f, 5.0f / 8.0f)
//	};
	Vector2 texCoords[4] = 
	{		
			Vector2(0.0f, 0.0f)
		,	Vector2(3.0f / 8.0f, 0.0f)
		,	Vector2(0.0f, 8.0f / 8.0f)
		,	Vector2(3.0f / 8.0f, 8.0f / 8.0f)
	};		
	float positionOnPoly = 0.0f;
	Vector2 p1, p2;
	int s1, s2;
	int polyCount = 0;
	while(1)
	{
		polygon.InterpolatePositionFromDistance(positionOnPoly, 0, p1, s1);	
		if (s1 == -1)break;
		polygon.InterpolatePositionFromDistance(positionOnPoly + lineLen, 0, p2, s2);	
		if (s2 == -1)break;
/*
		polygon.InterpolatePositionFromDistanceReverse(positionOnPoly, polygon.pointCount - 1, p1, s1);	
		if (s1 == -1)break;
		polygon.InterpolatePositionFromDistanceReverse(positionOnPoly + lineLen, polygon.pointCount - 1, p2, s2);	
		if (s2 == -1)break;
*/
		
		positionOnPoly += lineLen + spaceLen;
		
		Vector2 dir = p1 - p2;
		Vector2 n(dir.y, -dir.x);
		n.Normalize();
		Vector2 pf0 = p1 + n * halfWidth;
		Vector2 pf1 = p1 - n * halfWidth;
		Vector2 pf2 = p2 + n * halfWidth;
		Vector2 pf3 = p2 - n * halfWidth;
		
		
		tmpPolyline[polyCount * 6 + 0] = pf0;
		tmpPolyline[polyCount * 6 + 1] = pf1;
		tmpPolyline[polyCount * 6 + 2] = pf2;
		
		tmpPolyline[polyCount * 6 + 3] = pf1;
		tmpPolyline[polyCount * 6 + 4] = pf2;
		tmpPolyline[polyCount * 6 + 5] = pf3;
		
		tmpPolylineTexCoords[polyCount * 6 + 0] = texCoords[0];
		tmpPolylineTexCoords[polyCount * 6 + 1] = texCoords[1];
		tmpPolylineTexCoords[polyCount * 6 + 2] = texCoords[2];
		
		tmpPolylineTexCoords[polyCount * 6 + 3] = texCoords[1];
		tmpPolylineTexCoords[polyCount * 6 + 4] = texCoords[2];
		tmpPolylineTexCoords[polyCount * 6 + 5] = texCoords[3];
		
		polyCount++;
		if (polyCount >= 100)break;
	}	
	//RenderManager_disableTexturing();
	//RenderManager_flushState();
	RenderManager::Instance()->SetTexture(texture);
	RenderManager::Instance()->FlushState();
	glVertexPointer(2, GL_FLOAT, 0, tmpPolyline);
	glTexCoordPointer(2, GL_FLOAT, 0, tmpPolylineTexCoords);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDisableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_TRIANGLES, 0, polyCount * 6);
	//RenderManager_enableTexturing();
}
	
void RenderHelper::DrawBSpline(BezierSpline3 * bSpline, int segments, float ts, float te)
{
	// 
	Vector<Vector3> pts;
	for (int k = 0; k < segments; ++k)
	{
		pts.push_back(bSpline->Evaluate(0, ts + (te - ts) * ((float)k / (float)(segments - 1))));
	}
	RenderManager::Instance()->EnableTexturing(false);
	RenderManager::Instance()->FlushState();
	
	glVertexPointer(3, GL_FLOAT, 0, &pts.front());
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_LINE_STRIP, 0, segments);
	RenderManager::Instance()->EnableTexturing(true);	
}
	
void RenderHelper::DrawInterpolationFunc(Interpolation::Func func, const Rect & destRect)
{
	Vector<Vector3> pts;

	int segmentsCount = 20;
	for (int k = 0; k < segmentsCount; ++k)
	{
		Vector3 v;
		v.x = destRect.x + ((float)k / (float)(segmentsCount - 1)) * destRect.dx;
		v.y = destRect.y + func(((float)k / (float)(segmentsCount - 1))) * destRect.dy;
		v.z = 0.0f;
		pts.push_back(v);
	}
	RenderManager::Instance()->EnableTexturing(false);
	RenderManager::Instance()->FlushState();
	
	glVertexPointer(3, GL_FLOAT, 0, &pts.front());
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_LINE_STRIP, 0, segmentsCount);
	RenderManager::Instance()->EnableTexturing(true);	
}
	
void RenderHelper::DrawBox(const AABBox2 & box)
{
	DrawLine(Vector3(box.min.x, box.min.y, 0), Vector3(box.max.x, box.min.y, 0));
	DrawLine(Vector3(box.max.x, box.min.y, 0), Vector3(box.max.x, box.max.y, 0));
	DrawLine(Vector3(box.max.x, box.max.y, 0), Vector3(box.min.x, box.max.y, 0));
	DrawLine(Vector3(box.min.x, box.max.y, 0), Vector3(box.min.x, box.min.y, 0));
}
	
void RenderHelper::DrawBox(const AABBox3 & box)
{
	DrawLine(Vector3(box.min.x, box.min.y, box.min.z), Vector3(box.min.x, box.min.y, box.max.z));
	DrawLine(Vector3(box.min.x, box.min.y, box.min.z), Vector3(box.min.x, box.max.y, box.min.z));
	DrawLine(Vector3(box.min.x, box.max.y, box.max.z), Vector3(box.min.x, box.min.y, box.max.z));
	DrawLine(Vector3(box.min.x, box.max.y, box.max.z), Vector3(box.min.x, box.max.y, box.min.z));
	
	DrawLine(Vector3(box.max.x, box.min.y, box.min.z), Vector3(box.max.x, box.min.y, box.max.z));
	DrawLine(Vector3(box.max.x, box.min.y, box.min.z), Vector3(box.max.x, box.max.y, box.min.z));
	DrawLine(Vector3(box.max.x, box.max.y, box.max.z), Vector3(box.max.x, box.min.y, box.max.z));
	DrawLine(Vector3(box.max.x, box.max.y, box.max.z), Vector3(box.max.x, box.max.y, box.min.z));
	
	
	DrawLine(Vector3(box.min.x, box.min.y, box.min.z), Vector3(box.max.x, box.min.y, box.min.z));
	DrawLine(Vector3(box.min.x, box.max.y, box.min.z), Vector3(box.max.x, box.max.y, box.min.z));
	DrawLine(Vector3(box.min.x, box.min.y, box.max.z), Vector3(box.max.x, box.min.y, box.max.z));
	DrawLine(Vector3(box.min.x, box.max.y, box.max.z), Vector3(box.max.x, box.max.y, box.max.z));
}

	
	
};