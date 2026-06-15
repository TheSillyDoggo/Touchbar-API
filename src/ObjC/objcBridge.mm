#include "../objcBridge.hpp"

#import <Foundation/Foundation.h>
#import <AppKit/NSTouchBar.h>

#include "TouchBarCustomView.h"

TouchBarDelegate* g_TouchBarDelegate = NULL;

namespace touchbar_api
{
    void setupTouchbar()
    {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (!g_TouchBarDelegate)
            {
                g_TouchBarDelegate = [[TouchBarDelegate alloc] init];
                [NSApplication sharedApplication].automaticCustomizeTouchBarMenuItemEnabled = YES;
            }

            NSTouchBar* touchBar = [g_TouchBarDelegate makeTouchBar];
            
            NSArray<NSWindow*>* windows = [NSApplication sharedApplication].windows;
            for (int i = 0; i < windows.count; ++i)
            {
                NSWindow* wnd = windows[i];
                wnd.touchBar = touchBar;
            }
        });
    }

    void dirtifyTouchbar()
    {
        if (g_TouchBarDelegate) {
            dispatch_async(dispatch_get_main_queue(), ^{
                NSArray<NSWindow*>* windows = [NSApplication sharedApplication].windows;
                for (NSWindow* wnd in windows)
                {
                    NSTouchBar* bar = wnd.touchBar;
                    if (bar)
                    {
                        NSCustomTouchBarItem* item = (NSCustomTouchBarItem*)[bar itemForIdentifier:touchBarItemId];
                        if (item && item.view)
                        {
                            [item.view setNeedsDisplay:YES];
                        }
                    }
                }
            });
        }
    }
}