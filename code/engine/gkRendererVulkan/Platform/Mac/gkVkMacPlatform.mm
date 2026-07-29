#include "gkVkMacPlatform.h"

#include "gkPlatform.h"
#include "ISystem.h"
#include "IInputManager.h"

#import <AppKit/AppKit.h>
#import <QuartzCore/CAMetalLayer.h>

#include <map>
#include <set>
#include <cmath>
#include <cstdio>
#include <cstdlib>

struct gkVkMacWindow
{
	NSWindow* window;
	NSView* view;
	CAMetalLayer* layer;
	bool fullscreen;
	bool hadFocus;
	bool hasMousePosition;
	float mouseX;
	float mouseY;
	float backingScale;
	uint32_t drawableWidth;
	uint32_t drawableHeight;
	std::map<unsigned short, SInputSymbol*> keySymbols;
	std::set<unsigned short> pressedKeys;
	SInputSymbol* mouseButtons[8];
	bool pressedMouseButtons[8];
	SInputSymbol* mouseXSymbol;
	SInputSymbol* mouseYSymbol;
	SInputSymbol* mouseZSymbol;
	SInputSymbol* mouseWheelUpSymbol;
	SInputSymbol* mouseWheelDownSymbol;
};

namespace
{
	bool traceInput()
	{
		static const bool enabled = getenv("GK_INPUT_TRACE") != NULL;
		return enabled;
	}

	void updateDrawableMetrics(gkVkMacWindow* window)
	{
		if (!window || !window->window || !window->view || !window->layer)
			return;

		const CGFloat scale = window->window.backingScaleFactor > 0.0 ?
			window->window.backingScaleFactor : 1.0;
		const NSRect logicalBounds = window->view.bounds;
		const uint32_t drawableWidth = static_cast<uint32_t>(
			std::lround(logicalBounds.size.width));
		const uint32_t drawableHeight = static_cast<uint32_t>(
			std::lround(logicalBounds.size.height));
		const bool changed =
			window->backingScale != static_cast<float>(scale) ||
			window->drawableWidth != drawableWidth ||
			window->drawableHeight != drawableHeight;

		window->backingScale = static_cast<float>(scale);
		window->drawableWidth = drawableWidth;
		window->drawableHeight = drawableHeight;
		// MoltenVK derives VkSurfaceCapabilitiesKHR::currentExtent from the
		// layer bounds multiplied by contentsScale. Keep the Metal rendering
		// scale at 1 so the swapchain follows macOS logical ("looks like")
		// dimensions instead of the panel's Retina backing pixels.
		window->layer.contentsScale = 1.0;
		window->layer.drawableSize = CGSizeMake(drawableWidth, drawableHeight);

		if (changed && drawableWidth && drawableHeight)
		{
			const NSSize logicalSize = window->view.bounds.size;
			fprintf(stderr,
				"[gkVulkan/macOS] Drawable %ux%u logical pixels "
				"(%.0fx%.0f pt, backing scale %.2fx)\n",
				drawableWidth, drawableHeight,
				logicalSize.width, logicalSize.height,
				static_cast<double>(scale));
		}
	}

	void addKey(gkVkMacWindow* window, unsigned short nativeKey,
		EKeyId engineKey)
	{
		SInputSymbol* symbol = new SInputSymbol(
			static_cast<uint32>(nativeKey), engineKey, SInputSymbol::Button);
		symbol->deviceId = eDI_Keyboard;
		window->keySymbols[nativeKey] = symbol;
	}

