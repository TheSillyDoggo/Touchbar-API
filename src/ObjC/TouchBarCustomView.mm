#include "TouchBarCustomView.h"
#include "../objcBridge.hpp"

@implementation TouchBarCustomView
- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:frameRect];
    if (self) {
        self.allowedTouchTypes = NSTouchTypeMaskDirect;
    }
    return self;
}

- (void)touchesBeganWithEvent:(NSEvent *)event {
    NSSet<NSTouch *> *touches = [event touchesMatchingPhase:NSTouchPhaseBegan inView:self];
    for (NSTouch *touch in touches) {
        NSPoint location = [touch locationInView:self];
        touchbar_api::touchbarHandleTouchBegan((long)touch.identity, location.x, location.y);
    }
}

- (void)touchesMovedWithEvent:(NSEvent *)event {
    NSSet<NSTouch *> *touches = [event touchesMatchingPhase:NSTouchPhaseMoved inView:self];
    for (NSTouch *touch in touches) {
        NSPoint location = [touch locationInView:self];
        touchbar_api::touchbarHandleTouchMoved((long)touch.identity, location.x, location.y);
    }
}

- (void)touchesEndedWithEvent:(NSEvent *)event {
    NSSet<NSTouch *> *touches = [event touchesMatchingPhase:NSTouchPhaseEnded inView:self];
    for (NSTouch *touch in touches) {
        NSPoint location = [touch locationInView:self];
        touchbar_api::touchbarHandleTouchEnded((long)touch.identity, location.x, location.y);
    }
}

- (void)touchesCancelledWithEvent:(NSEvent *)event {
    NSSet<NSTouch *> *touches = [event touchesMatchingPhase:NSTouchPhaseCancelled inView:self];
    for (NSTouch *touch in touches) {
        NSPoint location = [touch locationInView:self];
        touchbar_api::touchbarHandleTouchCancelled((long)touch.identity, location.x, location.y);
    }
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    [[NSColor blackColor] setFill];
    NSRectFill(self.bounds);

    touchbar_api::updateSize(self.bounds.size.width, self.bounds.size.height);

    int width = touchbarTexWidth;
    int height = touchbarTexHeight;

    if (pixels.empty() || width <= 0 || height <= 0)
    {
        return;
    }

    size_t totalBytes = pixels.size();
    
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, pixels.data(), totalBytes, NULL);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    CGBitmapInfo bitmapInfo = kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big;

    CGImageRef cgImage = CGImageCreate(
        width, height,
        8,                      // Bits per component
        32,                     // Bits per pixel
        totalBytes / height,    // Bytes per row
        colorSpace, 
        bitmapInfo,
        provider, 
        NULL, 
        true, 
        kCGRenderingIntentDefault
    );

    if (cgImage)
    {
        NSGraphicsContext *nsContext = [NSGraphicsContext currentContext];
        CGContextRef cgContext = [nsContext CGContext];
        
        CGContextDrawImage(cgContext, self.bounds, cgImage);
        
        CGImageRelease(cgImage);
    }

    CGColorSpaceRelease(colorSpace);
    CGDataProviderRelease(provider);
}

@end