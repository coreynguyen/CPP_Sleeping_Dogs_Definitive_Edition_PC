/*
 ________  ___  ___  ________  _________        ________  ________  _________        ___   ___
|\   ____\|\  \|\  \|\   __  \|\___   ___\     |\   ____\|\   __  \|\___   ___\     |\  \ |\  \
\ \  \___|\ \  \\\  \ \  \|\  \|___ \  \_|     \ \  \___|\ \  \|\  \|___ \  \_|     \ \  \\_\  \  _______
 \ \  \    \ \   __  \ \   __  \   \ \  \       \ \  \  __\ \   ____\   \ \  \       \ \______  \|\  ___ \
  \ \  \____\ \  \ \  \ \  \ \  \   \ \  \       \ \  \|\  \ \  \___|    \ \  \       \|_____|\  \ \ \__\ \
   \ \_______\ \__\ \__\ \__\ \__\   \ \__\       \ \_______\ \__\        \ \__\             \ \__\ \______\
    \|_______|\|__|\|__|\|__|\|__|    \|__|        \|_______|\|__|         \|__|              \|__|\|______|

*/
//#define STB_IMAGE_IMPLEMENTATION
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE // Only define if not already defined
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif


#include <iostream>
#include <algorithm>
#include <cctype>
#include <sstream>
#include "bytestream.h"
#include "BigInventoryFile.h"
#include "Filenames.h"
#include "version.h"
#include "resource.h"
#include "interface.h"
#include "QuickCompression.h"

#include <FL/Fl.H>
#include <FL/x.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Table_Row.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H>
#include "guiicon.h"
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <dwmapi.h> // Include the DWM API header for dark theme

using namespace std;

void printBigInventory(const BigInventory_t &bigInventory) {
    std::cout << "BigInventory Information:" << std::endl;
    std::cout << "Sort Key: " << bigInventory.m_SortKey << std::endl;
    std::cout << "Entry Count: " << bigInventory.m_EntryCount << std::endl;
    std::cout << "Entry Offset: " << bigInventory.m_EntryOffset << std::endl;
    std::cout << "File Pointer: " << bigInventory.m_FilePointer << std::endl;
    std::cout << "State: " << bigInventory.m_State << std::endl;
    std::cout << "Mount Index: " << bigInventory.m_MountIndex << std::endl;
    std::cout << "Archive Name: " << bigInventory.m_ArchiveName << std::endl;

    std::cout << "\nEntries:" << std::endl;
    for (const auto &entry : bigInventory.entry) {
        std::cout << "Name UID: " << entry.m_NameUID << std::endl;
        std::cout << "Offset: " << entry.m_Offset << std::endl;
        std::cout << "Compressed Size: " << entry.m_CompressedSize << std::endl;
        std::cout << "Uncompressed Size: " << entry.m_UncompressedSize << std::endl;
        std::cout << "Load Offset: " << entry.m_LoadOffset << std::endl;
        std::cout << "Compressed Extra: " << entry.m_CompressedExtra << std::endl;
        std::cout << "--------------------------" << std::endl;
        }
    }

// Function to get relative file paths recursively
void getFilesRecursively(const std::string &folderPath, const std::string &baseFolder, std::vector<std::string> &files) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((folderPath + "\\*").c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return;
        }

    do {
        const std::string fileOrDir = findFileData.cFileName;
        if (fileOrDir != "." && fileOrDir != "..") {
            const std::string fullPath = folderPath + "\\" + fileOrDir;
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                getFilesRecursively(fullPath, baseFolder, files);
                }
            else {
                files.push_back(fullPath.substr(baseFolder.length() + 1));
                }
            }
        } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
    }



