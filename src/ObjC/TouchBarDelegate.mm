#include "TouchBarDelegate.h"
#include "TouchBarCustomView.h"

@implementation TouchBarDelegate
    - (NSTouchBar *)makeTouchBar
    {
        NSTouchBar *touchBar = [[NSTouchBar alloc] init];
        touchBar.delegate = self;
        touchBar.customizationIdentifier = touchBarCustomizationId;

        touchBar.defaultItemIdentifiers = @[touchBarItemId, NSTouchBarItemIdentifierOtherItemsProxy];
        touchBar.customizationAllowedItemIdentifiers = @[touchBarItemId];
        touchBar.principalItemIdentifier = touchBarItemId;

        return touchBar;
    }

    - (NSTouchBarItem *)touchBar:(NSTouchBar *)touchBar makeItemForIdentifier:(NSTouchBarItemIdentifier)identifier
    {
        if ([identifier isEqualToString:touchBarItemId])
        {
            NSCustomTouchBarItem* g_TouchBarItem = [[NSCustomTouchBarItem alloc] initWithIdentifier:touchBarItemId];

            TouchBarCustomView *trackingView = [[TouchBarCustomView alloc] initWithFrame:NSMakeRect(0, 0, 80085, 30)];
            /*trackingView.translatesAutoresizingMaskIntoConstraints = true;
            trackingView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

            [trackingView setContentHuggingPriority:NSLayoutPriorityDefaultLow
                forOrientation:NSLayoutConstraintOrientationHorizontal];*/

            g_TouchBarItem.view = trackingView;
            g_TouchBarItem.customizationLabel = NSLocalizedString(@"Do not customize", @"");

            /*
            if (@available(macOS 10.12, *)) {
                [trackingView setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
                [trackingView setContentHuggingPriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
            }*/

            return g_TouchBarItem;
        }

        return nil;
    }
@end