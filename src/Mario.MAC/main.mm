//
//  main.m
//  MarioV3
//
//  Created by neoarc on 2016. 8. 22..
//  Copyright © neoarc. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include <locale.h>

#include "Common.h"
#include "Game.h"
#include "OpenGLManager.h"
#include "FModManager.h"
#include "InputManager.h"

Game* g_pGame = nullptr;
bool g_bKeyMap[256];

#include <OpenGL/gl.h>

@interface NaNSWindowDelegate : NSObject
@end

@implementation NaNSWindowDelegate
- (void)windowDidResize:(NSNotification*)aNotification
{
    if (g_pGame == nullptr)
        return;
    
    if (g_pGame->m_pRenderManager == nullptr)
        return;
    
    NSWindow *nswindow = (__bridge NSWindow*)g_pGame->m_hWindow;
    
    int x, y, w, h;
    NSRect rect = [nswindow contentRectForFrameRect:[nswindow frame]];
    
    x = (int)rect.origin.x;
    y = (int)rect.origin.y;
    w = (int)rect.size.width;
    h = (int)rect.size.height;

    g_pGame->m_pRenderManager->OnSize(w, h);
    
    NSOpenGLContext *context = (__bridge NSOpenGLContext*)((OpenGLManager*)(g_pGame->m_pRenderManager))->m_pContext;
    [context update];
}
- (void)windowWillClose:(NSNotification*)aNotification
{
    if (g_pGame)
        g_pGame->m_bExit = true;
}
@end

