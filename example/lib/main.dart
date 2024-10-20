import 'dart:io';
import 'package:flutter/material.dart';
import 'package:jpeg_turbo_resizer/jpeg_turbo_resizer.dart';
import 'package:file_picker/file_picker.dart';
import 'package:path/path.dart' as path;

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'JPEG Turbo Resizer Example',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: JpegTurboResizerExample(),
    );
  }
}

class JpegTurboResizerExample extends StatefulWidget {
  @override
  _JpegTurboResizerExampleState createState() => _JpegTurboResizerExampleState();
}

class _JpegTurboResizerExampleState extends State<JpegTurboResizerExample> {
  String? _inputImagePath;
  String? _resizedImagePath;
  String? _destinationDirectory;
  String _outputFileName = '';
  int _maxResolution = 300;
  bool _isProcessing = false;
  final _formKey = GlobalKey<FormState>();

  Future<void> _pickImage() async {
    FilePickerResult? result = await FilePicker.platform.pickFiles(
      type: FileType.custom,
      allowedExtensions: ['jpg', 'jpeg', 'jp2', 'j2k', 'jfif'],
    );

    if (result != null) {
      final file = File(result.files.single.path!);
      setState(() {
        _inputImagePath = file.path;
        _destinationDirectory = path.dirname(file.path);
        _outputFileName = '${path.basenameWithoutExtension(file.path)}_resized${path.extension(file.path)}';
        _resizedImagePath = null; // Reset resized image when new input is selected
      });
    }
  }

  Future<void> _resizeImage() async {
    if (_formKey.currentState!.validate() && _inputImagePath != null) {
      setState(() {
        _isProcessing = true;
      });

      try {
        final outputPath = path.join(_destinationDirectory!, _outputFileName);

        JpegTurboResizer turboResizer = JpegTurboResizer();
        bool result = await turboResizer.resizeJpeg(
          _inputImagePath!,
          outputPath,
          _maxResolution,
        );

        if (result) {
          setState(() {
            _resizedImagePath = outputPath;
          });
          _showSnackBar('Image resized successfully!');
        } else {
          _showSnackBar('Error resizing image: $result');
        }
      } catch (e) {
        _showSnackBar('An error occurred: $e');
      } finally {
        setState(() {
          _isProcessing = false;
        });
      }
    }
  }

  void _showSnackBar(String message) {
    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(content: Text(message)),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('JPEG Turbo Resizer Example'),
      ),
      body: SingleChildScrollView(
        padding: EdgeInsets.all(16.0),
        child: Form(
          key: _formKey,
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              ElevatedButton.icon(
                onPressed: _isProcessing ? null : _pickImage,
                icon: Icon(Icons.image),
                label: Text('Select Image'),
              ),
              SizedBox(height: 16),
              if (_inputImagePath != null) ...[
                Text('Selected image:', style: TextStyle(fontWeight: FontWeight.bold)),
                Text(_inputImagePath!),
                SizedBox(height: 16),
                Image.file(
                  File(_inputImagePath!),
                  height: 200,
                  fit: BoxFit.cover,
                ),
                SizedBox(height: 16),
                Text('Destination directory:', style: TextStyle(fontWeight: FontWeight.bold)),
                Text(_destinationDirectory ?? ''),
                SizedBox(height: 16),
                TextFormField(
                  decoration: InputDecoration(
                    labelText: 'Output File Name',
                    border: OutlineInputBorder(),
                  ),
                  initialValue: _outputFileName,
                  validator: (value) {
                    if (value == null || value.isEmpty) {
                      return 'Please enter a file name';
                    }
                    if (!value.toLowerCase().endsWith('.jpg') &&
                        !value.toLowerCase().endsWith('.jpeg')) {
                      return 'File name must end with .jpg or .jpeg';
                    }
                    return null;
                  },
                  onChanged: (value) {
                    setState(() {
                      _outputFileName = value;
                    });
                  },
                ),
              ],
              SizedBox(height: 16),
              TextFormField(
                decoration: InputDecoration(
                  labelText: 'Max Resolution',
                  border: OutlineInputBorder(),
                ),
                initialValue: _maxResolution.toString(),
                keyboardType: TextInputType.number,
                validator: (value) {
                  if (value == null || value.isEmpty) {
                    return 'Please enter a max resolution';
                  }
                  if (int.tryParse(value) == null || int.parse(value) <= 0) {
                    return 'Please enter a valid positive number';
                  }
                  return null;
                },
                onChanged: (value) {
                  setState(() {
                    _maxResolution = int.tryParse(value) ?? 300;
                  });
                },
              ),
              SizedBox(height: 16),
              ElevatedButton.icon(
                onPressed: _isProcessing || _inputImagePath == null ? null : _resizeImage,
                icon: Icon(Icons.photo_size_select_large),
                label: Text('Resize Image'),
              ),
              SizedBox(height: 16),
              if (_isProcessing) Center(child: CircularProgressIndicator()),
              if (_resizedImagePath != null) ...[
                Text('Resized Image:', style: TextStyle(fontWeight: FontWeight.bold)),
                Text(_resizedImagePath!),
                SizedBox(height: 16),
                Image.file(
                  File(_resizedImagePath!),
                  height: 200,
                  fit: BoxFit.cover,
                ),
              ],
            ],
          ),
        ),
      ),
    );
  }
}