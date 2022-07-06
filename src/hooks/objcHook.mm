#include "hook.hpp"


#if defined(GEODE_IS_MACOS)

#import <Cocoa/Cocoa.h>
#include <DragDropEvent.hpp>
#include <MouseEvent.hpp>
#include <objc/runtime.h>

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

    + (void)load {
        NSLog(@"balls!");
    }
@end

@interface GeodeResponder : NSResponder
@end

@implementation GeodeResponder
    - (void)mouseMoved:(NSEvent*)ev {
        Log::get() << "balls";
    }
@end

// i hate all caps macros so much
#define $swizzle(cls, method, swizz) { \
    Method m = class_getInstanceMethod([cls class], @selector(method)); \
    auto thing = method_setImplementation(m, (IMP)swizzles::swizz); \
    swizzles::_##swizz = reinterpret_cast<decltype(&swizzles::swizz)>(thing); \
}

namespace swizzles {
    template <typename T, typename ...Args>
    using imp = std::function<T(id, SEL, Args...)>;

    imp<void, NSEvent*> _mouseMoved;
    void mouseMoved(NSView* self, SEL _cmd, NSEvent* ev) {
        MouseState state = MouseState::globalState();
        NSPoint point = [self convertPoint:[ev locationInWindow] fromView:nil];
        float zoom = [self frameZoomFactor];

        float x = point.x / zoom;
        float y = ([self getHeight] - point.y) / zoom;

        state.position = ccp(x, y);

        MouseEvent::create(state)->post();
        _mouseMoved(self, _cmd, ev);
    }

    imp<void, NSEvent*> _scrollWheel;
    void scrollWheel(NSView* self, SEL _cmd, NSEvent* ev) {
        MouseState state = MouseState::globalStateMut();
        state.scrollX = ev.scrollingDeltaX;
        state.scrollY = ev.scrollingDeltaY;

        MouseEvent::create(state)->post();
        _scrollWheel(self, _cmd, ev);
    }

    imp<void, NSEvent*> _mouseUp;
    void mouseUp(NSView* self, SEL _cmd, NSEvent* ev) {
        MouseState state = MouseState::globalStateMut();
        state.buttons = (state.buttons | MouseButtons::ButtonLeft) - MouseButtons::ButtonLeft;
        MouseEvent::create(state)->post();
        _mouseUp(self, _cmd, ev);
    }

    imp<void, NSEvent*> _mouseDown;
    void mouseDown(NSView* self, SEL _cmd, NSEvent* ev) {
        MouseState state = MouseState::globalStateMut();
        state.buttons = state.buttons | MouseButtons::ButtonLeft;
        MouseEvent::create(state)->post();

        _mouseDown(self, _cmd, ev);
    }

    imp<void, NSEvent*> _rightMouseUp;
    void rightMouseUp(NSView* self, SEL _cmd, NSEvent* ev) {
        MouseState state = MouseState::globalStateMut();
        state.buttons = (state.buttons | MouseButtons::ButtonRight) - MouseButtons::ButtonRight;
        MouseEvent::create(state)->post();
        _rightMouseUp(self, _cmd, ev);
    }

    imp<void, NSEvent*> _rightMouseDown;
    void rightMouseDown(NSView* self, SEL _cmd, NSEvent* ev) {
        MouseState state = MouseState::globalStateMut();
        state.buttons = state.buttons | MouseButtons::ButtonRight;
        MouseEvent::create(state)->post();

        _rightMouseDown(self, _cmd, ev);
    }

    imp<void, NSEvent*> _otherMouseUp;
    void otherMouseUp(NSView* self, SEL _cmd, NSEvent* ev) {
        MouseState state = MouseState::globalStateMut();
        state.buttons = (state.buttons | MouseButtons::ButtonMiddle) - MouseButtons::ButtonMiddle;
        MouseEvent::create(state)->post();
        _otherMouseUp(self, _cmd, ev);
    }

    imp<void, NSEvent*> _otherMouseDown;
    void otherMouseDown(NSView* self, SEL _cmd, NSEvent* ev) {
        MouseState state = MouseState::globalStateMut();
        state.buttons = state.buttons | MouseButtons::ButtonMiddle;
        MouseEvent::create(state)->post();

        _otherMouseDown(self, _cmd, ev);
    }
}

__attribute__((constructor)) void nswindowHook() {
    sleep(1);
    Loader::get()->queueInGDThread([] {
        $swizzle(NSApp.windows.lastObject.contentView, mouseMoved:, mouseMoved);
        $swizzle(NSApp.windows.lastObject.contentView, scrollWheel:, scrollWheel);
        $swizzle(NSApp.windows.lastObject.contentView, mouseUp:, mouseUp);
        $swizzle(NSApp.windows.lastObject.contentView, mouseDown:, mouseDown);
        $swizzle(NSApp.windows.lastObject.contentView, rightMouseUp:, rightMouseUp);
        $swizzle(NSApp.windows.lastObject.contentView, rightMouseDown:, rightMouseDown);
        $swizzle(NSApp.windows.lastObject.contentView, otherMouseUp:, otherMouseUp);
        $swizzle(NSApp.windows.lastObject.contentView, otherMouseDown:, otherMouseDown);
        [NSApp.windows.lastObject registerForDraggedTypes:[NSArray arrayWithObjects: NSFilenamesPboardType, nil]];
    });
}

#endif
