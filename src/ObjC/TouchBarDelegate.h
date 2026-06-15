#pragma once

#import <Foundation/Foundation.h>
#import <AppKit/NSTouchBar.h>

static NSString *touchBarCustomizationId = @"com.geometry-jump.touchbar-customization";
static NSString *touchBarItemId = @"com.geometry-jump.touchbar";

@interface TouchBarDelegate : NSObject <NSTouchBarDelegate>
    - (NSTouchBar *)makeTouchBar;
    - (NSTouchBarItem *)touchBar:(NSTouchBar *)touchBar makeItemForIdentifier:(NSTouchBarItemIdentifier)identifier;
@end