int main(int argc, const char * argv[])
{
    //[[NSAutoreleasePool alloc] init];
    
    setlocale(LC_CTYPE, "");
    
    // Init random seed
    srand(time(nullptr));
    
    // Check working directory
    // TODO
    
    Game *pGame = new Game();
    g_pGame = pGame;
    
    // Creates a simple window
    NSRect MainWindowRect = NSMakeRect(0,
                                       0,
                                       GameWindowDefaults::nInnerWidth * GameWindowDefaults::nZoomFactor,
                                       GameWindowDefaults::nInnerHeight * GameWindowDefaults::nZoomFactor
                                       );

    NSWindow* MainWindow = [[NSWindow alloc] initWithContentRect: MainWindowRect
                                                       styleMask: NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
                                                         backing: NSBackingStoreBuffered
                                                           defer: NO];
    [MainWindow setTitle:@"SuperMario v1.3 by neoarc (since 2016.06.26)"];
    [MainWindow orderFrontRegardless];
    
    // for Handling Event
    NaNSWindowDelegate *delegate = [[NaNSWindowDelegate alloc] init];
    MainWindow.delegate = delegate;
    
    if ([MainWindow delegate] != nil)
    {
        NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
        [center addObserver:nil
                   selector:@selector(windowDidResize:)
                       name:NSWindowDidResizeNotification
                     object:MainWindow];
        [center addObserver:nil
                   selector:@selector(windowWillClose:)
                       name:NSWindowWillCloseNotification
                     object:MainWindow];
    }

    // Init OpenGL
    NSOpenGLPixelFormatAttribute attr[] = {
        NSOpenGLPFAColorSize, 24,
        NSOpenGLPFAAlphaSize, 8,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccelerated,
        0
    };
    NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attr];
    NSOpenGLContext *OGLContext = [[NSOpenGLContext alloc] initWithFormat:format shareContext:NULL];
    //[format release];
    
    [OGLContext setView: [MainWindow contentView]];
    [OGLContext makeCurrentContext];
    
    // Creates the application object
    [NSApplication sharedApplication];
    
    // Use BundlePath as CurrentPath
    NSString *nsstr = [[NSBundle mainBundle] resourcePath];
    const char *szBundlePath = [nsstr cStringUsingEncoding:NSUTF8StringEncoding];
    
    NaString strPath = szBundlePath;
    NaStrArray ar = strPath.Split(L"/");
    ar.Pop();
    strPath = ar.Join(L"/");
    strPath += L"/";
    
    // Init Game
    pGame->Init((__bridge void*)MainWindow, strPath);
    
    ((OpenGLManager*)(pGame->m_pRenderManager))->m_pContext = (__bridge void*)OGLContext;
    [delegate windowDidResize:nil];
    
    // Default MessageLoop
    //[NSApp run];
    
    // Custom MessageLoop
    while (g_pGame->m_bExit == false)
    {
        NSEvent *event = [NSApp nextEventMatchingMask: NSAnyEventMask
                                            untilDate: nil
                                               inMode: NSDefaultRunLoopMode
                                              dequeue: YES];
        switch ([(NSEvent*)event type])
        {
            case NSKeyDown:
                g_bKeyMap[[(NSEvent*)event keyCode]] = 1;
                break;
            case NSKeyUp:
                g_bKeyMap[[(NSEvent*)event keyCode]] = 0;
                break;
            case NSMouseMoved:
                if (g_pGame && g_pGame->m_pInputManager)
                {
                    NSPoint globalLocation = [ NSEvent mouseLocation ];
                    NSPoint windowLocation = [MainWindow convertScreenToBase: globalLocation ];
                    NSPoint pt = [[MainWindow contentView] convertPoint: windowLocation fromView: nil ];
                    
                    NSRect frame = [[MainWindow contentView] frame];
                    pt.y = frame.size.height - pt.y;
                    
                    float fScaleX = (float)frame.size.width / (float)(g_pGame->m_nWidth);
                    float fScaleY = (float)frame.size.height / (float)(g_pGame->m_nHeight);
                    
                    POINT ptNew = {
                        (int)((float)pt.x / fScaleX),
                        (int)((float)pt.y / fScaleY)
                    };
                    
                    g_pGame->m_pInputManager->m_ptMouse = ptNew;
                }
                [NSApp sendEvent:event];
                break;
            case NSLeftMouseDown:
                if (g_pGame && g_pGame->m_pInputManager)
                {
                    NSPoint globalLocation = [ NSEvent mouseLocation ];
                    NSPoint windowLocation = [MainWindow convertScreenToBase: globalLocation ];
                    NSPoint pt = [[MainWindow contentView] convertPoint: windowLocation fromView: nil ];
                    
                    NSRect frame = [[MainWindow contentView] frame];
                    pt.y = frame.size.height - pt.y;
                    
                    float fScaleX = (float)frame.size.width / (float)(g_pGame->m_nWidth);
                    float fScaleY = (float)frame.size.height / (float)(g_pGame->m_nHeight);
                    
                    POINT ptNew = {
                        (int)((float)pt.x / fScaleX),
                        (int)((float)pt.y / fScaleY)
                    };
                    
                    g_pGame->m_pInputManager->m_ptLButtonDown = ptNew;
                }
                [NSApp sendEvent:event];
                break;
            case NSLeftMouseUp:
                if (g_pGame && g_pGame->m_pInputManager)
                {
                    NSPoint globalLocation = [ NSEvent mouseLocation ];
                    NSPoint windowLocation = [MainWindow convertScreenToBase: globalLocation ];
                    NSPoint pt = [[MainWindow contentView] convertPoint: windowLocation fromView: nil ];
                    
                    NSRect frame = [[MainWindow contentView] frame];
                    pt.y = frame.size.height - pt.y;
                    
                    float fScaleX = (float)frame.size.width / (float)(g_pGame->m_nWidth);
                    float fScaleY = (float)frame.size.height / (float)(g_pGame->m_nHeight);
                    
                    POINT ptNew = {
                        (int)((float)pt.x / fScaleX),
                        (int)((float)pt.y / fScaleY)
                    };
                    
                    g_pGame->m_pInputManager->m_ptLButtonUp = ptNew;
                    g_pGame->m_pInputManager->OnClick(); 
                }
                [NSApp sendEvent:event];
                break;
            default:
                [NSApp sendEvent:event];
                break;
        }
        //[event release];

        pGame->ProcessGameLoop();
        
        [OGLContext flushBuffer];
        
        // TODO
        // Clear KeyMap
    }
    
    pGame->OnExit();
   
    pGame->Release();
    delete pGame;
    
    return 0;
}
