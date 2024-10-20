import 'package:flutter_test/flutter_test.dart';
import 'package:jpeg_turbo_resizer/jpeg_turbo_resizer.dart';
import 'package:jpeg_turbo_resizer/jpeg_turbo_resizer_platform_interface.dart';
import 'package:jpeg_turbo_resizer/jpeg_turbo_resizer_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockJpegTurboResizerPlatform
    with MockPlatformInterfaceMixin
    implements JpegTurboResizerPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final JpegTurboResizerPlatform initialPlatform = JpegTurboResizerPlatform.instance;

  test('$MethodChannelJpegTurboResizer is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelJpegTurboResizer>());
  });

  test('getPlatformVersion', () async {
    JpegTurboResizer jpegTurboResizerPlugin = JpegTurboResizer();
    MockJpegTurboResizerPlatform fakePlatform = MockJpegTurboResizerPlatform();
    JpegTurboResizerPlatform.instance = fakePlatform;

    expect(await jpegTurboResizerPlugin.getPlatformVersion(), '42');
  });
}
