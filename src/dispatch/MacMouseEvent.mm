#include <dispatch/MacMouseEvent.hmm>
#include <dispatch/ExtMouseManager.hpp>

#ifdef GEODE_IS_MACOS

#include <CCEventDispatcher.h>

USE_GEODE_NAMESPACE();

static MacMouseEvent* s_sharedEvent = nil;


@implementation MacMouseEvent

+(MacMouseEvent*) sharedEvent {
	@synchronized(self) {
		if (s_sharedEvent == nil) {
            s_sharedEvent = [[self alloc] init]; // assignment not done here
            [[CCEventDispatcher sharedDispatcher] addMouseDelegate: s_sharedEvent priority: 1000];
        }
	}
    
	return s_sharedEvent;
}

-(BOOL) ccMouseMoved:(NSEvent*)event {
    NSPoint event_location = [event locationInWindow];
	NSPoint local_point = [[EAGLView sharedEGLView] convertPoint:event_location fromView:nil];
	
	float x = local_point.x;
	float y = [[EAGLView sharedEGLView] getHeight] - local_point.y;
	
    // int ids[1] = {0};
    
	// ids[0] = [event eventNumber];
	m_xPosition = x / [[EAGLView sharedEGLView] frameZoomFactor];
	m_yPosition = y / [[EAGLView sharedEGLView] frameZoomFactor];
    return FALSE;
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