#import <QuartzCore/QuartzCore.h>
#import "TouchView.h"


#include "gkPlatform.h"

SIOSInputDelegate* global_getIOSInputDelegate();
SIOSInputDelegate* g_IOSInputDelegate = NULL;

//CLASS IMPLEMENTATIONS:

@implementation gkTouchView

- (id) initWithFrame:(CGRect)frame
{
    
    UIScreen* screen = [UIScreen mainScreen];
    _scale = [screen scale];
    

    
    CGRect	rect = [screen applicationFrame];
    _offset = rect.size.width;
    
    //- 16:9
    
    if(false)
    {
        rect.size.width = rect.size.height * 9 / 16;
    }
    
    if( rect.size.width * rect.size.height > 700000 )
    {
        _scale = 1.0;
    }
    
    
    _offset -= rect.size.width;
    _offset *= 0.5f;
    
	if((self = [super initWithFrame:frame])) {
		[self setMultipleTouchEnabled:YES];
	}
    m_Frame = frame;
    
    
    UIRotationGestureRecognizer *rotationGesture = [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(gestureRotateCallback:)];
    [self addGestureRecognizer:rotationGesture];
    //[rotationGesture setDelegate:self];
    [rotationGesture setRotation:0.f];
    
    UIPinchGestureRecognizer *pinchGesture = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(gestureScaleCallback:)];
    [pinchGesture setScale:1.f];
    //[pinchGesture setDelegate:self];
    [self addGestureRecognizer:pinchGesture];
    
    UIPanGestureRecognizer *panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(gesturePanCallback:)];
    panGesture.minimumNumberOfTouches = 2;
    [self addGestureRecognizer:panGesture];
    
    //    UIScreenEgdePanGestureRecognizer *panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(gesturePanCallback:)];
    //    panGesture.minimumNumberOfTouches = 2;
    //    [self addGestureRecognizer:panGesture];
    
    // ccw rotate 90 degrees
    UISwipeGestureRecognizer *swipeGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(gestureSwipeLeftCallback:)];
    [self addGestureRecognizer:swipeGesture];
    //[swipeGesture setDelegate:self];
    [swipeGesture setDelaysTouchesBegan:NO];
    [swipeGesture setDirection:UISwipeGestureRecognizerDirectionDown];
    [swipeGesture setCancelsTouchesInView:NO];
    
    swipeGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(gestureSwipeRightCallback:)];
    [self addGestureRecognizer:swipeGesture];
    //[swipeGesture setDelegate:self];
    [swipeGesture setDelaysTouchesBegan:NO];
    [swipeGesture setDirection:UISwipeGestureRecognizerDirectionUp];
    [swipeGesture setCancelsTouchesInView:NO];
    
    
    swipeGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(gestureSwipeUpCallback:)];
    [self addGestureRecognizer:swipeGesture];
    //[swipeGesture setDelegate:self];
    [swipeGesture setDelaysTouchesBegan:NO];
    [swipeGesture setDirection:UISwipeGestureRecognizerDirectionLeft];
    [swipeGesture setCancelsTouchesInView:NO];
    
    
    swipeGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(gestureSwipeDownCallback:)];
    [self addGestureRecognizer:swipeGesture];
    //[swipeGesture setDelegate:self];
    [swipeGesture setDelaysTouchesBegan:NO];
    [swipeGesture setDirection:UISwipeGestureRecognizerDirectionRight];
    [swipeGesture setCancelsTouchesInView:NO];
    
    m_gesture_processing = false;
    
	return self;
}

- (void) dealloc
{
	
	[super dealloc];
}

- (void) layoutSubviews
{
}

- (void) setCurrentContext
{
}

- (BOOL) isCurrentContext
{
}

- (void) clearCurrentContext
{
}

- (CGPoint) transformPoint: (CGPoint)pt
{
    CGPoint ret;
    ret.x = m_Frame.size.height - pt.y;
    ret.y = pt.x - _offset;
    
    ret.x *= _scale;
    ret.y *= _scale;
    
    return ret;
}

// Handles the start of a touch
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	g_IOSInputDelegate = global_getIOSInputDelegate();
    
	for( UITouch *touch in touches )
	{
		// Grab the location of the touch
		m_TouchStart = [touch locationInView:self];
		//if(_scale != 1.0)
		//	m_TouchStart.x*=_scale;m_TouchStart.y*=_scale;

        
        CGPoint pt = [self transformPoint:[touch locationInView:self]];
        
        SIOSInputEvent iievent;
        iievent.motionType = eIET_finger;
        iievent.motionState = eIES_IOSstart;
        iievent.deviceIdx = (gk_ptr_type)touch;
        iievent.asyncPos.x = pt.x;
        iievent.asyncPos.y = pt.y;
        
        if (g_IOSInputDelegate) {
            g_IOSInputDelegate->pushEvent(iievent);
        }

	}
}

