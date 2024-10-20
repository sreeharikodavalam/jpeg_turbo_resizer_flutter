import 'dart:async';
import 'dart:ffi';
import 'dart:io';

import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

class JpegTurboResizer {
  static const MethodChannel _channel = MethodChannel('jpeg_turbo_resizer');

  static Future<String?> get platformVersion async {
    final String? version = await _channel.invokeMethod('getPlatformVersion');
    return version;
  }

  Future<bool> resizeJpeg(String inputPath, String outputPath, int maxResolution) async {
    try {
      final bool result = await _channel.invokeMethod('resizeJpeg', {
        'inputPath': inputPath,
        'outputPath': outputPath,
        'maxResolution': maxResolution,
      });
      return result;
    } catch (e) {
      print('Error resizing image: $e');
      return false;
    }
  }
}