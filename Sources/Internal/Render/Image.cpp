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
#include "Render/Image.h"
#include "Render/Texture.h"

namespace DAVA 
{
    

Image::Image()
:	data(0)
,   dataSize(0)
,	width(0)
,	height(0)
,	format(FORMAT_RGB565)
{
}

Image::~Image()
{
	SafeDeleteArray(data);
}

Image * Image::Create(int32 width, int32 height, PixelFormat format)
{
	Image * image = new Image();
	image->width = width;
	image->height = height;
	image->format = format;
    
    int32 formatSize = Texture::GetPixelFormatSizeInBytes(format);
    if(formatSize)
    {
        image->dataSize = width * height * formatSize;
        image->data = new uint8[image->dataSize];
    }
    else 
    {
        Logger::Error("Image::Create trying to create image with wrong format");
    }
    
	return image;
}


  
#if defined(__DAVAENGINE_WIN32__) || defined(__DAVAENGINE_MACOS__) || defined(__DAVAENGINE_IPHONE__) || defined(__DAVAENGINE_ANDROID__)

#else //other platforms

#endif //PLATFORMS	


void Image::ResizeImage(int32 newWidth, int32 newHeight)
{
	if(newWidth>0 && newHeight>0)
	{
		uint8 * newData = NULL;
		uint8 formatSize = Texture::GetPixelFormatSizeInBytes(format);

		if(formatSize>0)
		{
			newData = new uint8[newWidth * newHeight * formatSize];
			memset(newData, 0, newWidth * newHeight * formatSize);

			float32 kx = (float32)width / (float32)newWidth;
			float32 ky = (float32)height / (float32)newHeight;

			float32 xx = 0, yy = 0;
			int32 offset = 0;
			int32 offsetOld = 0;
			int32 posX, posY;
			for (int32 y = 0; y < newHeight; ++y)
			{
				for (int32 x = 0; x < newWidth; ++x)
				{
					posX = (int32)(xx + 0.5f);
					posY = (int32)(yy + 0.5f);
					if (posX >= width)
						posX = width - 1;

					if (posY >= height)
						posY = height - 1;


					offsetOld = (posY * width + posX) * formatSize;
					memcpy(newData + offset, data + offsetOld, formatSize);

					xx += kx;
					offset += formatSize;
				}
				yy += ky;
				xx = 0;
			}

			// resized data
			width = newWidth;
			height = newHeight;
			SafeDeleteArray(data);
			data = newData;
		}
	}

}

void Image::ResizeCanvas(int32 newWidth, int32 newHeight)
{
    if(newWidth>0 && newHeight>0)
    {
        uint8 * newData = NULL;
        uint32 newDataSize = 0;
        uint8 formatSize = Texture::GetPixelFormatSizeInBytes(format);
        
        if(formatSize>0)
        {
            newDataSize = newWidth * newHeight * formatSize;
            newData = new uint8[newDataSize];
            
            uint32 currentLine = 0;
            uint32 indexOnLine = 0;
            uint32 indexInOldData = 0;
            
            for(uint32 i = 0; i < newDataSize; ++i)
            {
                if((currentLine+1)*newWidth*formatSize<=i)
                {
                    currentLine++;
                }
                
                indexOnLine = i - currentLine*newWidth*formatSize;
                
                if(currentLine<(uint32)height)
                {
                    // within height of old image
                    if(indexOnLine<(uint32)(width*formatSize))
                    {
                        // we have data in old image for new image
                        indexInOldData = currentLine*width*formatSize + indexOnLine;
                        newData[i] = data[indexInOldData];
                    }
                    else
                    {
                        newData[i] = 0;
                    }
                }
                else
                {
                    newData[i] = 0;
                }
            }
            
            // resized data
            width = newWidth;
            height = newHeight;
            
            SafeDeleteArray(data);
            data = newData;
            dataSize = newDataSize;
        }
    }
}


};