void replaceBigAndBix(const std::string &bixPath, const std::string &folderPath) {
    std::string bigPath = bixPath.substr(0, bixPath.find_last_of('.')) + ".big";

    // Backup original files
    CopyFile(bixPath.c_str(), (bixPath + ".bak").c_str(), FALSE);
    CopyFile(bigPath.c_str(), (bigPath + ".bak").c_str(), FALSE);

    // Open original bix file
    bytestream bixStream;
    if (!bixStream.openFile(bixPath)) {
        std::cerr << "Could not open original bix file!" << std::endl;
        return;
        }

    // Read original bix data
    BigInventory_t bigInventory;
    if (!bigInventory.read(bixStream)) {
        std::cerr << "Failed to read bix file!" << std::endl;
        return;
        }

    // Open original big file
    HANDLE hBigFile = CreateFile(bigPath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hBigFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Could not open original big file!" << std::endl;
        return;
        }

    // Create new big file
    HANDLE hNewBigFile = CreateFile((bigPath + ".new").c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hNewBigFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Could not create new big file!" << std::endl;
        CloseHandle(hBigFile);
        return;
        }

    // Get list of files to import
    std::vector<std::string> files;
    getFilesRecursively(folderPath, folderPath, files);

    // Track current offset in the new .big file
    uint32_t currentOffset = 0;

    // Buffer size and alignment
    const size_t BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];

    // Process each entry in the .bix file
    for (size_t i = 0; i < bigInventory.entry.size(); ++i) {
        auto &entry = bigInventory.entry[i];
        bool replaced = false;

        // Check if the current entry matches any file in the import folder
        for (const auto &file : files) {
            //uint32_t fileHash = StringHashUpper32(file.c_str());

            // if the filename itself is a hash then process it as such
            uint32_t fileHash = getHashFromFilename(file);

            if (fileHash == entry.m_NameUID) {
                std::string filePath = folderPath + "\\" + file;
                HANDLE hNewFile = CreateFile(filePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                if (hNewFile == INVALID_HANDLE_VALUE) {
                    std::cerr << "Could not open file: " << filePath << std::endl;
                    continue;
                    }

                LARGE_INTEGER newSize;
                GetFileSizeEx(hNewFile, &newSize);
                entry.m_CompressedSize = 0;
                entry.m_UncompressedSize = static_cast<uint32_t>(newSize.QuadPart);
                entry.m_LoadOffset = 0;
                entry.m_CompressedExtra = 0;

                // Update entry offset
                entry.m_Offset = currentOffset;

                LARGE_INTEGER offset;
                offset.QuadPart = currentOffset * 4;  // Convert to byte offset
                SetFilePointerEx(hNewBigFile, offset, NULL, FILE_BEGIN);

                // Read the new file in chunks and write it to the new .big file
                DWORD bytesRead, bytesWritten;
                while (ReadFile(hNewFile, buffer, BUFFER_SIZE, &bytesRead, NULL) && bytesRead > 0) {
                    WriteFile(hNewBigFile, buffer, bytesRead, &bytesWritten, NULL);
                    }

                // Pad the file to a 4096-byte boundary
                DWORD paddingSize = (4096 - (newSize.QuadPart % 4096)) % 4096;
                if (paddingSize > 0) {
                    memset(buffer, 0xCD, paddingSize);
                    WriteFile(hNewBigFile, buffer, paddingSize, &bytesWritten, NULL);
                    }

                CloseHandle(hNewFile);

                // Update current offset for the next file
                currentOffset += (newSize.QuadPart + paddingSize) / 4;
                replaced = true;
                break;
                }
            }

        if (!replaced) {
            // If the file does not exist in the folder, copy the original data from the old .big file
            LARGE_INTEGER offset;
            offset.QuadPart = entry.m_Offset * 4;  // Convert to byte offset
            SetFilePointerEx(hBigFile, offset, NULL, FILE_BEGIN);

            DWORD actualSize = 0;
            DWORD beginPadding = 0;
            DWORD endPadding = 0;
            if (entry.m_CompressedSize > 0) {
                // File is compressed
                beginPadding = (entry.m_LoadOffset % 4096);
                endPadding = ((4096 - ((entry.m_CompressedSize + entry.m_LoadOffset) % 4096)) % 4096);
                actualSize = entry.m_CompressedSize + beginPadding + endPadding;
                }
            else {
                // File is uncompressed
                actualSize = entry.m_UncompressedSize;
                }

            // Move the file pointer to the correct position in the new big file
            LARGE_INTEGER newOffset;
            newOffset.QuadPart = currentOffset * 4;  // Convert to byte offset
            SetFilePointerEx(hNewBigFile, newOffset, NULL, FILE_BEGIN);

            // Read the file from the old .big file in chunks and write it to the new .big file
            DWORD bytesRead, bytesWritten;
            DWORD remaining = actualSize;
            while (remaining > 0) {
                DWORD toRead = (remaining > BUFFER_SIZE) ? BUFFER_SIZE : remaining;
                if (!ReadFile(hBigFile, buffer, toRead, &bytesRead, NULL) || bytesRead == 0) {
                    std::cerr << "Failed to read from original big file." << std::endl;
                    break;
                    }
                if (!WriteFile(hNewBigFile, buffer, bytesRead, &bytesWritten, NULL) || bytesWritten == 0) {
                    std::cerr << "Failed to write to new big file." << std::endl;
                    break;
                    }
                remaining -= bytesRead;
                }

            // Update entry offset
            entry.m_Offset = currentOffset;

            // Pad the file to a 4096-byte boundary
            DWORD paddingSize = (4096 - (actualSize % 4096)) % 4096;
            if (paddingSize > 0) {
                memset(buffer, 0, paddingSize);
                WriteFile(hNewBigFile, buffer, paddingSize, &bytesWritten, NULL);
                }

            // Update current offset for the next file
            currentOffset += (actualSize + paddingSize) / 4;
            }
        }

    // Close big files
    CloseHandle(hBigFile);
    CloseHandle(hNewBigFile);

    // Rename new big file to replace original
    MoveFileEx((bigPath + ".new").c_str(), bigPath.c_str(), MOVEFILE_REPLACE_EXISTING);

    // Save new .bix file using Windows API
    bytestream newBixStream;
    newBixStream.createFile(bixStream.size);
    bigInventory.write(newBixStream);

    HANDLE hNewBixFile = CreateFile(bixPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hNewBixFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Could not create new bix file!" << std::endl;
        return;
        }

    DWORD bytesWritten;
    WriteFile(hNewBixFile, newBixStream.stream, newBixStream.size, &bytesWritten, NULL);
    CloseHandle(hNewBixFile);

    bixStream.close();
}

