import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:jpeg_turbo_resizer/jpeg_turbo_resizer_method_channel.dart';

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();

  MethodChannelJpegTurboResizer platform = MethodChannelJpegTurboResizer();
  const MethodChannel channel = MethodChannel('jpeg_turbo_resizer');

  setUp(() {
    TestDefaultBinaryMessengerBinding.instance.defaultBinaryMessenger.setMockMethodCallHandler(
      channel,
      (MethodCall methodCall) async {
        return '42';
      },
    );
  });

  tearDown(() {
    TestDefaultBinaryMessengerBinding.instance.defaultBinaryMessenger.setMockMethodCallHandler(channel, null);
  });

  test('getPlatformVersion', () async {
    expect(await platform.getPlatformVersion(), '42');
  });
}