	void createInputSymbols(gkVkMacWindow* window)
	{
		// ANSI keys. NSEvent key codes are hardware-position based and therefore
		// remain stable when the active keyboard layout changes.
		addKey(window, 0, eKI_A);
		addKey(window, 1, eKI_S);
		addKey(window, 2, eKI_D);
		addKey(window, 3, eKI_F);
		addKey(window, 4, eKI_H);
		addKey(window, 5, eKI_G);
		addKey(window, 6, eKI_Z);
		addKey(window, 7, eKI_X);
		addKey(window, 8, eKI_C);
		addKey(window, 9, eKI_V);
		addKey(window, 11, eKI_B);
		addKey(window, 12, eKI_Q);
		addKey(window, 13, eKI_W);
		addKey(window, 14, eKI_E);
		addKey(window, 15, eKI_R);
		addKey(window, 16, eKI_Y);
		addKey(window, 17, eKI_T);
		addKey(window, 18, eKI_1);
		addKey(window, 19, eKI_2);
		addKey(window, 20, eKI_3);
		addKey(window, 21, eKI_4);
		addKey(window, 22, eKI_6);
		addKey(window, 23, eKI_5);
		addKey(window, 24, eKI_Equals);
		addKey(window, 25, eKI_9);
		addKey(window, 26, eKI_7);
		addKey(window, 27, eKI_Minus);
		addKey(window, 28, eKI_8);
		addKey(window, 29, eKI_0);
		addKey(window, 30, eKI_RBracket);
		addKey(window, 31, eKI_O);
		addKey(window, 32, eKI_U);
		addKey(window, 33, eKI_LBracket);
		addKey(window, 34, eKI_I);
		addKey(window, 35, eKI_P);
		addKey(window, 37, eKI_L);
		addKey(window, 38, eKI_J);
		addKey(window, 39, eKI_Apostrophe);
		addKey(window, 40, eKI_K);
		addKey(window, 41, eKI_Semicolon);
		addKey(window, 42, eKI_Backslash);
		addKey(window, 43, eKI_Comma);
		addKey(window, 44, eKI_Slash);
		addKey(window, 45, eKI_N);
		addKey(window, 46, eKI_M);
		addKey(window, 47, eKI_Period);
		addKey(window, 48, eKI_Tab);
		addKey(window, 49, eKI_Space);
		addKey(window, 50, eKI_Tilde);
		addKey(window, 51, eKI_Backspace);
		addKey(window, 53, eKI_Escape);

		// Modifiers and navigation.
		addKey(window, 54, eKI_RWin);
		addKey(window, 55, eKI_LWin);
		addKey(window, 56, eKI_LShift);
		addKey(window, 57, eKI_CapsLock);
		addKey(window, 58, eKI_LAlt);
		addKey(window, 59, eKI_LCtrl);
		addKey(window, 60, eKI_RShift);
		addKey(window, 61, eKI_RAlt);
		addKey(window, 62, eKI_RCtrl);
		addKey(window, 114, eKI_Insert);
		addKey(window, 115, eKI_Home);
		addKey(window, 116, eKI_PgUp);
		addKey(window, 117, eKI_Delete);
		addKey(window, 119, eKI_End);
		addKey(window, 121, eKI_PgDn);
		addKey(window, 123, eKI_Left);
		addKey(window, 124, eKI_Right);
		addKey(window, 125, eKI_Down);
		addKey(window, 126, eKI_Up);

		// Function keys and numeric keypad.
		addKey(window, 65, eKI_NP_Period);
		addKey(window, 67, eKI_NP_Multiply);
		addKey(window, 69, eKI_NP_Add);
		addKey(window, 75, eKI_NP_Divide);
		addKey(window, 76, eKI_NP_Enter);
		addKey(window, 78, eKI_NP_Substract);
		addKey(window, 82, eKI_NP_0);
		addKey(window, 83, eKI_NP_1);
		addKey(window, 84, eKI_NP_2);
		addKey(window, 85, eKI_NP_3);
		addKey(window, 86, eKI_NP_4);
		addKey(window, 87, eKI_NP_5);
		addKey(window, 88, eKI_NP_6);
		addKey(window, 89, eKI_NP_7);
		addKey(window, 91, eKI_NP_8);
		addKey(window, 92, eKI_NP_9);
		addKey(window, 96, eKI_F5);
		addKey(window, 97, eKI_F6);
		addKey(window, 98, eKI_F7);
		addKey(window, 99, eKI_F3);
		addKey(window, 100, eKI_F8);
		addKey(window, 101, eKI_F9);
		addKey(window, 103, eKI_F11);
		addKey(window, 105, eKI_F13);
		addKey(window, 107, eKI_F14);
		addKey(window, 109, eKI_F10);
		addKey(window, 111, eKI_F12);
		addKey(window, 113, eKI_F15);
		addKey(window, 118, eKI_F4);
		addKey(window, 120, eKI_F2);
		addKey(window, 122, eKI_F1);

		for (uint32 i = 0; i < 8; ++i)
		{
			window->mouseButtons[i] = new SInputSymbol(
				i, static_cast<EKeyId>(eKI_Mouse1 + i), SInputSymbol::Button);
			window->mouseButtons[i]->deviceId = eDI_Mouse;
			window->pressedMouseButtons[i] = false;
		}
		window->mouseXSymbol =
			new SInputSymbol(0, eKI_MouseX, SInputSymbol::RawAxis);
		window->mouseYSymbol =
			new SInputSymbol(1, eKI_MouseY, SInputSymbol::RawAxis);
		window->mouseZSymbol =
			new SInputSymbol(2, eKI_MouseZ, SInputSymbol::RawAxis);
		window->mouseWheelUpSymbol =
			new SInputSymbol(3, eKI_MouseWheelUp, SInputSymbol::Button);
		window->mouseWheelDownSymbol =
			new SInputSymbol(4, eKI_MouseWheelDown, SInputSymbol::Button);
		window->mouseXSymbol->deviceId = eDI_Mouse;
		window->mouseYSymbol->deviceId = eDI_Mouse;
		window->mouseZSymbol->deviceId = eDI_Mouse;
		window->mouseWheelUpSymbol->deviceId = eDI_Mouse;
		window->mouseWheelDownSymbol->deviceId = eDI_Mouse;
	}