// Function to open file dialog and get file path
std::string openFileDialog() {
    OPENFILENAME ofn;
    char fileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Bix Files\0*.bix\0All Files\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "bix";

    if (GetOpenFileName(&ofn)) {
        return std::string(fileName);
        }

    return "";
}

// Function to open folder dialog and get folder path
std::string openFolderDialog() {
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = "Browse for Folder";
    bi.ulFlags = BIF_EDITBOX | BIF_NEWDIALOGSTYLE;  // Allow pasting paths and show the new style dialog
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    char path[MAX_PATH];

    if (pidl != 0) {
        SHGetPathFromIDList(pidl, path);
        IMalloc* imalloc = 0;
        if (SUCCEEDED(SHGetMalloc(&imalloc))) {
            imalloc->Free(pidl);
            imalloc->Release();
            }
        return std::string(path);
        }

    return "";
}

// Callback for bix file selection button
void bixFileButtonCallback(Fl_Widget* widget, void* input) {
    std::string filePath = openFileDialog();
    if (!filePath.empty()) {
        ((Fl_Input*)input)->value(filePath.c_str());
        }
}

// Callback for import folder selection button
void importFolderButtonCallback(Fl_Widget* widget, void* input) {
    std::string folderPath = openFolderDialog();
    if (!folderPath.empty()) {
        ((Fl_Input*)input)->value(folderPath.c_str());
        }
}

// Callback for unpack button
void unpackButtonCallback(Fl_Widget* widget, void* data) {
    Fl_Input* bixInput = ((Fl_Input**)data)[0];
    Fl_Input* folderInput = ((Fl_Input**)data)[1];

    std::string bixPath = bixInput->value();
    std::string folderPath = folderInput->value();

    if (!bixPath.empty() && !folderPath.empty()) {
        replaceBigAndBix(bixPath, folderPath);
        fl_alert("Unpacking completed successfully!");
        } else {
        fl_alert("Please select both the .bix file and the import folder.");
        }
}





void theme1() {
    std::vector<unsigned char> r, g, b;
    int N = 4 + FL_NUM_GRAY;
    r = std::vector<unsigned char>(N, 0);
    g = std::vector<unsigned char>(N, 0);
    b = std::vector<unsigned char>(N, 0);

    // store default (OS-dependent) interface colors:
    Fl::get_system_colors();
    Fl::get_color(FL_BACKGROUND_COLOR, r[0], g[0], b[0]);
    Fl::get_color(FL_BACKGROUND2_COLOR, r[1], g[1], b[1]);
    Fl::get_color(FL_FOREGROUND_COLOR, r[2], g[2], b[2]);
    Fl::get_color(FL_SELECTION_COLOR, r[3], g[3], b[3]);

    for (int i = 0; i < FL_NUM_GRAY; i++) {
        Fl::get_color(fl_gray_ramp(i), r[4 + i], g[4 + i], b[4 + i]);
    }

    // Light color theme
    Fl::set_color(FL_BACKGROUND_COLOR, 240, 240, 240);   // Light gray background
    Fl::set_color(FL_BACKGROUND2_COLOR, 255, 255, 255);  // White for secondary background
    Fl::set_color(FL_FOREGROUND_COLOR, 0, 0, 0);         // Black text
    for (int i = 0; i < FL_NUM_GRAY; i++) {
        double min = 200.0, max = 255.0;
        int d = (int)(min + i * (max - min) / (FL_NUM_GRAY - 1));
        Fl::set_color(fl_gray_ramp(i), d, d, d);
    }
    Fl::reload_scheme();
    Fl::set_color(FL_SELECTION_COLOR, 173, 216, 230);    // Light blue for selection
}

