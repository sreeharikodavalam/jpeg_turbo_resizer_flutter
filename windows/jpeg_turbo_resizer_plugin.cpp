#include "jpeg_turbo_resizer_plugin.h"
#include <flutter/plugin_registrar_windows.h>
#include <flutter/method_channel.h>
#include <flutter/standard_method_codec.h>
#include <memory>
#include <sstream>
#include "exif/exif.h"
#include <turbojpeg.h>

namespace jpeg_turbo_resizer {

    JpegTurboResizerPlugin::JpegTurboResizerPlugin() {}

    JpegTurboResizerPlugin::~JpegTurboResizerPlugin() {}

    void JpegTurboResizerPlugin::RegisterWithRegistrar(
            flutter::PluginRegistrarWindows *registrar) {
        auto channel =
                std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
                        registrar->messenger(), "jpeg_turbo_resizer",
                                &flutter::StandardMethodCodec::GetInstance());

        auto plugin = std::make_unique<JpegTurboResizerPlugin>();

        channel->SetMethodCallHandler(
                [plugin_pointer = plugin.get()](const auto &call, auto result) {
                    plugin_pointer->HandleMethodCall(call, std::move(result));
                });

        registrar->AddPlugin(std::move(plugin));
    }

    void JpegTurboResizerPlugin::HandleMethodCall(
            const flutter::MethodCall<flutter::EncodableValue> &method_call,
            std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
        if (method_call.method_name().compare("resizeJpeg") == 0) {
            const auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
            if (arguments) {
                auto input_path = std::get<std::string>(arguments->at(flutter::EncodableValue("inputPath")));
                auto output_path = std::get<std::string>(arguments->at(flutter::EncodableValue("outputPath")));
                auto max_resolution = std::get<int>(arguments->at(flutter::EncodableValue("maxResolution")));

                int resize_result = resize_jpeg(input_path.c_str(), output_path.c_str(), max_resolution);
                if (resize_result == 0) {
                    result->Success(flutter::EncodableValue(true));
                } else {
                    result->Error("RESIZE_FAILED", "Failed to resize image", flutter::EncodableValue(resize_result));
                }
            } else {
                result->Error("INVALID_ARGUMENTS", "Invalid arguments");
            }
        } else {
            result->NotImplemented();
        }
    }

    int JpegTurboResizerPlugin::resize_jpeg(const char* input_path, const char* output_path, int max_resolution) {
        tjhandle handle = NULL;
        tjhandle compressHandle = NULL;
        FILE* infile = NULL;
        unsigned char* input_buffer = NULL;
        unsigned char* rgb_buffer = NULL;
        unsigned char* resized_buffer = NULL;
        unsigned char* output_buffer = NULL;
        int result = 0;

        easyexif::EXIFInfo exif_info;
        int exif_orientation = 1;  // Default orientation (no rotation)

        if (!input_path || !output_path || max_resolution <= 0) {
            return -1;
        }

        do {
            handle = tjInitDecompress();
            compressHandle = tjInitCompress();
            if (!handle || !compressHandle) {
                result = -2;
                break;
            }

            infile = fopen(input_path, "rb");
            if (!infile) {
                result = -3;
                break;
            }

            if (fseek(infile, 0, SEEK_END) != 0) {
                result = -4;
                break;
            }
            long input_size = ftell(infile);
            if (input_size <= 0) {
                result = -5;
                break;
            }
            if (fseek(infile, 0, SEEK_SET) != 0) {
                result = -6;
                break;
            }

            input_buffer = (unsigned char*)malloc(input_size);
            if (!input_buffer) {
                result = -7;
                break;
            }

            size_t bytes_read = fread(input_buffer, 1, input_size, infile);
            if (bytes_read != (size_t)input_size) {
                result = -8;
                break;
            }
            fclose(infile);
            infile = NULL;

            // Read EXIF data
            exif_info.parseFrom(input_buffer, input_size);
            exif_orientation = exif_info.Orientation;

            int width, height, subsamp, colorspace;
            if (tjDecompressHeader3(handle, input_buffer, input_size, &width, &height, &subsamp, &colorspace) < 0) {
                result = -9;
                break;
            }

            if (width <= max_resolution && height <= max_resolution) {
                FILE* outfile = fopen(output_path, "wb");
                if (!outfile) {
                    result = -10;
                    break;
                }
                size_t bytes_written = fwrite(input_buffer, 1, input_size, outfile);
                fclose(outfile);
                if (bytes_written != (size_t)input_size) {
                    result = -11;
                    break;
                }
                break;
            }

            int target_width, target_height;
            if (width > height) {
                target_width = max_resolution;
                target_height = (height * max_resolution) / width;
            } else {
                target_height = max_resolution;
                target_width = (width * max_resolution) / height;
            }

            target_width = (target_width < 1) ? 1 : target_width;
            target_height = (target_height < 1) ? 1 : target_height;

            rgb_buffer = (unsigned char*)malloc(width * height * 3);
            if (!rgb_buffer) {
                result = -12;
                break;
            }

            if (tjDecompress2(handle, input_buffer, input_size, rgb_buffer, width, 0, height, TJPF_RGB, TJFLAG_FASTDCT) < 0) {
                result = -13;
                break;
            }

            resized_buffer = (unsigned char*)malloc(target_width * target_height * 3);
            if (!resized_buffer) {
                result = -14;
                break;
            }

            for (int y = 0; y < target_height; y++) {
                for (int x = 0; x < target_width; x++) {
                    int src_x = x * width / target_width;
                    int src_y = y * height / target_height;
                    for (int c = 0; c < 3; c++) {
                        resized_buffer[(y * target_width + x) * 3 + c] = rgb_buffer[(src_y * width + src_x) * 3 + c];
                    }
                }
            }

            unsigned char* oriented_buffer = NULL;
            int oriented_width = target_width;
            int oriented_height = target_height;

            switch (exif_orientation) {
                case 2: // Flip horizontally
                    oriented_buffer = flip_horizontally(resized_buffer, target_width, target_height);
                    break;
                case 3: // Rotate 180 degrees
                    oriented_buffer = rotate_180(resized_buffer, target_width, target_height);
                    break;
                case 4: // Flip vertically
                    oriented_buffer = flip_vertically(resized_buffer, target_width, target_height);
                    break;
                case 5: // Rotate 90 degrees clockwise and flip vertically
                    oriented_buffer = rotate_90_cw_and_flip(resized_buffer, target_width, target_height);
                    oriented_width = target_height;
                    oriented_height = target_width;
                    break;
                case 6: // Rotate 90 degrees clockwise
                    oriented_buffer = rotate_90_cw(resized_buffer, target_width, target_height);
                    oriented_width = target_height;
                    oriented_height = target_width;
                    break;
                case 7: // Rotate 90 degrees clockwise and flip horizontally
                    oriented_buffer = rotate_90_cw_and_flip(resized_buffer, target_width, target_height);
                    oriented_width = target_height;
                    oriented_height = target_width;
                    break;
                case 8: // Rotate 90 degrees counterclockwise
                    oriented_buffer = rotate_90_ccw(resized_buffer, target_width, target_height);
                    oriented_width = target_height;
                    oriented_height = target_width;
                    break;
                default: // No orientation change needed
                    oriented_buffer = resized_buffer;
                    break;
            }

            unsigned long output_size = 0;
            if (tjCompress2(compressHandle, oriented_buffer, oriented_width, 0, oriented_height, TJPF_RGB,
                            &output_buffer, &output_size, subsamp, 100, TJFLAG_ACCURATEDCT) < 0) {
                result = -15;
                break;
            }

            FILE* outfile = fopen(output_path, "wb");
            if (!outfile) {
                result = -16;
                break;
            }
            size_t bytes_written = fwrite(output_buffer, 1, output_size, outfile);
            fclose(outfile);
            if (bytes_written != output_size) {
                result = -17;
                break;
            }

            if (oriented_buffer != resized_buffer) {
                free(oriented_buffer);
            }

        } while (0);

        if (infile) fclose(infile);
        if (input_buffer) free(input_buffer);
        if (rgb_buffer) free(rgb_buffer);
        if (resized_buffer) free(resized_buffer);
        if (output_buffer) tjFree(output_buffer);
        if (handle) tjDestroy(handle);
        if (compressHandle) tjDestroy(compressHandle);

        return result;
    }

    unsigned char* JpegTurboResizerPlugin::flip_horizontally(unsigned char* buffer, int width, int height) {
        unsigned char* result = (unsigned char*)malloc(width * height * 3);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                for (int c = 0; c < 3; c++) {
                    result[(y * width + x) * 3 + c] = buffer[(y * width + (width - 1 - x)) * 3 + c];
                }
            }
        }
        return result;
    }

    unsigned char* JpegTurboResizerPlugin::flip_vertically(unsigned char* buffer, int width, int height) {
        unsigned char* result = (unsigned char*)malloc(width * height * 3);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                for (int c = 0; c < 3; c++) {
                    result[(y * width + x) * 3 + c] = buffer[((height - 1 - y) * width + x) * 3 + c];
                }
            }
        }
        return result;
    }

    unsigned char* JpegTurboResizerPlugin::rotate_90_cw(unsigned char* buffer, int width, int height) {
        unsigned char* result = (unsigned char*)malloc(width * height * 3);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                for (int c = 0; c < 3; c++) {
                    result[(x * height + (height - 1 - y)) * 3 + c] = buffer[(y * width + x) * 3 + c];
                }
            }
        }
        return result;
    }

    unsigned char* JpegTurboResizerPlugin::rotate_90_ccw(unsigned char* buffer, int width, int height) {
        unsigned char* result = (unsigned char*)malloc(width * height * 3);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                for (int c = 0; c < 3; c++) {
                    result[((width - 1 - x) * height + y) * 3 + c] = buffer[(y * width + x) * 3 + c];
                }
            }
        }
        return result;
    }

    unsigned char* JpegTurboResizerPlugin::rotate_180(unsigned char* buffer, int width, int height) {
        unsigned char* result = (unsigned char*)malloc(width * height * 3);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                for (int c = 0; c < 3; c++) {
                    result[(y * width + x) * 3 + c] = buffer[((height - 1 - y) * width + (width - 1 - x)) * 3 + c];
                }
            }
        }
        return result;
    }

    unsigned char* JpegTurboResizerPlugin::rotate_90_cw_and_flip(unsigned char* buffer, int width, int height) {
        unsigned char* rotated = rotate_90_cw(buffer, width, height);
        unsigned char* result = flip_horizontally(rotated, height, width);
        free(rotated);
        return result;
    }

}  // namespace jpeg_turbo_resizer