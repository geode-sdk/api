#include <dispatch/MacMouseEvent.hmm>
#include <dispatch/ExtMouseManager.hpp>

#ifdef GEODE_IS_MACOS

#include <CCEventDispatcher.h>
#import <Foundation/Foundation.h>


USE_GEODE_NAMESPACE();

static MacMouseEvent* s_sharedEvent = nil;
using EventType = void(*)(id, SEL, NSEvent*);
static EventType s_originalMouseMoved;

@implementation MacMouseEvent

// i just want to get this working
- (void)mouseMovedHook:(NSEvent*)event {
	s_originalMouseMoved(self, @selector(mouseMoved:), event);
    [[MacMouseEvent sharedEvent] mouseMoved:event];
}

+ (void)load {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        Class class_ = NSClassFromString(@"EAGLView");

        SEL originalSelector = @selector(mouseMoved:);
        SEL swizzledSelector = @selector(mouseMovedHook:);

        Method originalMethod = class_getInstanceMethod(class_, originalSelector);
        Method swizzledMethod = class_getInstanceMethod([self class], swizzledSelector);


        Log::get() << (void*) originalMethod << "  sdfdfsfsd " << (void*) swizzledMethod;
        s_originalMouseMoved = (EventType)method_getImplementation(originalMethod);
        // When swizzling a class method, use the following:
        // Method originalMethod = class_getClassMethod(class, originalSelector);
        // Method swizzledMethod = class_getClassMethod(class, swizzledSelector);

        method_exchangeImplementations(originalMethod, swizzledMethod);

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

-(void) mouseMoved:(NSEvent*)event {
    NSPoint event_location = [event locationInWindow];
	NSPoint local_point = [[NSClassFromString(@"EAGLView") sharedEGLView] convertPoint:event_location fromView:nil];
	
	float x = local_point.x;
	float y = [[NSClassFromString(@"EAGLView") sharedEGLView] getHeight] - local_point.y;

	m_xPosition = x / [[NSClassFromString(@"EAGLView") sharedEGLView] frameZoomFactor];
	m_yPosition = y / [[NSClassFromString(@"EAGLView") sharedEGLView] frameZoomFactor];

	Log::get() << m_xPosition << " " << m_yPosition;
}

-(cocos2d::CCPoint) getMousePosition {
    return cocos2d::CCPoint(m_xPosition, m_yPosition);
}

@end

CCPoint ExtMouseManager::getMousePosition() {
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