	int inputModifiers(const gkVkMacWindow* window, NSEvent* event)
	{
		int modifiers = eMM_None;
		if (window->pressedKeys.count(59))
			modifiers |= eMM_LCtrl;
		if (window->pressedKeys.count(62))
			modifiers |= eMM_RCtrl;
		if (window->pressedKeys.count(56))
			modifiers |= eMM_LShift;
		if (window->pressedKeys.count(60))
			modifiers |= eMM_RShift;
		if (window->pressedKeys.count(58))
			modifiers |= eMM_LAlt;
		if (window->pressedKeys.count(61))
			modifiers |= eMM_RAlt;
		if (window->pressedKeys.count(55))
			modifiers |= eMM_LWin;
		if (window->pressedKeys.count(54))
			modifiers |= eMM_RWin;
		if (event.modifierFlags & NSEventModifierFlagCapsLock)
			modifiers |= eMM_CapsLock;
		return modifiers;
	}

	void postButton(gkVkMacWindow* window, SInputSymbol* symbol, bool pressed,
		NSEvent* nativeEvent)
	{
		if (!symbol || !gEnv || !gEnv->pInputManager)
			return;
		symbol->PressEvent(pressed);
		SInputEvent event;
		symbol->AssignTo(event, inputModifiers(window, nativeEvent));
		event.timestamp =
			static_cast<uint32>(nativeEvent.timestamp * 1000.0);
		gEnv->pInputManager->SetModifiers(event.modifiers);
		if (traceInput())
			fprintf(stderr, "[gkInput/macOS] key=%u state=%s modifiers=0x%x\n",
				static_cast<uint32>(event.keyId),
				pressed ? "pressed" : "released", event.modifiers);
		gEnv->pInputManager->PostInputEvent(event);
	}

	void postAxis(gkVkMacWindow* window, SInputSymbol* symbol, float delta,
		float position, NSEvent* nativeEvent)
	{
		if (!symbol || !gEnv || !gEnv->pInputManager)
			return;
		symbol->state = eIS_Changed;
		symbol->value = delta;
		symbol->value2 = position;
		SInputEvent event;
		symbol->AssignTo(event, inputModifiers(window, nativeEvent));
		event.timestamp =
			static_cast<uint32>(nativeEvent.timestamp * 1000.0);
		gEnv->pInputManager->SetModifiers(event.modifiers);
		if (traceInput() && delta != 0.0f)
			fprintf(stderr,
				"[gkInput/macOS] axis=%u delta=%.3f position=%.3f\n",
				static_cast<uint32>(event.keyId), delta, position);
		gEnv->pInputManager->PostInputEvent(event);
	}

