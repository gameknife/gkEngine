# gkRendererVulkan

This module is the Windows Vulkan backend described by
`docs/gkVulkanRenderer_Design.md`.

## Current implementation status

Windows P0, P1 and a usable forward-rendering subset of P2 are implemented:

- renderer DLL lifecycle and engine module selection;
- Vulkan 1.2 minimum capability checks;
- optional validation layer and debug messenger;
- physical-device and graphics/present queue selection;
- Win32 window and surface creation;
- swapchain creation and recreation;
- two frame contexts with fences, acquire semaphores and command buffers;
- per-swapchain-image present semaphores;
- dynamic-rendering clear frames using synchronization2 barriers;
- an offline GLSL to SPIR-V build with `spirv-val` verification;
- color and per-swapchain-image depth attachments with dynamic rendering;
- engine render-sequence and main-camera integration;
- engine VB/IB layouts, persistent static-buffer caching and indexed draws;
- `.mtl` parsing including sub-materials, diffuse color, UV tiling and texture channels;
- uncompressed/RLE 24/32-bit TGA plus 32-bit/DXT5 DDS decoding, image upload,
  descriptor caching and sampling;
- CPU skinning fallback for the legacy four-weight vertex layout;
- offline unlit/lit material pipelines, alpha-test, alpha blending, depth-write
  control, material double-sided culling and a procedural fallback pipeline;
- engine sun-direction propagation and local-space directional lighting;
- opaque-before-transparent submission;
- depth-independent screen-overlay rendering for Aux boxes and lines;
- FreeType HUD rendering with indexed dynamic geometry, per-glyph BGRA color,
  A8 atlas alpha sampling and incremental atlas uploads;
- persistent driver pipeline-cache data and optional GPU timestamp queries;
- camera picking-ray and world-to-screen projection helpers;
- resize, minimize/restore, out-of-date and suboptimal handling;
- a validation-enabled indexed/descriptor/depth smoke test.

The indoor scene and the documented testcase matrix start through
`gkLauncher.exe` with validation enabled. The current Windows build is suitable
for manual acceptance of static textured scenes and the forward fallback.
Terrain layer blending, shadows, deferred passes, post processing, GPU particles
and full animated-character parity remain future work, so the renderer does not
replace D3D9 as the Windows default.

## Build and validate

Use a Visual Studio developer command prompt with a Vulkan SDK installed:

```powershell
cmake -S . -B build-vulkan -G Ninja `
  -DCMAKE_BUILD_TYPE=RelWithDebInfo `
  -DGK_RENDERER_BACKEND=VULKAN `
  -DGK_VULKAN_VALIDATION=ON
cmake --build build-vulkan --target gkRendererVulkan gkVulkanSmoke gkFont
ctest --test-dir build-vulkan --output-on-failure
```

The renderer DLL is written to `exec/bin64/gkRendererVulkan.dll`. The Windows
build compiles the bundled FreeType sources for `gkFont`, avoiding the legacy
toolset-specific prebuilt library. `gkVulkanRuntimeShaders` also copies rebuilt
SPIR-V files into `exec/bin64` even when the renderer DLL itself does not need
relinking.

For an incremental shader-cache build outside CMake:

```powershell
python .\exec\tools\compile_vulkan_shaders.py `
  --source .\exec\engine\shaders\vulkan\source `
  --output .\build-vulkan\vulkan-shader-cache `
  --glslc "$env:VULKAN_SDK\Bin\glslc.exe" `
  --spirv-val "$env:VULKAN_SDK\Bin\spirv-val.exe"
```

The tool hashes every source, validates every SPIR-V module and writes a stable
`manifest.json`; runtime shader compilation is not used.

For visual acceptance, run:

```powershell
.\exec\bin64\gkVulkanSmoke.exe --interactive
```

A cyan indexed triangle using the engine vertex format should remain visible. Resize,
minimize and restore the window repeatedly; press Escape or close the window
to exit. Validation warnings and errors are written to stderr and the debugger.