void toggle_theme() {

 std::vector<unsigned char> r, g, b;
    int N = 4 + FL_NUM_GRAY;
    r.resize(N, 0);
    g.resize(N, 0);
    b.resize(N, 0);

    // Store default (OS-dependent) interface colors:
    Fl::get_system_colors();
    Fl::get_color(FL_BACKGROUND_COLOR, r[0], g[0], b[0]);
    Fl::get_color(FL_BACKGROUND2_COLOR, r[1], g[1], b[1]);
    Fl::get_color(FL_FOREGROUND_COLOR, r[2], g[2], b[2]);
    Fl::get_color(FL_SELECTION_COLOR, r[3], g[3], b[3]);

    for (int i = 0; i < FL_NUM_GRAY; i++) {
        Fl::get_color(fl_gray_ramp(i), r[4 + i], g[4 + i], b[4 + i]);
    }
  if (USE_DARK_MODE) { // dark mode
      Fl::set_color(FL_BACKGROUND_COLOR, 50, 50, 50);
      Fl::set_color(FL_BACKGROUND2_COLOR, 120, 120, 120);
      Fl::set_color(FL_FOREGROUND_COLOR, 240, 240, 240);
      for (int i = 0; i < FL_NUM_GRAY; i++) {
          double min = 0., max = 135.;
          int d = (int)(min + i * (max - min) / (FL_NUM_GRAY - 1.));
          Fl::set_color(fl_gray_ramp(i), d, d, d);
          }
      Fl::reload_scheme();
      Fl::set_color(FL_SELECTION_COLOR, 200, 200, 200);
      }
  else {
      // restore default colors
      Fl::set_color(FL_BACKGROUND_COLOR, r[0], g[0], b[0]);
      Fl::set_color(FL_BACKGROUND2_COLOR, r[1], g[1], b[1]);
      Fl::set_color(FL_FOREGROUND_COLOR, r[2], g[2], b[2]);
      for (int i = 0; i < FL_NUM_GRAY; i++) {
          Fl::set_color(fl_gray_ramp(i), r[4 + i], g[4 + i], b[4 + i]);
          }
      Fl::reload_scheme();
      Fl::set_color(FL_SELECTION_COLOR, r[3], g[3], b[3]);
      theme1();
      }


}



bool getWindowTheme(HWND &hwnd) {
  // based on https://stackoverflow.com/questions/51334674/how-to-detect-windows-10-light-dark-mode-in-win32-application

  // The value is expected to be a REG_DWORD, which is a signed 32-bit little-endian
  auto buffer = std::vector<char>(4);
  auto cbData = static_cast<DWORD>(buffer.size() * sizeof(char));
  auto res = RegGetValueW(
      HKEY_CURRENT_USER,
      L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
      L"AppsUseLightTheme",
      RRF_RT_REG_DWORD, // expected value type
      nullptr,
      buffer.data(),
      &cbData
      );

  if (res != ERROR_SUCCESS) {
      throw std::runtime_error("Error: error_code=" + std::to_string(res));
      }

  // convert bytes written to our buffer to an int, assuming little-endian
  auto i = int(buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0]);

USE_DARK_MODE = i == 0;

    if (hwnd != nullptr) {
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &USE_DARK_MODE, 4);
        ::UpdateWindow(hwnd);
    }
toggle_theme();

  return i == 1;




}