	void releaseAllInput(gkVkMacWindow* window, NSEvent* nativeEvent)
	{
		std::set<unsigned short> pressedKeys = window->pressedKeys;
		for (std::set<unsigned short>::const_iterator it = pressedKeys.begin();
			it != pressedKeys.end(); ++it)
		{
			std::map<unsigned short, SInputSymbol*>::iterator symbol =
				window->keySymbols.find(*it);
			window->pressedKeys.erase(*it);
			if (symbol != window->keySymbols.end())
				postButton(window, symbol->second, false, nativeEvent);
		}
		window->pressedKeys.clear();
		for (uint32 i = 0; i < 8; ++i)
		{
			if (window->pressedMouseButtons[i])
				postButton(window, window->mouseButtons[i], false, nativeEvent);
			window->pressedMouseButtons[i] = false;
		}
		window->hasMousePosition = false;
	}

	void handleKeyboardEvent(gkVkMacWindow* window, NSEvent* event)
	{
		const unsigned short keyCode = event.keyCode;
		std::map<unsigned short, SInputSymbol*>::iterator symbol =
			window->keySymbols.find(keyCode);
		if (symbol == window->keySymbols.end())
			return;

		if (event.type == NSEventTypeFlagsChanged)
		{
			const bool pressed = window->pressedKeys.count(keyCode) == 0;
			if (pressed)
				window->pressedKeys.insert(keyCode);
			else
				window->pressedKeys.erase(keyCode);
			postButton(window, symbol->second, pressed, event);
			return;
		}

		const bool pressed = event.type == NSEventTypeKeyDown;
		if (pressed)
		{
			if (event.isARepeat || window->pressedKeys.count(keyCode))
				return;
			window->pressedKeys.insert(keyCode);
		}
		else
		{
			if (!window->pressedKeys.count(keyCode))
				return;
			window->pressedKeys.erase(keyCode);
		}
		postButton(window, symbol->second, pressed, event);
	}

	void handleMouseMove(gkVkMacWindow* window, NSEvent* event)
	{
		NSPoint point = [window->view convertPoint:event.locationInWindow
			fromView:nil];
		const NSRect logicalBounds = window->view.bounds;
		const float x = static_cast<float>(point.x);
		const float y =
			static_cast<float>(logicalBounds.size.height - point.y);
		const float deltaX = window->hasMousePosition ?
			x - window->mouseX : 0.0f;
		const float deltaY = window->hasMousePosition ?
			y - window->mouseY : 0.0f;
		window->hasMousePosition = true;
		window->mouseX = x;
		window->mouseY = y;
		postAxis(window, window->mouseXSymbol, deltaX, x, event);
		postAxis(window, window->mouseYSymbol, deltaY, y, event);
	}

	void handleMouseButton(gkVkMacWindow* window, NSEvent* event, bool pressed)
	{
		const NSInteger button = event.buttonNumber;
		if (button < 0 || button >= 8)
			return;
		handleMouseMove(window, event);
		if (window->pressedMouseButtons[button] == pressed)
			return;
		window->pressedMouseButtons[button] = pressed;
		postButton(window, window->mouseButtons[button], pressed, event);
	}

	void handleScroll(gkVkMacWindow* window, NSEvent* event)
	{
		const float delta = static_cast<float>(event.scrollingDeltaY);
		if (delta == 0.0f)
			return;
		postAxis(window, window->mouseZSymbol, delta, 0.0f, event);
		SInputSymbol* wheel = delta > 0.0f ?
			window->mouseWheelUpSymbol : window->mouseWheelDownSymbol;
		postButton(window, wheel, true, event);
		postButton(window, wheel, false, event);
	}

	void handleInputEvent(gkVkMacWindow* window, NSEvent* event)
	{
		switch (event.type)
		{
		case NSEventTypeKeyDown:
		case NSEventTypeKeyUp:
		case NSEventTypeFlagsChanged:
			handleKeyboardEvent(window, event);
			break;
		case NSEventTypeMouseMoved:
		case NSEventTypeLeftMouseDragged:
		case NSEventTypeRightMouseDragged:
		case NSEventTypeOtherMouseDragged:
			handleMouseMove(window, event);
			break;
		case NSEventTypeLeftMouseDown:
		case NSEventTypeRightMouseDown:
		case NSEventTypeOtherMouseDown:
			handleMouseButton(window, event, true);
			break;
		case NSEventTypeLeftMouseUp:
		case NSEventTypeRightMouseUp:
		case NSEventTypeOtherMouseUp:
			handleMouseButton(window, event, false);
			break;
		case NSEventTypeScrollWheel:
			handleScroll(window, event);
			break;
		default:
			break;
		}
	}
}

