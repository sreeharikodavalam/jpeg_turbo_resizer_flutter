#include "include/jpeg_turbo_resizer/jpeg_turbo_resizer_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "jpeg_turbo_resizer_plugin.h"

void JpegTurboResizerPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  jpeg_turbo_resizer::JpegTurboResizerPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}

FLUTTER_PLUGIN_EXPORT int resize_jpeg(const char* input_path, const char* output_path, int max_resolution) {
  return jpeg_turbo_resizer::JpegTurboResizerPlugin::resize_jpeg(input_path, output_path, max_resolution);
}