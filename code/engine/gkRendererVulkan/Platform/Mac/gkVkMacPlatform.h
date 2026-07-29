#pragma once

#include <stdint.h>

struct gkVkMacWindow;

gkVkMacWindow* gkVkMacCreateWindow(uint32_t width, uint32_t height);
void gkVkMacDestroyWindow(gkVkMacWindow* window);
void* gkVkMacMetalLayer(gkVkMacWindow* window);
bool gkVkMacPumpEvents(gkVkMacWindow* window, uint32_t* width,
	uint32_t* height, bool* minimized);
float gkVkMacBackingScale(gkVkMacWindow* window);
void gkVkMacDrawableSize(gkVkMacWindow* window, uint32_t* width,
	uint32_t* height);
void gkVkMacSetFullscreen(gkVkMacWindow* window, bool fullscreen);
void gkVkMacSetWindowSize(gkVkMacWindow* window, uint32_t width, uint32_t height);

#if defined(GK_VULKAN_INPUT_TESTING)
void gkVkMacPostTestKey(uint16_t keyCode, bool pressed);
void gkVkMacPostTestMouseMove(float x, float y);
void gkVkMacPostTestMouseButton(uint8_t button, bool pressed, float x, float y);
#endif
