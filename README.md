# Sleeping Dogs: Definitive Edition Archive Manager

## Overview

The Sleeping Dogs: Definitive Edition Archive Manager is a specialized tool designed to manage the `.big` and `.bix` files used in the Steam version of Sleeping Dogs: Definitive Edition. The `.big` files are data buffers, while the `.bix` files serve as indices for these buffers. This tool enables users to efficiently export and import files from these archives.

## Features

- **Export Files**: Export individual files or all files from the `.big` archives.
- **Import Files**: Import files into the `.big` archives.
- **Hash Management**: Utilizes CRC32 hashes for accurate file identification and management.
- **File Handling**: Supports reading and saving of uncompressed files, with the ability to read compressed data from the `.big` archives and save them as decompressed files.

## Current Limitations

- **Compression Support**: The tool does not support saving files in a compressed format back into the `.big` archives. It can only import uncompressed files.
- **File Hashes**: The archive stores file hashes instead of names. A `filelist.txt` is included to provide some names for known hashes, but it is incomplete.

## Usage

### Opening an Archive

1. Launch the application.
2. Use the `File` menu to select `Open Archive`.
3. Navigate to and select the desired `.bix` file.

### Exporting Files

- **Export Selected Files**:
  1. Select the files you want to export from the list.
  2. Use the `Tools` menu to select `Export Selection`.
  3. Choose the destination directory.

- **Export All Files**:
  1. Use the `Tools` menu to select `Export All to Folder`.
  2. Choose the destination directory.

### Importing Files

- **Import File to Selected Row**:
  1. Select the row in the table where you want to import the file.
  2. Use the `Tools` menu to select `Replace Selection`.
  3. Choose the file to import.

- **Import All Files from Folder**:
  1. Use the `Tools` menu to select `Replace From Folder`.
  2. Choose the folder containing the files to import.

### Hash Management

The tool uses CRC32 hashes to manage and identify files within the archives. It calculates hashes for filenames and manages them in a hash map to ensure accurate file operations.

## Building

### Prerequisites

- MinGW32 for compilation
- FLTK (Fast Light Toolkit) for the graphical user interface
  
### Build Instructions

1. **Install MinGW**:
   - Ensure MinGW is installed and added to your system PATH.

2. **Install FLTK**:
   - Download and install FLTK from the official website.

3. **Clone the Repository**:
    ```bash
    git clone https://github.com/yourusername/sleeping-dogs-archive-manager.git
    cd sleeping-dogs-archive-manager
    ```

4. **Build the Project Using MinGW**:
    ```bash
    g++ -Wall -fexceptions -DMING32 -DWIN32 -Iinclude -Iinclude/FLTK -c src/*.cpp
    g++ -o bin/Release/dogpack obj/Release/*.o -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic -lfltk_gl -lfltk_forms -lfltk_images -lfltk_jpeg -lfltk_png -lfltk_z -lfltk -ldwmapi -lshlwapi -lcomdlg32 -lole32 -luuid -lcomctl32 -lgdi32 -lws2_32
    ```

## False Positive Virus Report

It has been reported that Windows might identify this application as a Trojan horse virus. This is a false positive likely resulting from the toolchain used for development, which includes MinGW32 for compilation and FLTK for the graphical user interface. The application also accesses the Windows registry to determine if Windows dark mode is activated to sync the UI theme accordingly. Rest assured, this application is safe to use.

## Acknowledgements

Special thanks to the **[SDmodding](https://github.com/SDmodding)** community for their invaluable resources and code structures that significantly aided in developing this tool.

## Disclaimer

This tool is provided as-is, without any warranty or guarantee of its functionality or safety. The developer is not responsible for any damage or data loss that may occur from using this tool. Given that this software involves file reading and writing operations, there is an inherent risk of file overwrites and data corruption. Users are strongly advised to back up their files before using this tool to avoid any unintended data loss.


