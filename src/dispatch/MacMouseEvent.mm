#include <dispatch/MacMouseEvent.hmm>
#include <dispatch/ExtMouseDispatcher.hpp>

#ifdef GEODE_IS_MACOS

#include <platform/mac/CCEventDispatcher.h>
#import <Foundation/Foundation.h>


USE_GEODE_NAMESPACE();

static MacMouseEvent* s_sharedEvent = nil;
using EventType = void(*)(id, SEL, NSEvent*);


@implementation MacMouseEvent



// i just want to get this working
#define GEODE_API_OBJC_SWIZZLE_METHODS(mouseType, prefix, originalPrefix)               \
static EventType originalPrefix##Dragged;                                               \
- (void)prefix##DraggedHook:(NSEvent*)event {                                           \
	originalPrefix##Dragged(self, @selector(prefix##Dragged:), event);                  \
	[[MacMouseEvent sharedEvent] moved:event];                                          \
}                                                                                       \
static EventType originalPrefix##Down;                                                  \
- (void)prefix##DownHook:(NSEvent*)event {                                              \
	originalPrefix##Down(self, @selector(prefix##Down:), event);                        \
	[[MacMouseEvent sharedEvent] down:event type:geode::MouseEvent::mouseType];         \
}                                                                                       \
static EventType originalPrefix##Up;                                                    \
- (void)prefix##UpHook:(NSEvent*)event {                                                \
	originalPrefix##Up(self, @selector(prefix##Up:), event);                            \
	[[MacMouseEvent sharedEvent] up:event type:geode::MouseEvent::mouseType];           \
}

static EventType s_originalMouseMoved;
- (void)mouseMovedHook:(NSEvent*)event {
	s_originalMouseMoved(self, @selector(mouseMoved:), event);
	[[MacMouseEvent sharedEvent] moved:event];
}
GEODE_API_OBJC_SWIZZLE_METHODS(Left, mouse, s_originalMouse)
GEODE_API_OBJC_SWIZZLE_METHODS(Right, rightMouse, s_originalRightMouse)
GEODE_API_OBJC_SWIZZLE_METHODS(Middle, otherMouse, s_originalOtherMouse)

#define GEODE_API_OBJC_SWIZZLE(method, swizzle, original)                               \
Method method##Method = class_getInstanceMethod(class_, @selector(method:));            \
Method swizzle##Method = class_getInstanceMethod([self class], @selector(swizzle:));    \
original = (decltype(original))method_getImplementation(method##Method);                \
method_exchangeImplementations(method##Method, swizzle##Method);


+ (void)load {
	static dispatch_once_t onceToken;
	dispatch_once(&onceToken, ^{
		Class class_ = NSClassFromString(@"EAGLView");

		GEODE_API_OBJC_SWIZZLE(mouseMoved, mouseMovedHook, s_originalMouseMoved)

		GEODE_API_OBJC_SWIZZLE(mouseDragged, mouseDraggedHook, s_originalMouseDragged)
		GEODE_API_OBJC_SWIZZLE(mouseDown, mouseDownHook, s_originalMouseDown)
		GEODE_API_OBJC_SWIZZLE(mouseUp, mouseUpHook, s_originalMouseUp)

		GEODE_API_OBJC_SWIZZLE(rightMouseDragged, rightMouseDraggedHook, s_originalRightMouseDragged)
		GEODE_API_OBJC_SWIZZLE(rightMouseDown, rightMouseDownHook, s_originalRightMouseDown)
		GEODE_API_OBJC_SWIZZLE(rightMouseUp, rightMouseUpHook, s_originalRightMouseUp)

		GEODE_API_OBJC_SWIZZLE(otherMouseDragged, otherMouseDraggedHook, s_originalOtherMouseDragged)
		GEODE_API_OBJC_SWIZZLE(otherMouseDown, otherMouseDownHook, s_originalOtherMouseDown)
		GEODE_API_OBJC_SWIZZLE(otherMouseUp, otherMouseUpHook, s_originalOtherMouseUp)
	});
}

+(MacMouseEvent*) sharedEvent {
	@synchronized(self) {
		if (s_sharedEvent == nil) {
			s_sharedEvent = [[self alloc] init]; // assignment not done here
		}
	}
	
	return s_sharedEvent;
}

-(void) moved:(NSEvent*)event {
	NSPoint event_location = [event locationInWindow];
	NSPoint local_point = [[NSClassFromString(@"EAGLView") sharedEGLView] convertPoint:event_location fromView:nil];
	
	float x = local_point.x;
	float y = [[NSClassFromString(@"EAGLView") sharedEGLView] getHeight] - local_point.y;

	m_xPosition = x / [[NSClassFromString(@"EAGLView") sharedEGLView] frameZoomFactor];
	m_yPosition = y / [[NSClassFromString(@"EAGLView") sharedEGLView] frameZoomFactor];
}

-(void) down:(NSEvent*)event type:(geode::MouseEvent)type {
	[self moved: event];
	ExtMouseDispatcher::get()->mouseDown(type);
}

-(void) up:(NSEvent*)event type:(geode::MouseEvent)type {
	[self moved: event];
	ExtMouseDispatcher::get()->mouseUp(type);
}

-(cocos2d::CCPoint) getMousePosition {
	return cocos2d::CCPoint(m_xPosition, m_yPosition);
}

@end

CCPoint ExtMouseDispatcher::getMousePosition() {
	static auto cachedMousePos = CCPointZero;
	auto mpos = [s_sharedEvent getMousePosition];
	if (mpos == cachedMousePos) return cachedMousePos;
	
	auto winSize = CCDirector::sharedDirector()->getWinSize();
	auto winSizePx = CCDirector::sharedDirector()->getOpenGLView()->getViewPortRect();
	auto ratio_w = winSize.width / winSizePx.size.width;
	auto ratio_h = winSize.height / winSizePx.size.height;
	mpos.y = winSizePx.size.height - mpos.y;
	mpos.x *= ratio_w;
	mpos.y *= ratio_h;
	return mpos;
}

#endif
