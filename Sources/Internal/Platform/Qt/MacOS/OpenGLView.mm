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
#import "OpenGLView.h"
#include "DAVAEngine.h"


@implementation OpenGLView
@synthesize willQuit;

-(id) initWithFrame: (NSRect) frameRect
{
	NSLog(@"[CoreMacOSPlatform] OpenGLView Init");
	
#ifdef __DAVAENGINE_MACOS_VERSION_10_6__
	NSLog(@"Display bpp: %d", [self displayBitsPerPixel:kCGDirectMainDisplay]);
#else //#ifdef __DAVAENGINE_MACOS_VERSION_10_6__
	NSLog(@"Display bpp: %d", CGDisplayBitsPerPixel(kCGDirectMainDisplay));
#endif //#ifdef __DAVAENGINE_MACOS_VERSION_10_6__
	

    // Pixel Format Attributes for the View-based (non-FullScreen) NSOpenGLContext
    NSOpenGLPixelFormatAttribute attrs[] = 
	{
		
        // Specifying "NoRecovery" gives us a context that cannot fall back to the software renderer.  This makes the View-based context a compatible with the fullscreen context, enabling us to use the "shareContext" feature to share textures, display lists, and other OpenGL objects between the two.
        NSOpenGLPFANoRecovery,
		
        // Attributes Common to FullScreen and non-FullScreen
#ifdef __DAVAENGINE_MACOS_VERSION_10_6__
        NSOpenGLPFAColorSize, [self displayBitsPerPixel:kCGDirectMainDisplay],//24,
#else //#ifdef __DAVAENGINE_MACOS_VERSION_10_6__
        NSOpenGLPFAColorSize, CGDisplayBitsPerPixel(kCGDirectMainDisplay),//24,
#endif //#ifdef __DAVAENGINE_MACOS_VERSION_10_6__
        NSOpenGLPFADepthSize, 16,
        NSOpenGLPFAStencilSize, 8,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccelerated,
        0
    };
    GLint rendererID;
	
    // Create our non-FullScreen pixel format.
    NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
	
    // Just as a diagnostic, report the renderer ID that this pixel format binds to.  CGLRenderers.h contains a list of known renderers and their corresponding RendererID codes.
    [pixelFormat getValues:&rendererID forAttribute:NSOpenGLPFARendererID forVirtualScreen:0];
    NSLog(@"[CoreMacOSPlatform] NSOpenGLView pixelFormat RendererID = %08x", (unsigned)rendererID);
	
    self = [super initWithFrame:frameRect pixelFormat:pixelFormat];
	if (self)
	{

	}
	trackingArea = nil;
	[self enableTrackingArea];
	isFirstDraw = true;

	// enable vsync
	GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
	
//	activeCursor = 0;
    
    //RenderManager::Create(Core::RENDERER_OPENGL);
	
    willQuit = false;
    
	return self;	
}