gkVkMacWindow* gkVkMacCreateWindow(uint32_t width, uint32_t height)
{
	@autoreleasepool
	{
		NSApplication* application = [NSApplication sharedApplication];
		[application setActivationPolicy:NSApplicationActivationPolicyRegular];
		[application finishLaunching];

		const NSRect frame = NSMakeRect(0, 0, width, height);
		const NSWindowStyleMask style =
			NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
			NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
		NSWindow* nativeWindow = [[NSWindow alloc]
			initWithContentRect:frame
			styleMask:style
			backing:NSBackingStoreBuffered
			defer:NO];
		if (!nativeWindow)
			return nullptr;

		NSView* view = [nativeWindow contentView];
		[view setWantsLayer:YES];
		CAMetalLayer* metalLayer = [CAMetalLayer layer];
		metalLayer.framebufferOnly = YES;
		[view setLayer:metalLayer];

		[nativeWindow setTitle:@"gkENGINE Vulkan"];
		[nativeWindow setAcceptsMouseMovedEvents:YES];
		[nativeWindow center];
		[nativeWindow makeKeyAndOrderFront:nil];
		[application activateIgnoringOtherApps:YES];

		gkVkMacWindow* result = new gkVkMacWindow;
		result->window = nativeWindow;
		result->view = view;
		result->layer = metalLayer;
		result->fullscreen = false;
		result->hadFocus = true;
		result->hasMousePosition = false;
		result->mouseX = 0.0f;
		result->mouseY = 0.0f;
		result->backingScale = 0.0f;
		result->drawableWidth = 0;
		result->drawableHeight = 0;
		createInputSymbols(result);
		updateDrawableMetrics(result);
		return result;
	}
}

void gkVkMacDestroyWindow(gkVkMacWindow* window)
{
	if (!window)
		return;
	releaseAllInput(window, [NSApp currentEvent]);
	for (std::map<unsigned short, SInputSymbol*>::iterator it =
		window->keySymbols.begin(); it != window->keySymbols.end(); ++it)
		delete it->second;
	for (uint32 i = 0; i < 8; ++i)
		delete window->mouseButtons[i];
	delete window->mouseXSymbol;
	delete window->mouseYSymbol;
	delete window->mouseZSymbol;
	delete window->mouseWheelUpSymbol;
	delete window->mouseWheelDownSymbol;
	@autoreleasepool
	{
		[window->window orderOut:nil];
		[window->window close];
		window->layer = nil;
		window->view = nil;
		window->window = nil;
	}
	delete window;
}

void* gkVkMacMetalLayer(gkVkMacWindow* window)
{
	return window ? (__bridge void*)window->layer : nullptr;
}

bool gkVkMacPumpEvents(gkVkMacWindow* window, uint32_t* width,
	uint32_t* height, bool* minimized)
{
	if (!window)
		return false;
	@autoreleasepool
	{
		NSApplication* application = [NSApplication sharedApplication];
		for (;;)
		{
			NSEvent* event = [application
				nextEventMatchingMask:NSEventMaskAny
				untilDate:[NSDate distantPast]
				inMode:NSDefaultRunLoopMode
				dequeue:YES];
			if (!event)
				break;
			handleInputEvent(window, event);
			if (event.type == NSEventTypeKeyDown &&
				(event.modifierFlags & NSEventModifierFlagCommand) &&
				[[event charactersIgnoringModifiers] isEqualToString:@"q"])
				[window->window performClose:nil];
			[application sendEvent:event];
		}
		[application updateWindows];
		const bool hasFocus =
			application.active && window->window.keyWindow;
		if (window->hadFocus && !hasFocus)
			releaseAllInput(window, [application currentEvent]);
		window->hadFocus = hasFocus;

		updateDrawableMetrics(window);
		if (width)
			*width = window->drawableWidth;
		if (height)
			*height = window->drawableHeight;
		if (minimized)
			*minimized = window->window.miniaturized ||
				window->drawableWidth == 0 || window->drawableHeight == 0;
		return window->window.visible;
	}
}

