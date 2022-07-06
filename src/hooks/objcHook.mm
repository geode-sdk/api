#include "hook.hpp"


#if defined(GEODE_IS_MACOS)

#import <Cocoa/Cocoa.h>
#include <DragDropEvent.hpp>
#include <MouseEvent.hpp>

@interface NSView(EAGLView)
 - (float)frameZoomFactor;
 - (float)getHeight;
@end

@implementation NSWindow(GeodeHook)
	- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender {
        NSArray* dragItems = [[sender draggingPasteboard] readObjectsForClasses:[NSArray arrayWithObject:[NSURL class]] options:nil];

        for (auto handler : Event::getHandlers()) {
            if (auto dragHandler = dynamic_cast<DragDropHandler*>(handler)) {
                for (NSURL* dragItem in dragItems) {
                    if (dragHandler->wouldAccept(std::string(dragItem.path.pathExtension.UTF8String))) {
                        return NSDragOperationCopy;
                    }
                }
            }
        }

        return NSDragOperationNone; // real
	}

	- (BOOL)prepareForDragOperation:(id<NSDraggingInfo>)sender {
        return YES;
	}

	-(BOOL)performDragOperation:(id<NSDraggingInfo>)sender {
	    NSArray* dragItems = [[sender draggingPasteboard] readObjectsForClasses:[NSArray arrayWithObject:[NSURL class]] options:nil];
	    
	    for (NSURL* dragItem in dragItems) {
            DragDropEvent(dragItem.path.UTF8String).post();
	    }
	    return NO;
	}

    - (void)mouseDown:(NSEvent*)ev {
        MouseState& state = MouseState::globalStateMut();
        switch (ev.type) {
            case NSEventTypeLeftMouseDown:
                state.buttons = state.buttons | MouseButtons::ButtonLeft;
                break;
            case NSEventTypeRightMouseDown:
                state.buttons = state.buttons | MouseButtons::ButtonRight;
                break;
            case NSEventTypeOtherMouseDown:
                state.buttons = state.buttons | MouseButtons::ButtonMiddle;
                break;
            default:
                break;
        }

        [self.nextResponder mouseDown:ev];
    }

    - (void)mouseUp:(NSEvent*)ev {
        MouseState& state = MouseState::globalStateMut();
        switch (ev.type) {
            case NSEventTypeLeftMouseUp:
                state.buttons = (state.buttons | MouseButtons::ButtonLeft) - MouseButtons::ButtonLeft;
                break;
            case NSEventTypeRightMouseUp:
                state.buttons = (state.buttons | MouseButtons::ButtonRight) - MouseButtons::ButtonRight;
                break;
            case NSEventTypeOtherMouseUp:
                state.buttons = (state.buttons | MouseButtons::ButtonMiddle) - MouseButtons::ButtonMiddle;
                break;
            default:
                break;
        }

        [self.nextResponder mouseUp: ev];
    }

    - (void)mouseMoved:(NSEvent*)ev {
        NSPoint point = [self.contentView convertPoint:[ev locationInWindow] fromView:nil];
        float zoom = [self.contentView frameZoomFactor];

        float x = point.x / zoom;
        float y = ([self.contentView getHeight] - point.y) / zoom;

        MouseState::globalStateMut().position = ccp(x, y);

        [self.nextResponder mouseMoved: ev];
    }

    - (void)scrollWheel:(NSEvent*)ev {
        MouseState& state = MouseState::globalStateMut();
        state.scrollX = ev.scrollingDeltaX;
        state.scrollY = ev.scrollingDeltaY;

        [self.nextResponder scrollWheel:ev];
    }
@end

__attribute__((constructor)) void nswindowHook() {
    [NSApp.mainWindow registerForDraggedTypes:[NSArray arrayWithObjects: NSFilenamesPboardType, nil]];
    // idk
    // [NSApp.mainWindow setAcceptsMouseMovedEvents:YES];
}

#endif
