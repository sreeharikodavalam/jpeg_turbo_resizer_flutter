# JPEG Turbo Resizer Flutter Plugin

A custom Flutter plugin for Windows that enables fast and efficient image resizing using the libjpeg-turbo library. This plugin focuses on processing JPEG images with high performance.

## Features

- Fast JPEG image resizing
- Windows platform support
- Simple API for easy integration
- User-friendly image selection and output specification

## Supported File Types

This plugin uses libjpeg-turbo, which primarily supports JPEG file formats. Specifically, it can handle:

- JPEG (.jpg, .jpeg)
- JPEG 2000 (.jp2, .j2k) - read-only support
- JFIF (.jfif)

Note: While libjpeg-turbo can read some other formats like PPM, BMP, and TIFF, this plugin currently focuses on JPEG processing. Future updates may expand support for additional input formats.

## Installation

Add this to your package's `pubspec.yaml` file:

```yaml
dependencies:
  jpeg_turbo_resizer: ^0.0.1
  file_picker: ^5.2.10
  path_provider: ^2.0.15
```

Then run:

```
flutter pub get
```

## Usage

Import the necessary packages in your Dart code:

```dart
import 'package:jpeg_turbo_resizer/jpeg_turbo_resizer.dart';
import 'package:file_picker/file_picker.dart';
import 'package:path_provider/path_provider.dart';
```

To resize a JPEG image:

```dart
JpegTurboResizer turboResizer = JpegTurboResizer();
final result = await turboResizer.resizeJpeg(
  inputPath,
  outputPath,
  300,  // Max resolution
);

if (result == 0) {
  print('Image resized successfully!');
} else {
  print('Error resizing image: $result');
}
```

## Example

Here's an interactive widget that demonstrates how to use the plugin:

```dart
import 'dart:io';
import 'package:flutter/material.dart';
import 'package:jpeg_turbo_resizer/jpeg_turbo_resizer.dart';
import 'package:file_picker/file_picker.dart';
import 'package:path_provider/path_provider.dart';

class ResizeImageWidget extends StatefulWidget {
  @override
  _ResizeImageWidgetState createState() => _ResizeImageWidgetState();
}

class _ResizeImageWidgetState extends State<ResizeImageWidget> {
  String? _inputImagePath;
  String? _resizedImagePath;
  String _outputFileName = 'resized_image.jpg';
  final _formKey = GlobalKey<FormState>();

  Future<void> _pickImage() async {
    FilePickerResult? result = await FilePicker.platform.pickFiles(
      type: FileType.custom,
      allowedExtensions: ['jpg', 'jpeg', 'jp2', 'j2k', 'jfif'],
    );

    if (result != null) {
      setState(() {
        _inputImagePath = result.files.single.path;
      });
    }
  }

  Future<void> _resizeImage() async {
    if (_formKey.currentState!.validate() && _inputImagePath != null) {
      final directory = await getApplicationDocumentsDirectory();
      final outputPath = '${directory.path}/$_outputFileName';

      JpegTurboResizer turboResizer = JpegTurboResizer();
      final result = await turboResizer.resizeJpeg(
        _inputImagePath!,
        outputPath,
        300,  // Max resolution
      );

      if (result == 0) {
        setState(() {
          _resizedImagePath = outputPath;
        });
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Image resized successfully!')),
        );
      } else {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Error resizing image: $result')),
        );
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    // ... (rest of the widget build method remains the same)
  }
}
```

To use this widget, simply add it to your app's widget tree:

```dart
class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(title: Text('JPEG Turbo Resizer Example')),
        body: ResizeImageWidget(),
      ),
    );
  }
}
```

This example allows the user to:
1. Select an input JPEG image using a file picker.
2. Specify the output file name.
3. Resize the image and display the result.

## Platform Support

Currently, this plugin only supports Windows. Future updates may include support for additional platforms.

## Dependencies

This plugin requires the libjpeg-turbo library to be installed on the system. Make sure it's properly set up before using the plugin.

Additionally, this example uses the following Flutter packages:
- `file_picker`: For selecting input images.
- `path_provider`: For getting the application documents directory.

Make sure to add these dependencies to your `pubspec.yaml` file.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.