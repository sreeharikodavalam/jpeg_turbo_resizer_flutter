#ifndef FLUTTER_PLUGIN_JPEG_TURBO_RESIZER_PLUGIN_C_API_H_
#define FLUTTER_PLUGIN_JPEG_TURBO_RESIZER_PLUGIN_C_API_H_

#include <flutter_plugin_registrar.h>

#ifdef FLUTTER_PLUGIN_IMPL
#define FLUTTER_PLUGIN_EXPORT __declspec(dllexport)
#else
#define FLUTTER_PLUGIN_EXPORT __declspec(dllimport)
#endif

#if defined(__cplusplus)
extern "C" {
#endif

FLUTTER_PLUGIN_EXPORT void JpegTurboResizerPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar);

FLUTTER_PLUGIN_EXPORT int resize_jpeg(const char* input_path, const char* output_path, int max_resolution);

#if defined(__cplusplus)
}  // extern "C"
#endif

#endif  // FLUTTER_PLUGIN_JPEG_TURBO_RESIZER_PLUGIN_C_API_H_