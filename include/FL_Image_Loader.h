#ifndef FL_IMAGE_LOADER_H
#define FL_IMAGE_LOADER_H

#include <FL/Fl_RGB_Image.H>
#include <webp/decode.h>
//#include "stb_image.h"


std::string getFileType(const char* filename) {
    // Determine file type based on signature
    // Note that this implementation depends on Windows APIs
    // On non-Windows systems, you will need a different implementation

    // Convert filename to wide string
    std::wstring wFilename;
    size_t len = strlen(filename);
    wFilename.resize(len);
    mbstowcs(&wFilename[0], filename, len);

    HANDLE hFile = CreateFileW(
        wFilename.c_str(),       // file to open
        GENERIC_READ,             // open for reading
        FILE_SHARE_READ,          // share for reading
        NULL,                     // default security
        OPEN_EXISTING,            // existing file only
        FILE_ATTRIBUTE_NORMAL,    // normal file
        NULL                      // no attr. template
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        return ""; // Failed to open file
    }

    char signature[8] = { 0 };
    DWORD bytesRead = 0;

    if (ReadFile(hFile, signature, sizeof(signature), &bytesRead, NULL) == FALSE || bytesRead != sizeof(signature)) {
        CloseHandle(hFile);
        return ""; // Failed to read file
    }

    CloseHandle(hFile);

    if (signature[0] == 0x89 && signature[1] == 'P' && signature[2] == 'N' && signature[3] == 'G' &&
        signature[4] == '\r' && signature[5] == '\n' && signature[6] == 0x1A && signature[7] == '\n') {
        return "PNG";
    } else if (signature[0] == 0xFF && signature[1] == 0xD8 && signature[2] == 0xFF && signature[3] == 0xE0) {
        return "JPG";
    } else if (signature[0] == 'B' && signature[1] == 'M') {
        return "BMP";
    } else if (signature[0] == 'D' && signature[1] == 'D' && signature[2] == 'S' && signature[3] == ' ') {
        return "DDS";
    } else if (signature[2] == 0x02 && signature[3] == 0x00 && signature[12] == 0x00 && signature[13] == 0x00) {
        return "TGA";
    } else if (signature[0] == 'G' && signature[1] == 'I' && signature[2] == 'F' && signature[3] == '8') {
        return "GIF";
    } else if (signature[0] == 'R' && signature[1] == 'I' && signature[2] == 'F' && signature[3] == 'F' &&
               signature[8] == 'W' && signature[9] == 'E' && signature[10] == 'B' && signature[11] == 'P') {
        return "WEBP";
    }

    return "Unknown";
}



Fl_RGB_Image* createFallbackImage() {
    const int width = 8;
    const int height = 8;
    const int channels = 3;
    unsigned char* imageBuffer = new unsigned char[width * height * channels];

    // Fill the image with pink color
    for (int i = 0; i < width * height * channels; i += channels) {
        imageBuffer[i] = 255;   // Red
        imageBuffer[i + 1] = 0; // Green
        imageBuffer[i + 2] = 255; // Blue
    }

    // Draw diagonal black lines to form an X pattern
    for (int i = 0; i < width; ++i) {
        imageBuffer[(i * width + i) * channels] = 0;     // Red
        imageBuffer[(i * width + i) * channels + 1] = 0; // Green
        imageBuffer[(i * width + i) * channels + 2] = 0; // Blue

        imageBuffer[(i * width + (width - 1 - i)) * channels] = 0;     // Red
        imageBuffer[(i * width + (width - 1 - i)) * channels + 1] = 0; // Green
        imageBuffer[(i * width + (width - 1 - i)) * channels + 2] = 0; // Blue
    }

    // Caller is responsible for deleting the returned Fl_RGB_Image
    return new Fl_RGB_Image(imageBuffer, width, height, channels);
}

Fl_RGB_Image* loadImage(const char* filepath) {
    int width, height, channels;
    unsigned char* imageBuffer = nullptr;

    // Load image based on file type
    std::string fileType = getFileType(filepath);
    if (fileType == "WEBP") {
        // Load WEBP image using libwebp
        // Load WEBP image using libwebp
        WebPDecoderConfig config;
        if (WebPInitDecoderConfig(&config) != 1) {
            return createFallbackImage(); // Return fallback image on libwebp initialization failure
        }

        FILE* file = fopen(filepath, "rb");
        if (!file) {
            return createFallbackImage(); // Return fallback image if file fails to open
        }

        const size_t fileSize = 4096; // Adjust the buffer size as needed
        uint8_t* buffer = (uint8_t*)malloc(fileSize);
        if (!buffer) {
            fclose(file);
            return createFallbackImage(); // Return fallback image on buffer allocation failure
        }

        size_t bytesRead = fread(buffer, 1, fileSize, file);
        fclose(file);

        if (WebPDecode(buffer, bytesRead, &config) != VP8_STATUS_OK) {
            free(buffer);
            return createFallbackImage(); // Return fallback image on WEBP decoding failure
        }

        width = config.output.width;
        height = config.output.height;
        channels = 4; // RGBA

        imageBuffer = new unsigned char[width * height * channels];
        memcpy(imageBuffer, config.output.u.RGBA.rgba, width * height * channels);

        free(buffer);
        WebPFreeDecBuffer(&config.output);
    } else {
        // Load other image formats using stb_image
//        imageBuffer = stbi_load(filepath, &width, &height, &channels, 0);
        if (!imageBuffer) {
            return createFallbackImage(); // Return fallback image if image loading fails
        }
    }

    // Create FL_RGB_Image
    Fl_RGB_Image* flImage = new Fl_RGB_Image(imageBuffer, width, height, channels);

    // Clean up image buffer
    if (fileType == "WEBP") {
        delete[] imageBuffer;
    } else {
//        stbi_image_free(imageBuffer);
    }

    return flImage;
}


#endif // FL_IMAGE_LOADER_H