float gkVkMacBackingScale(gkVkMacWindow* window)
{
	if (!window)
		return 1.0f;
	updateDrawableMetrics(window);
	return window->backingScale;
}

void gkVkMacDrawableSize(gkVkMacWindow* window, uint32_t* width,
	uint32_t* height)
{
	if (!window)
		return;
	updateDrawableMetrics(window);
	if (width)
		*width = window->drawableWidth;
	if (height)
		*height = window->drawableHeight;
}

void gkVkMacSetFullscreen(gkVkMacWindow* window, bool fullscreen)
{
	if (!window || window->fullscreen == fullscreen)
		return;
	[window->window toggleFullScreen:nil];
	window->fullscreen = fullscreen;
}

void gkVkMacSetWindowSize(gkVkMacWindow* window, uint32_t width, uint32_t height)
{
	if (!window || !width || !height)
		return;
	[window->window setContentSize:NSMakeSize(width, height)];
}

#if defined(GK_VULKAN_INPUT_TESTING)
void gkVkMacPostTestKey(uint16_t keyCode, bool pressed)
{
	NSWindow* window = [NSApp keyWindow];
	const bool modifierKey =
		keyCode == 54 || keyCode == 55 || (keyCode >= 56 && keyCode <= 62);
	NSEventModifierFlags modifierFlags = 0;
	if (pressed)
	{
		if (keyCode == 54 || keyCode == 55)
			modifierFlags = NSEventModifierFlagCommand;
		else if (keyCode == 56 || keyCode == 60)
			modifierFlags = NSEventModifierFlagShift;
		else if (keyCode == 57)
			modifierFlags = NSEventModifierFlagCapsLock;
		else if (keyCode == 58 || keyCode == 61)
			modifierFlags = NSEventModifierFlagOption;
		else if (keyCode == 59 || keyCode == 62)
			modifierFlags = NSEventModifierFlagControl;
	}
	NSEvent* event = [NSEvent keyEventWithType:(modifierKey ?
		NSEventTypeFlagsChanged :
		(pressed ? NSEventTypeKeyDown : NSEventTypeKeyUp))
		location:NSZeroPoint
		modifierFlags:modifierFlags
		timestamp:NSProcessInfo.processInfo.systemUptime
		windowNumber:window.windowNumber
		context:nil
		characters:@""
		charactersIgnoringModifiers:@""
		isARepeat:NO
		keyCode:keyCode];
	[NSApp postEvent:event atStart:NO];
}

void gkVkMacPostTestMouseMove(float x, float y)
{
	NSWindow* window = [NSApp keyWindow];
	NSEvent* event = [NSEvent mouseEventWithType:NSEventTypeMouseMoved
		location:NSMakePoint(x, y)
		modifierFlags:0
		timestamp:NSProcessInfo.processInfo.systemUptime
		windowNumber:window.windowNumber
		context:nil
		eventNumber:0
		clickCount:0
		pressure:0.0];
	[NSApp postEvent:event atStart:NO];
}

void gkVkMacPostTestMouseButton(
	uint8_t button, bool pressed, float x, float y)
{
	NSWindow* window = [NSApp keyWindow];
	NSEventType type;
	if (button == 0)
		type = pressed ? NSEventTypeLeftMouseDown : NSEventTypeLeftMouseUp;
	else if (button == 1)
		type = pressed ? NSEventTypeRightMouseDown : NSEventTypeRightMouseUp;
	else
		type = pressed ? NSEventTypeOtherMouseDown : NSEventTypeOtherMouseUp;
	NSEvent* event = [NSEvent mouseEventWithType:type
		location:NSMakePoint(x, y)
		modifierFlags:0
		timestamp:NSProcessInfo.processInfo.systemUptime
		windowNumber:window.windowNumber
		context:nil
		eventNumber:0
		clickCount:1
		pressure:(pressed ? 1.0 : 0.0)];
	[NSApp postEvent:event atStart:NO];
}
#endif
