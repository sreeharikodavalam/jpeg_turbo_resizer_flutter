#ifndef FLUTTER_PLUGIN_JPEG_TURBO_RESIZER_PLUGIN_H_
#define FLUTTER_PLUGIN_JPEG_TURBO_RESIZER_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>
#include "exif/exif.h"

namespace jpeg_turbo_resizer {

class JpegTurboResizerPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  JpegTurboResizerPlugin();

  virtual ~JpegTurboResizerPlugin();

  // Disallow copy and assign.
  JpegTurboResizerPlugin(const JpegTurboResizerPlugin&) = delete;
  JpegTurboResizerPlugin& operator=(const JpegTurboResizerPlugin&) = delete;

  // Method to handle the method call from Dart
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

  // The actual resizing function (now public)
  static int resize_jpeg(const char* input_path, const char* output_path, int max_resolution);

 private:
    static unsigned char* flip_horizontally(unsigned char* buffer, int width, int height);
    static unsigned char* flip_vertically(unsigned char* buffer, int width, int height);
    static unsigned char* rotate_90_cw(unsigned char* buffer, int width, int height);
    static unsigned char* rotate_90_ccw(unsigned char* buffer, int width, int height);
    static unsigned char* rotate_180(unsigned char* buffer, int width, int height);
    static unsigned char* rotate_90_cw_and_flip(unsigned char* buffer, int width, int height);

    // ... any private members ...
};

}  // namespace jpeg_turbo_resizer

#endif  // FLUTTER_PLUGIN_JPEG_TURBO_RESIZER_PLUGIN_H_