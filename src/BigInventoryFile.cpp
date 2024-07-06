#include "BigInventoryFile.h"
#include "Filenames.h"
#include "QuickCompression.h"
#include <algorithm>
#include <shlobj.h>


bool ResourceEntry_t::read(bytestream &f) {
    reserved.clear();
    m_TypeUID = f.readUlong();

    static const uint32_t t_types[] = {0x2C5C40A8L}; // BIGFile
    bool result = std::find(std::begin(t_types), std::end(t_types), m_TypeUID) != std::end(t_types);
    if (result) {
        m_EntrySize[0] = f.readUlong();
        m_EntrySize[1] = f.readUlong();
        m_Offset = f.readUlong();

        if (m_Offset > 0) {
            reserved.resize(m_Offset);
            for (uint32_t i = 0; i < m_Offset; ++i) {
                reserved[i] = f.readUbyte();
            }
        }
    }
    return result;
}

bool ResourceEntry_t::write(bytestream &f) {
    f.writeUlong(m_TypeUID);
    f.writeUlong(m_EntrySize[0]);
    f.writeUlong(m_EntrySize[1]);
    f.writeUlong(m_Offset);
    for (auto &byte : reserved) {
        f.writeUbyte(byte);
    }
    return true;
}

bool ResourceData_t::read(bytestream &f) {
    bool result = base.read(f);
    if (result) {
        for (int i = 0; i < 24; ++i) {
            m_Pad1[i] = f.readUbyte();
        }
        m_NameUID = f.readUlong();
        for (int i = 0; i < 20; ++i) {
            pad2[i] = f.readUbyte();
        }
        m_ChunkUID = f.readUlong();

        for (int i = 0; i < 36; ++i) {
            char b = f.readbyte();
            if (b == 0) {
                f.seek(36 - i - 1, seek::cur);
                break;
            } else {
                m_DebugName[i] = b;
            }
        }
    }
    return result;
}

bool ResourceData_t::write(bytestream &f) {
    if (base.write(f)) {
        for (int i = 0; i < 24; ++i) {
            f.writeUbyte(m_Pad1[i]);
        }
        f.writeUlong(m_NameUID);
        for (int i = 0; i < 20; ++i) {
            f.writeUbyte(pad2[i]);
        }
        f.writeUlong(m_ChunkUID);
        for (int i = 0; i < 36; ++i) {
            f.writebyte(m_DebugName[i]);
        }
        return true;
    }
    return false;
}

void BigInventoryEntry_t::read(bytestream &f) {
    m_NameUID = f.readUlong();
    m_Offset = f.readUlong();
    m_LoadOffset = f.readUlong();
    m_CompressedSize = f.readUlong();
    m_CompressedExtra = f.readUlong();
    m_UncompressedSize = f.readUlong();
}

void BigInventoryEntry_t::write(bytestream &f) {
    f.writeUlong(m_NameUID);
    f.writeUlong(m_Offset);
    f.writeUlong(m_LoadOffset);
    f.writeUlong(m_CompressedSize);
    f.writeUlong(m_CompressedExtra);
    f.writeUlong(m_UncompressedSize);
}

bool BigInventory_t::read(bytestream &f) {
    if (m_Resource.read(f)) {
        m_SortKey = f.readlonglong();
        m_EntryCount = f.readlonglong();
        m_EntryOffset = f.tell() + f.readlonglong();
        m_FilePointer = f.readlonglong();
        m_State = f.readshort();
        m_MountIndex = f.readshort();
        for (int i = 0; i < 8; ++i) {
            m_Pad1[i] = f.readUbyte();
        }
        for (int i = 0; i < 32; ++i) {
            char b = f.readbyte();
            if (b != 0) {
                m_ArchiveName[i] = b;
            }
        }
        for (int i = 0; i < 12; ++i) {
            m_Pad2[i] = f.readUbyte();
        }

        if (m_EntryCount > 0) {
            entry.resize(m_EntryCount);
            f.seek(m_EntryOffset, seek::set);
            for (auto &e : entry) {
                e.read(f);
            }
        }
        return true;
    }
    return false;
}

