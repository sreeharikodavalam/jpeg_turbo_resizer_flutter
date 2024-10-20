import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'jpeg_turbo_resizer_platform_interface.dart';

/// An implementation of [JpegTurboResizerPlatform] that uses method channels.
class MethodChannelJpegTurboResizer extends JpegTurboResizerPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('jpeg_turbo_resizer');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }
}