// Handles the continuation of a touch.
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	g_IOSInputDelegate = global_getIOSInputDelegate();
    
    
    
	for( UITouch *touch in touches )
	{
        //gkLogMessage("moving index: %d", (int)touch);
        
		// Grab the location of the touch
		CGPoint touchLocation = [touch locationInView:self];
		//if(_scale != 1.0)
		//	touchLocation.x*=_scale;touchLocation.y*=_scale;
        
        CGPoint pt = [self transformPoint:[touch locationInView:self]];
        
        SIOSInputEvent iievent;
        iievent.motionType = eIET_finger;
        iievent.motionState = eIES_IOSchange;
        iievent.deviceIdx = (gk_ptr_type)touch;
        iievent.asyncPos.x = pt.x;
        iievent.asyncPos.y = pt.y;
        
        if (g_IOSInputDelegate) {
            g_IOSInputDelegate->pushEvent(iievent);
        }

	}
}

// Handles the end of a touch event.
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	g_IOSInputDelegate = global_getIOSInputDelegate();
    
	for( UITouch *touch in touches )
	{
		// Grab the location of the touch
		// look at start coords and finish coords and decide what action to set
		CGPoint touchLocation = [touch locationInView:self];
		//if(_scale != 1.0)
		//	touchLocation.x*=_scale;touchLocation.y*=_scale;
		
		CGPoint touchMove;	// no operator because we're in Obj-C
		touchMove.x = touchLocation.x - m_TouchStart.x;
		touchMove.y = touchLocation.y - m_TouchStart.y;
    
            
        CGPoint pt = [self transformPoint:[touch locationInView:self]];
        
        SIOSInputEvent iievent;
        iievent.motionType = eIET_finger;
        iievent.motionState = eIES_IOSend;
        iievent.deviceIdx = (gk_ptr_type)touch;
        iievent.asyncPos.x = pt.x;
        iievent.asyncPos.y = pt.y;
            
            if (g_IOSInputDelegate) {
                g_IOSInputDelegate->pushEvent(iievent);
            } 
		
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    g_IOSInputDelegate = global_getIOSInputDelegate();
    
    for (UITouch *touch in touches) {
        
        CGPoint pt = [self transformPoint:[touch locationInView:self]];
        
        SIOSInputEvent iievent;
        iievent.motionType = eIET_finger;
        iievent.deviceIdx = (gk_ptr_type)touch;
        iievent.asyncPos.x = pt.x;
        iievent.asyncPos.y = pt.y;
        
        if (g_IOSInputDelegate) {
            g_IOSInputDelegate->pushEvent(iievent);
        }
        
    }
}

- (void)gestureRotateCallback:(UIRotationGestureRecognizer *)gestureRecognizer
{
    if ([gestureRecognizer state] == UIGestureRecognizerStateBegan)
    {
        //m_device->addGestureMsg( eKI_Gesture_Rotate , eIS_OnPress, [gestureRecognizer rotation]);
    }
    else if ( [gestureRecognizer state] == UIGestureRecognizerStateChanged) {
        //m_device->addGestureMsg( eKI_Gesture_Rotate , eIS_Changed, [gestureRecognizer rotation]);
    }
    else if ([gestureRecognizer state] == UIGestureRecognizerStateEnded)
    {
        //m_device->addGestureMsg( eKI_Gesture_Rotate , eIS_OnRelease, [gestureRecognizer rotation]);
    }
    
    
    
}

- (void)gestureScaleCallback:(UIPinchGestureRecognizer *)gestureRecognizer
{
    bool makeEvent = false;
    SIOSInputEvent iievent;
    iievent.deviceIdx = 0;
    iievent.asyncPos.x = [gestureRecognizer scale];
    iievent.asyncPos.y = [gestureRecognizer scale];
    iievent.motionType = eIET_ges_zoom;
    
    if ([gestureRecognizer state] == UIGestureRecognizerStateBegan)
    {
        iievent.motionState = eIES_IOSstart;
        makeEvent = true;
    }
    else if ( [gestureRecognizer state] == UIGestureRecognizerStateChanged)
    {
        iievent.motionState = eIES_IOSchange;
        makeEvent = true;
    }
    else if ([gestureRecognizer state] == UIGestureRecognizerStateEnded)
    {
        iievent.motionState = eIES_IOSend;
        makeEvent = true;
    }
    
    if ( makeEvent )
    {
        if (g_IOSInputDelegate) {
            g_IOSInputDelegate->pushEvent(iievent);
        }
    }
}

