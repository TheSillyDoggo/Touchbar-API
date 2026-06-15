#pragma once

#import <Foundation/Foundation.h>
#import <AppKit/NSTouchBar.h>

@interface TouchBarDelegate : NSObject <NSTouchBarDelegate>
    - (NSTouchBar *)makeTouchBar;
    - (NSTouchBarItem *)touchBar:(NSTouchBar *)touchBar makeItemForIdentifier:(NSTouchBarItemIdentifier)identifier;
@end