#ifdef __DAVAENGINE_MACOS_VERSION_10_6__
- (size_t) displayBitsPerPixel:(CGDirectDisplayID) displayId 
{
    
	CGDisplayModeRef mode = CGDisplayCopyDisplayMode(displayId);
	size_t depth = 0;
    
	CFStringRef pixEnc = CGDisplayModeCopyPixelEncoding(mode);
	if(CFStringCompare(pixEnc, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
		depth = 32;
	else if(CFStringCompare(pixEnc, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
		depth = 16;
	else if(CFStringCompare(pixEnc, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
		depth = 8;
    
	return depth;
}
#endif //#ifdef __DAVAENGINE_MACOS_VERSION_10_6__


- (void) enableTrackingArea
{
	[trackingArea release];
	trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds] options:(NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveInKeyWindow) owner:self userInfo:nil];
	[self addTrackingArea:trackingArea];
}

- (void) disableTrackingArea
{
	if (trackingArea)
		[self removeTrackingArea: trackingArea];
}

- (void) dealloc
{
	[super dealloc];
}

- (BOOL)isOpaque
{
	return YES;
}

- (void)reshape
{
	NSRect rect = self.frame;
	DAVA::RenderManager::Instance()->Init(rect.size.width, rect.size.height);
	UIControlSystem::Instance()->SetInputScreenAreaSize(rect.size.width, rect.size.height);
	Core::Instance()->SetPhysicalScreenSize(rect.size.width, rect.size.height);
	
    isFirstDraw = true;
    
	sizeChanged = YES;
	[super reshape];
}

- (void)userFireTimer: (id)timer
{
	[self setNeedsDisplay:YES];
}

//bool firstLaunch = true;
	

- (void)drawRect:(NSRect)theRect
{
    if(willQuit)
        return;
    
//	Logger::Debug("drawRect started");
	
//	if (activeCursor != RenderManager::Instance()->GetCursor())
//	{
//		activeCursor = RenderManager::Instance()->GetCursor();
//		[[self window] invalidateCursorRectsForView: self];
//	}
	
	
	DAVA::RenderManager::Instance()->Lock();
	
	if (isFirstDraw)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		isFirstDraw = false;
	}	

	
	
	DAVA::Core::Instance()->SystemProcessFrame();

	
    DAVA::RenderManager::Instance()->SetColor(Color::White());
    
    
/*	// This is an optimization.  If the view is being
	// resized, don't do a buffer swap.  The GL content
	// will be updated as part of the window flush anyway.
	// This makes live resize look nicer as the GL view
	// won't get flushed ahead of the window flush.  It also
	// makes live resize faster since we're not flushing twice.
	// Because I want the animtion to continue while resize
	// is happening, I use my own flag rather than calling
	// [self inLiveReize].  For most apps this wouldn't be
	// necessary.
 
	if(!sizeChanged)
	{
		[[self openGLContext] flushBuffer];
	}
	else glFlush();
	sizeChanged = NO; */
    if(DAVA::Core::Instance()->IsActive())
    {
        [[self openGLContext] flushBuffer];
    }
	DAVA::RenderManager::Instance()->Unlock();
//	Logger::Debug("drawRect ended");

}

- (void) resetCursorRects
{
	NSLog(@"OpenGLView resetCursorRects");
	//
//	if (activeCursor)
//	{
//		//activeCursor->MacOSX_Set();
//		NSCursor * cursor = (NSCursor*)activeCursor->GetMacOSXCursor();
//		[self addCursorRect: [self bounds] cursor: cursor];
//	}else {
		[super resetCursorRects];
//	}
}

-(void)cursorUpdate:(NSEvent *)theEvent
{
	NSLog(@"OpenGLView  Cursor update");
}

- (BOOL)acceptsFirstResponder
{
	return YES;
}

- (BOOL)becomeFirstResponder
{
	return YES;
}

- (BOOL)resignFirstResponder
{	
	return YES;
}

static Vector<DAVA::UIEvent> activeTouches;
void MoveTouchsToVector(NSEvent *curEvent, int touchPhase, Vector<UIEvent> *outTouches)
{
	int button = 0;
	if(curEvent.type == NSLeftMouseDown || curEvent.type == NSLeftMouseUp || curEvent.type == NSLeftMouseDragged || curEvent.type == NSMouseMoved)
	{
		button = 1;
	}
	else if(curEvent.type == NSRightMouseDown || curEvent.type == NSRightMouseUp || curEvent.type == NSRightMouseDragged)
	{
		button = 2;
	}
	else 
	{
		button = curEvent.buttonNumber + 1;
	}

//	NSLog(@"Event button %d", button);
	
	int phase = UIEvent::PHASE_MOVE;
	if(curEvent.type == NSLeftMouseDown || curEvent.type == NSRightMouseDown || curEvent.type == NSOtherMouseDown)
	{
		phase = UIEvent::PHASE_BEGAN;
//		NSLog(@"Event phase PHASE_BEGAN");
	}
	else if(curEvent.type == NSLeftMouseUp || curEvent.type == NSRightMouseUp || curEvent.type == NSOtherMouseUp)
	{
		phase = UIEvent::PHASE_ENDED;
//		NSLog(@"Event phase PHASE_ENDED");
	}
	else if(curEvent.type == NSLeftMouseDragged || curEvent.type == NSRightMouseDragged || curEvent.type == NSOtherMouseDragged)
	{
		phase = UIEvent::PHASE_DRAG;
	}
	
	if(phase == UIEvent::PHASE_DRAG)
	{
		for(Vector<DAVA::UIEvent>::iterator it = activeTouches.begin(); it != activeTouches.end(); it++)
		{
				NSPoint p = [curEvent locationInWindow];
            NSWindow *window = [curEvent window];
            NSRect r = [window contentRectForFrameRect:[window frame]];
            CGFloat height = r.size.height;
            
				it->physPoint.x = p.x;
//				it->physPoint.y = Core::Instance()->GetPhysicalScreenHeight() - p.y;
                it->physPoint.y = height - p.y;

				it->timestamp = curEvent.timestamp;
				it->tapCount = curEvent.clickCount;
				it->phase = phase;
		}
	}
	
	bool isFind = false;
	for(Vector<DAVA::UIEvent>::iterator it = activeTouches.begin(); it != activeTouches.end(); it++)
	{
		if(it->tid == button)
		{
			isFind = true;
			
			NSPoint p = [curEvent locationInWindow];
            NSWindow *window = [curEvent window];
            NSRect r = [window contentRectForFrameRect:[window frame]];
            CGFloat height = r.size.height;
            
			it->physPoint.x = p.x;
//			it->physPoint.y = Core::Instance()->GetPhysicalScreenHeight() - p.y;
            it->physPoint.y = height - p.y;

			it->timestamp = curEvent.timestamp;
			it->tapCount = curEvent.clickCount;
			it->phase = phase;

			break;
		}
	}
	
	if(!isFind)
	{
		UIEvent newTouch;
		newTouch.tid = button;
		NSPoint p = [curEvent locationInWindow];
        
        NSWindow *window = [curEvent window];
        NSRect r = [window contentRectForFrameRect:[window frame]];
        CGFloat height = r.size.height;
       
		newTouch.physPoint.x = p.x;
//		newTouch.physPoint.y = Core::Instance()->GetPhysicalScreenHeight() - p.y;
        newTouch.physPoint.y = height - p.y;

		newTouch.timestamp = curEvent.timestamp;
		newTouch.tapCount = curEvent.clickCount;
		newTouch.phase = phase;
		activeTouches.push_back(newTouch);
	}

	for(Vector<DAVA::UIEvent>::iterator it = activeTouches.begin(); it != activeTouches.end(); it++)
	{
		outTouches->push_back(*it);
	}

	if(phase == UIEvent::PHASE_ENDED || phase == UIEvent::PHASE_MOVE)
	{
		for(Vector<DAVA::UIEvent>::iterator it = activeTouches.begin(); it != activeTouches.end(); it++)
		{
			if(it->tid == button)
			{
				activeTouches.erase(it);
				break;
			}
		}
	}
	
}


-(void)process:(int)touchPhase touch:(NSEvent*)touch
{
	Vector<DAVA::UIEvent> touches;
	Vector<DAVA::UIEvent> emptyTouches;
	MoveTouchsToVector(touch, touchPhase, &touches);
//	NSLog(@"----- Touches --------");
//	for(int i = 0; i < touches.size(); i++)
//	{
//		NSLog(@"Button %d       phase %d", touches[i].buttonID, touches[i].phase);
//	}
//	NSLog(@"----- ------- --------");
	UIControlSystem::Instance()->OnInput(touchPhase, emptyTouches, touches);
	touches.clear();
}

- (void)mouseDown:(NSEvent *)theEvent
{
	[self process:DAVA::UIEvent::PHASE_BEGAN touch:theEvent];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
	[self process:DAVA::UIEvent::PHASE_MOVE touch:theEvent];
}

- (void)mouseUp:(NSEvent *)theEvent
{
	[self process:DAVA::UIEvent::PHASE_ENDED touch:theEvent];
}

- (void)mouseDragged:(NSEvent *)theEvent
{
	[self process:DAVA::UIEvent::PHASE_ENDED touch:theEvent];
}
- (void)mouseEntered:(NSEvent *)theEvent
{
	NSLog(@"mouse ENTERED");
    if(RenderManager::Instance()->GetCursor())
    {
        if(RenderManager::Instance()->GetCursor()->IsShow())
            [NSCursor unhide];
        else
            [NSCursor hide];
    }
//	[self process:DAVA::UIEvent::PHASE_ENDED touch:theEvent];
}
- (void)mouseExited:(NSEvent *)theEvent
{
	NSLog(@"mouse EXITED");
    [NSCursor unhide];
//	[self process:DAVA::UIEvent::PHASE_ENDED touch:theEvent];
}
- (void)rightMouseDown:(NSEvent *)theEvent
{
	[self process:DAVA::UIEvent::PHASE_ENDED touch:theEvent];
}
- (void)rightMouseDragged:(NSEvent *)theEvent
{
	[self process:DAVA::UIEvent::PHASE_ENDED touch:theEvent];
}
- (void)rightMouseUp:(NSEvent *)theEvent
{
	[self process:DAVA::UIEvent::PHASE_ENDED touch:theEvent];
}
- (void)otherMouseDown:(NSEvent *)theEvent
{
	[self process:DAVA::UIEvent::PHASE_ENDED touch:theEvent];
}
- (void)otherMouseDragged:(NSEvent *)theEvent
{
	[self process:DAVA::UIEvent::PHASE_ENDED touch:theEvent];
}
- (void)otherMouseUp:(NSEvent *)theEvent
{
	[self process:DAVA::UIEvent::PHASE_ENDED touch:theEvent];
}

static int32 oldModifersFlags = 0;
- (void) keyDown:(NSEvent *)event
{
	{
		Logger::Debug("glview keypress!");
		unichar c = [[event characters] characterAtIndex:0];
		
		Vector<DAVA::UIEvent> touches;
		Vector<DAVA::UIEvent> emptyTouches;

		for(Vector<DAVA::UIEvent>::iterator it = activeTouches.begin(); it != activeTouches.end(); it++)
		{
			touches.push_back(*it);
		}
		
		DAVA::UIEvent ev;
		ev.keyChar = c;
		ev.phase = DAVA::UIEvent::PHASE_KEYCHAR;
		ev.timestamp = event.timestamp;
		ev.tapCount = 1;
		ev.tid = InputSystem::Instance()->GetKeyboard()->GetDavaKeyForSystemKey([event keyCode]);
        
        touches.push_back(ev);
		
		UIControlSystem::Instance()->OnInput(0, emptyTouches, touches);
        touches.pop_back();
		UIControlSystem::Instance()->OnInput(0, emptyTouches, touches);
	}
	
    InputSystem::Instance()->GetKeyboard()->OnSystemKeyPressed([event keyCode]);
    if ([event modifierFlags]&NSCommandKeyMask)
    {
        InputSystem::Instance()->GetKeyboard()->OnSystemKeyUnpressed([event keyCode]);
    }

//NSLog(@"key Down View");
//	unichar c = [[event charactersIgnoringModifiers] characterAtIndex:0];
//	
//	if ([event modifierFlags] & NSCommandKeyMask)
//	{
//		if (c == 'f')
//		{
//			NSLog(@"[CoreMacOSPlatform] Switch screen mode");
//			if (Core::Instance()->GetScreenMode() == Core::MODE_WINDOWED)
//			{
//				Core::Instance()->SwitchScreenToMode(Core::MODE_FULLSCREEN);
//			}else 
//			{	
//				Core::Instance()->SwitchScreenToMode(Core::MODE_WINDOWED);
//			}
//		}
//	}

}	

- (void) keyUp:(NSEvent *)event
{
    InputSystem::Instance()->GetKeyboard()->OnSystemKeyUnpressed([event keyCode]);
}

- (void) flagsChanged :(NSEvent *)event
{
    int32 newModifers = [event modifierFlags];
    static int32 masks[] = {NSAlphaShiftKeyMask, NSShiftKeyMask, NSControlKeyMask, NSAlternateKeyMask, NSCommandKeyMask};
    static int32 keyCodes[] = {DVMACOS_CAPS_LOCK, DVMACOS_SHIFT, DVMACOS_CONTROL, DVMACOS_OPTION, DVMACOS_COMMAND};

    for (int i = 0; i < 5; i++) 
    {
        if ((oldModifersFlags&masks[i]) != (newModifers&masks[i]))
        {
            if (newModifers&masks[i]) 
            {
                InputSystem::Instance()->GetKeyboard()->OnSystemKeyPressed(keyCodes[i]);
            }
            else 
            {
                InputSystem::Instance()->GetKeyboard()->OnSystemKeyUnpressed(keyCodes[i]);
            }
        }
    }
    
    
    oldModifersFlags = newModifers;
}


@end