bool BigInventory_t::write(bytestream &f) {

    f.resize(192 + m_Resource.base.reserved.size() + (entry.size() * 24));
    f.seek(0);


    if (m_Resource.write(f)) {
        f.writelonglong(m_SortKey);
        f.writelonglong(m_EntryCount);
        f.writelonglong(m_EntryOffset=72);
        f.writelonglong(m_FilePointer);
        f.writeshort(m_State);
        f.writeshort(m_MountIndex);
        for (int i = 0; i < 8; ++i) {
            f.writeUbyte(m_Pad1[i]);
        }
        for (int i = 0; i < 32; ++i) {
            f.writebyte(m_ArchiveName[i]);
        }
        for (int i = 0; i < 12; ++i) {
            f.writeUbyte(m_Pad2[i]);
        }

        if (!entry.empty()) {
            f.seek(192, seek::set);
            for (auto &e : entry) {
                e.write(f);
            }
        }
        return true;
    }
    return false;
}

void exportFile(HANDLE hBigFile, const std::string& exportDir, const BigInventoryEntry_t& entry, bool isBulkExport) {
    // Helper function to convert uint32_t to an 8-character uppercase hexadecimal string
    auto uint32ToHexStr = [](uint32_t value) {
        std::stringstream ss;
        ss << std::uppercase << std::hex << std::setw(8) << std::setfill('0') << value;
        return ss.str();
    };

    std::string fileName = getFilePathFromHash(entry.m_NameUID);
    if (fileName.empty()) {
        fileName = uint32ToHexStr(entry.m_NameUID) + ".bin";
    }

    // Calculate the file offset
    LARGE_INTEGER fileOffset;
    if (entry.m_CompressedSize > 0) {
        // Calculate new address for compressed files
        fileOffset.QuadPart = (entry.m_Offset * 4) + (entry.m_LoadOffset % 4096);
    } else {
        fileOffset.QuadPart = entry.m_Offset * 4; // Convert to byte offset
    }

    // Set the file pointer to the offset of the file to be exported
    if (!SetFilePointerEx(hBigFile, fileOffset, NULL, FILE_BEGIN)) {
        std::cerr << "Failed to set file pointer in original big file!" << std::endl;
        return;
    }

    // Read the file content
    DWORD fileSize = entry.m_CompressedSize > 0 ? entry.m_CompressedSize : entry.m_UncompressedSize;
    std::vector<char> buffer(fileSize);
    DWORD bytesRead;
    if (!ReadFile(hBigFile, buffer.data(), fileSize, &bytesRead, NULL) || bytesRead != fileSize) {
        std::cerr << "Failed to read file from original big file!" << std::endl;
        return;
    }

    // Determine the export path
    std::string fullExportPath;
    if (isBulkExport) {
        fullExportPath = exportDir + "\\" + fileName;
    } else {
        fullExportPath = exportDir;
    }

    // Create directories if necessary
    size_t pos = fullExportPath.find_last_of("\\/");
    if (pos != std::string::npos) {
        std::string directory = fullExportPath.substr(0, pos);
        SHCreateDirectoryExA(NULL, directory.c_str(), NULL);
    }

    if (entry.m_CompressedSize > 0) {
        // Decompress the file directly from the buffer
        QuickCompression decompressor; // Create an instance of QuickCompression
        std::vector<uint8_t> decompressedData(entry.m_UncompressedSize);
        try {
            decompressor.DecompressData(reinterpret_cast<const uint8_t*>(buffer.data()), fileSize, decompressedData);
        } catch (const std::exception& e) {
            std::cerr << "Failed to decompress file: " << e.what() << std::endl;
            return;
        }

        // Write the decompressed data to the export file
        HANDLE hExportFile = CreateFile(fullExportPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hExportFile == INVALID_HANDLE_VALUE) {
            std::cerr << "Could not create export file: " << fullExportPath << std::endl;
            return;
        }

        DWORD bytesWritten;
        if (!WriteFile(hExportFile, decompressedData.data(), entry.m_UncompressedSize, &bytesWritten, NULL) || bytesWritten != entry.m_UncompressedSize) {
            std::cerr << "Failed to write to export file!" << std::endl;
            CloseHandle(hExportFile);
            return;
        }
        CloseHandle(hExportFile);
    } else {
        // Save the uncompressed file directly
        HANDLE hExportFile = CreateFile(fullExportPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hExportFile == INVALID_HANDLE_VALUE) {
            std::cerr << "Could not create export file: " << fullExportPath << std::endl;
            return;
        }

        DWORD bytesWritten;
        if (!WriteFile(hExportFile, buffer.data(), fileSize, &bytesWritten, NULL) || bytesWritten != fileSize) {
            std::cerr << "Failed to write to export file!" << std::endl;
            CloseHandle(hExportFile);
            return;
        }
        CloseHandle(hExportFile);
    }

    std::cout << "Exported: " << fileName << " to " << fullExportPath << std::endl;
}