- (void)gesturePanCallback:(UIPanGestureRecognizer *)gestureRecognizer
{
    bool makeEvent = false;
    SIOSInputEvent iievent;
    iievent.deviceIdx = 0;
    
    CGPoint pt = [self transformPoint:[gestureRecognizer locationInView:self]];
    
    iievent.asyncPos.x = pt.x;
    iievent.asyncPos.y = pt.y;
    iievent.motionType = eIET_ges_pan;
    
    
    if ([gestureRecognizer state] == UIGestureRecognizerStateBegan)
    {
        iievent.motionState = eIES_IOSstart;
        makeEvent = true;
    }
    else if ( [gestureRecognizer state] == UIGestureRecognizerStateChanged)
    {
        iievent.motionState = eIES_IOSchange;
        makeEvent = true;
    }
    else if ([gestureRecognizer state] == UIGestureRecognizerStateEnded)
    {
        iievent.motionState = eIES_IOSend;
        makeEvent = true;
    }
    
    if ( makeEvent )
    {
        if (g_IOSInputDelegate) {
            g_IOSInputDelegate->pushEvent(iievent);
        }
    }
}

- (void)gestureSwipeUpCallback:(UISwipeGestureRecognizer *)gestureRecognizer
{
    if ([gestureRecognizer state] == UIGestureRecognizerStateBegan)
    {
        //m_device->addGestureMsg( eKI_Gesture_SwipeUp , eIS_OnPress);
        //m_gesture_processing = true;
    }
    else if ([gestureRecognizer state] == UIGestureRecognizerStateChanged)
    {
        //m_device->addGestureMsg( eKI_Gesture_SwipeUp , eIS_Changed);
    }
    else if ([gestureRecognizer state] == UIGestureRecognizerStateEnded)
    {
        //m_device->addGestureMsg( eKI_Gesture_SwipeUp , eIS_OnPress);
        //m_gesture_processing = false;
    }
}

- (void)gestureSwipeDownCallback:(UISwipeGestureRecognizer *)gestureRecognizer
{
    if ([gestureRecognizer state] == UIGestureRecognizerStateBegan)
    {
        //m_device->addGestureMsg( eKI_Gesture_SwipeDown , eIS_OnPress);
        //m_gesture_processing = true;
    }
    else if ([gestureRecognizer state] == UIGestureRecognizerStateChanged)
    {
        //m_device->addGestureMsg( eKI_Gesture_SwipeDown , eIS_Changed);
    }
    else if ([gestureRecognizer state] == UIGestureRecognizerStateEnded)
    {
        //m_device->addGestureMsg( eKI_Gesture_SwipeDown , eIS_OnPress);
        //m_gesture_processing = false;
    }
}

- (void)gestureSwipeLeftCallback:(UISwipeGestureRecognizer *)gestureRecognizer
{
    if ([gestureRecognizer state] == UIGestureRecognizerStateBegan)
    {
        //m_device->addGestureMsg( eKI_Gesture_SwipeLeft , eIS_OnPress);
    }
    else if ([gestureRecognizer state] == UIGestureRecognizerStateChanged)
    {
        //m_device->addGestureMsg( eKI_Gesture_SwipeLeft , eIS_Changed);
    }
    else if ([gestureRecognizer state] == UIGestureRecognizerStateEnded)
    {
        //m_device->addGestureMsg( eKI_Gesture_SwipeLeft , eIS_OnPress);
    }
}

- (void)gestureSwipeRightCallback:(UISwipeGestureRecognizer *)gestureRecognizer
{
    if ([gestureRecognizer state] == UIGestureRecognizerStateBegan)
    {
        //m_device->addGestureMsg( eKI_Gesture_SwipeRight , eIS_OnPress);
    }
    else if ([gestureRecognizer state] == UIGestureRecognizerStateChanged)
    {
        //m_device->addGestureMsg( eKI_Gesture_SwipeRight , eIS_Changed);
    }
    else if ([gestureRecognizer state] == UIGestureRecognizerStateEnded)
    {
        //m_device->addGestureMsg( eKI_Gesture_SwipeRight , eIS_OnPress);
    }
}

- (void)gestureTapCallback:(UITapGestureRecognizer *)gestureRecognizer
{
    CGPoint pt = [self transformPoint:[gestureRecognizer locationInView:self]];
    if ([gestureRecognizer state] == UIGestureRecognizerStateBegan)
    {
        //m_device->addIOSInputMsg((UINT32)0, eIT_DoubleTouch, VEC2(pt.x, pt.y));
    }
    else if ([gestureRecognizer state] == UIGestureRecognizerStateChanged)
    {
        //m_device->addGestureMsg( eKI_Gesture_SwipeRight , eIS_Changed);
    }
    else if ([gestureRecognizer state] == UIGestureRecognizerStateEnded)
    {
        //m_device->addGestureMsg( eKI_Gesture_SwipeRight , eIS_OnRelease);
    }
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    return YES;
}

- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer
{
    return YES;
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
    return m_gesture_processing;
}

@end
