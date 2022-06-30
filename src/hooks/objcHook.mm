#include "hook.hpp"


#if defined(GEODE_IS_MACOS)

#import <Cocoa/Cocoa.h>
#include <dispatch/MacMouseEvent.hmm>
#include <DragDropEvent.hpp>

// Code taken from https://cocoawithlove.com/2008/03/supersequent-implementation.html
/*
#define invokeSupersequent(...) \
    ([self getImplementationOf:_cmd \
        after:impOfCallingMethod(self, _cmd)]) \
            (self, _cmd, ##__VA_ARGS__)

#define invokeSupersequentNoParameters() \
   ([self getImplementationOf:_cmd \
      after:impOfCallingMethod(self, _cmd)]) \
         (self, _cmd)

@implementation NSObject (SupersequentImplementation)

// Lookup the next implementation of the given selector after the
// default one. Returns nil if no alternate implementation is found.
- (IMP)getImplementationOf:(SEL)lookup after:(IMP)skip
{
    BOOL found = NO;
    
    Class currentClass = object_getClass(self);
    while (currentClass)
    {
        // Get the list of methods for this class
        unsigned int methodCount;
        Method *methodList = class_copyMethodList(currentClass, &methodCount);
        
        // Iterate over all methods
        unsigned int i;
        for (i = 0; i < methodCount; i++)
        {
            // Look for the selector
            if (method_getName(methodList[i]) != lookup)
            {
                continue;
            }
            
            IMP implementation = method_getImplementation(methodList[i]);
            
            // Check if this is the "skip" implementation
            if (implementation == skip)
            {
                found = YES;
            }
            else if (found)
            {
                // Return the match.
                free(methodList);
                return implementation;
            }
        }
    
        // No match found. Traverse up through super class' methods.
        free(methodList);

        currentClass = class_getSuperclass(currentClass);
    }
    return nil;
}

@end
*/
// end code snippet

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
@end

__attribute__((constructor)) void nswindowHook() {
    [NSApp.mainWindow registerForDraggedTypes:[NSArray arrayWithObjects: NSFilenamesPboardType, nil]];
    // idk
    // [NSApp.mainWindow setAcceptsMouseMovedEvents:YES];
}

#endif