bool loadBixFile(const std::string &bixPath) {
    globalBixPath = bixPath;
    globalBigPath = bixPath.substr(0, bixPath.find_last_of('.')) + ".big";

    bytestream bixStream;
    if (!bixStream.openFile(bixPath)) {
        std::cerr << "Could not open original bix file!" << std::endl;
        return false;
    }

    if (!globalBigInventory.read(bixStream)) {
        std::cerr << "Failed to read bix file!" << std::endl;
        return false;
    }

    bixStream.close();
    return true;
}

bool importFile(HANDLE hBigFile, HANDLE hNewBigFile, const std::string &filePath, BigInventoryEntry_t &entry, uint32_t &currentOffset) {
    HANDLE hNewFile = CreateFile(filePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hNewFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Could not open file: " << filePath << std::endl;
        return false;
    }

    LARGE_INTEGER newSize;
    GetFileSizeEx(hNewFile, &newSize);
    entry.m_CompressedSize = 0;
    entry.m_UncompressedSize = static_cast<uint32_t>(newSize.QuadPart);
    entry.m_LoadOffset = 0;
    entry.m_CompressedExtra = 0;

    entry.m_Offset = currentOffset;

    LARGE_INTEGER offset;
    offset.QuadPart = currentOffset * 4;  // Convert to byte offset
    SetFilePointerEx(hNewBigFile, offset, NULL, FILE_BEGIN);

    const size_t BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    DWORD bytesRead, bytesWritten;
    while (ReadFile(hNewFile, buffer, BUFFER_SIZE, &bytesRead, NULL) && bytesRead > 0) {
        WriteFile(hNewBigFile, buffer, bytesRead, &bytesWritten, NULL);
    }

    DWORD paddingSize = (4096 - (newSize.QuadPart % 4096)) % 4096;
    if (paddingSize > 0) {
        memset(buffer, 0xCD, paddingSize);
        WriteFile(hNewBigFile, buffer, paddingSize, &bytesWritten, NULL);
    }

    CloseHandle(hNewFile);

    currentOffset += (newSize.QuadPart + paddingSize) / 4;
    return true;
}

bool copyOriginalData(HANDLE hBigFile, HANDLE hNewBigFile, BigInventoryEntry_t &entry, uint32_t &currentOffset) {
    LARGE_INTEGER offset;
    offset.QuadPart = entry.m_Offset * 4;  // Convert to byte offset
    SetFilePointerEx(hBigFile, offset, NULL, FILE_BEGIN);

    DWORD actualSize = 0;
    if (entry.m_CompressedSize > 0) {
        DWORD beginPadding = (entry.m_LoadOffset % 4096);
        DWORD endPadding = ((4096 - ((entry.m_CompressedSize + entry.m_LoadOffset) % 4096)) % 4096);
        actualSize = entry.m_CompressedSize + beginPadding + endPadding;
    } else {
        actualSize = entry.m_UncompressedSize;
    }

    LARGE_INTEGER newOffset;
    newOffset.QuadPart = currentOffset * 4;  // Convert to byte offset
    SetFilePointerEx(hNewBigFile, newOffset, NULL, FILE_BEGIN);

    const size_t BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    DWORD bytesRead, bytesWritten;
    DWORD remaining = actualSize;
    while (remaining > 0) {
        DWORD toRead = (remaining > BUFFER_SIZE) ? BUFFER_SIZE : remaining;
        if (!ReadFile(hBigFile, buffer, toRead, &bytesRead, NULL) || bytesRead == 0) {
            std::cerr << "Failed to read from original big file." << std::endl;
            return false;
        }
        if (!WriteFile(hNewBigFile, buffer, bytesRead, &bytesWritten, NULL) || bytesWritten == 0) {
            std::cerr << "Failed to write to new big file." << std::endl;
            return false;
        }
        remaining -= bytesRead;
    }

    entry.m_Offset = currentOffset;

    DWORD paddingSize = (4096 - (actualSize % 4096)) % 4096;
    if (paddingSize > 0) {
        memset(buffer, 0, paddingSize);
        WriteFile(hNewBigFile, buffer, paddingSize, &bytesWritten, NULL);
    }

    currentOffset += (actualSize + paddingSize) / 4;
    return true;
}

BigInventory_t globalBigInventory;
std::string globalBixPath;
std::string globalBigPath;
std::unordered_map<int, std::string> importFilesMap;
