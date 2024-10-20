import 'dart:io';

import 'package:flutter/material.dart';
import 'package:jpeg_turbo_resizer/jpeg_turbo_resizer_method_channel.dart';
import 'dart:async';
import 'package:path_provider/path_provider.dart';
import 'package:flutter/services.dart';
import 'package:jpeg_turbo_resizer/jpeg_turbo_resizer.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  String _platformVersion = 'Unknown';
  final _jpegTurboResizerPlugin = JpegTurboResizer();

  MethodChannelJpegTurboResizer platform = MethodChannelJpegTurboResizer();
  @override
  void initState() {
    super.initState();
    initPlatformState();
  }

  // Platform messages are asynchronous, so we initialize in an async method.
  Future<void> initPlatformState() async {
    String platformVersion;
    // Platform messages may fail, so we use a try/catch PlatformException.
    // We also handle the message potentially returning null.
    try {
      platformVersion =  await platform.getPlatformVersion() ?? 'Unknown platform version';
    } on PlatformException {
      platformVersion = 'Failed to get platform version.';
    }

    // If the widget was removed from the tree while the asynchronous platform
    // message was in flight, we want to discard the reply rather than calling
    // setState to update our non-existent appearance.
    if (!mounted) return;

    setState(() {
      _platformVersion = platformVersion;
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: ResizeImageWidget(),
      ),
    );
  }
}


class ResizeImageWidget extends StatefulWidget {
  @override
  _ResizeImageWidgetState createState() => _ResizeImageWidgetState();
}

class _ResizeImageWidgetState extends State<ResizeImageWidget> {
  String? _resizedImagePath;

  Future<void> _resizeImage() async {
    final directory = await getApplicationDocumentsDirectory();
    final inputPath = '${directory.path}/TEMP_FOLDER/JKP05745.JPG';
    final outputPath = '${directory.path}TEMP_FOLDER/JKP05745-${DateTime.now().millisecond}.JPG';

    // For this example, we're assuming an input image exists.
    // In a real app, you'd probably get this from the user or camera.
    JpegTurboResizer turboResizer = JpegTurboResizer();
    final result = await turboResizer.resizeJpeg(
      inputPath,
      outputPath,
      300,  // Max resolution
    );

    if (result == 0) {
      setState(() {
        _resizedImagePath = outputPath;
      });
    } else {
      print('Error resizing image: $result');
    }
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        ElevatedButton(
          onPressed: _resizeImage,
          child: Text('Resize Image'),
        ),
        if (_resizedImagePath != null)
          Image.file(File(_resizedImagePath!))
      ],
    );
  }
}