//int main() {
//    try {
//        QuickCompression qc;
//        std::string inputFilePath = "G:\\tmp\\SDDE_RREPACK\\compressed.qcmp";
//        std::string outputFilePath = "G:\\tmp\\SDDE_RREPACK\\decompressed_output.bin";
//
//        // Assuming you know the buffer offset
//        size_t bufferOffset = 0; // Set the correct offset
//
//        qc.DecompressFromBuffer(inputFilePath, bufferOffset, outputFilePath);
//
//        std::cout << "Decompression completed successfully." << std::endl;
//    } catch (const std::exception& e) {
//        std::cerr << "Decompression error: " << e.what() << std::endl;
//        return 1;
//    }
//
//    return 0;
//}


//int main() {
//    try {
//        QuickCompression qc;
//        std::string inputFilePath = "G:\\tmp\\SDDE_RREPACK\\Back\\GameSD10\\Data\\World\\Game\\Zone\\SD\\NP_TG\\NP_TG.perm.bin";
//        std::string outputFilePath = "G:\\tmp\\SDDE_RREPACK\\compressed.qcmp";
//
//        qc.CompressToFile(inputFilePath, outputFilePath);
//
//        std::cout << "Compression completed successfully." << std::endl;
//    } catch (const std::exception& e) {
//        std::cerr << "Compression error: " << e.what() << std::endl;
//        return 1;
//    }
//
//    return 0;
//}


int main (int argc, char** argv) {

    // Include Build Date in the Application Title
    SetConsoleTitleA(appver);

    // Get Commands in Wide Characters
    int argcW; LPWSTR *argvW = CommandLineToArgvW(GetCommandLineW(), &argcW); if (argcW == 0) {return 0;}



//    if (argc < 1) {return 0;}
//
//    bytestream f;
//    if (!f.openFile(argv[1])) {
//        std::cerr << "Could not open file!" << std::endl;
//        return 1;
//            }
//
//    BigInventory_t bigInventory;
//    if (bigInventory.read(f)) {
//        std::cout << "Read successful!" << std::endl;
//        printBigInventory(bigInventory);
//            }
//    else {
//        std::cout << "Read failed!" << std::endl;
//            }
//
//    f.close();


    if (argc == 3) {
        // CLI mode
        std::string bixPath = argv[1];
        std::string folderPath = argv[2];

        if (!bixPath.empty() && !folderPath.empty()) {
            replaceBigAndBix(bixPath, folderPath);
            std::cout << "Unpacking completed successfully!" << std::endl;
            } else {
            std::cerr << "Please provide both the .bix file and the import folder." << std::endl;
            }
        }
        else {



        bool useBasicForm = false;
        if (useBasicForm) {

            // FLTK UI mode
            Fl_Window* window = new Fl_Window(400, 200, "Replace Big and Bix");

            Fl_Input* bixInput = new Fl_Input(100, 20, 220, 30, "Bix File:");
            Fl_Button* bixButton = new Fl_Button(330, 20, 50, 30, "...");
            bixButton->callback(bixFileButtonCallback, bixInput);

            Fl_Input* folderInput = new Fl_Input(100, 60, 220, 30, "Import Folder:");
            Fl_Button* folderButton = new Fl_Button(330, 60, 50, 30, "...");
            folderButton->callback(importFolderButtonCallback, folderInput);

            Fl_Input* inputs[2] = { bixInput, folderInput };
            Fl_Button* unpackButton = new Fl_Button(150, 120, 100, 30, "Unpack");
            unpackButton->callback(unpackButtonCallback, inputs);

            window->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(101)));
            Fl::scheme("gtk+");


            window->end();
            window->show(argc, argv);

            return Fl::run();
            }
        else {
            // Initialize the file list from filelist.txt
            std::string filelist_status = "!No Filelist.txt";
            initializeFileList();
            if (getFileHashMapCount() > 0) {
                filelist_status = "Filelist.txt [" + to_string(getFileHashMapCount()) + "]";

                }


            Fl_Double_Window* window = dogpack();
            //myTable = new MyTable(10, 40, 780, 500);
            HWND hwnd = fl_xid(window);
            getWindowTheme(hwnd);

            window->end();
            window->show();
            app_filename = "";
            //stb_llabel->label(app_filename.c_str());
            //stb_rlabel->label(app_version.c_str());
            updateLabels();
            stb_llabel->copy_label("");
            stb_llabel->copy_label(filelist_status.c_str());
            // Register the global event handler
            //Fl::add_handler(handle_exit_event);

            return Fl::run();
            }
        }



    LocalFree(argvW); // Free memory allocated for CommandLineToArgvW arguments.

    return 0;
        }

