#ifndef BIGINVENTORYFILE_H
#define BIGINVENTORYFILE_H

#include <windows.h>
#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include "bytestream.h"

struct ResourceEntry_t {
    uint32_t                m_TypeUID           = 0;
    uint32_t                m_EntrySize[2]      = {0, 0};
    uint32_t                m_Offset            = 0;
    std::vector<uint8_t>    reserved;

    bool read(bytestream &f);
    bool write(bytestream &f);
    };

struct ResourceData_t {
    ResourceEntry_t         base;
    uint8_t                 m_Pad1[24]          = {0};
    uint32_t                m_NameUID           = 0;
    uint8_t                 pad2[20]            = {0};
    uint32_t                m_ChunkUID          = 0;
    char                    m_DebugName[36]     = "";

    bool read(bytestream &f);
    bool write(bytestream &f);
    };

struct BigInventoryEntry_t {
    uint32_t                m_NameUID           = 0;
    uint32_t                m_Offset            = 0;
    uint32_t                m_LoadOffset        = 0;
    uint32_t                m_CompressedSize    = 0;
    uint32_t                m_CompressedExtra   = 0;
    uint32_t                m_UncompressedSize  = 0;

    void read(bytestream &f);
    void write(bytestream &f);
    };

struct BigInventory_t {
    ResourceData_t          m_Resource;
    int64_t                 m_SortKey           = -1;
    int64_t                 m_EntryCount        = 0;
    int64_t                 m_EntryOffset       = 0;
    int64_t                 m_FilePointer       = 0;
    int16_t                 m_State             = 0;
    int16_t                 m_MountIndex        = -1;
    uint8_t                 m_Pad1[8]           = {0};
    char                    m_ArchiveName[32]   = "";
    uint8_t                 m_Pad2[12]          = {0};
    std::vector<BigInventoryEntry_t> entry;

    bool read(bytestream &f);
    bool write(bytestream &f);
    };

extern BigInventory_t globalBigInventory;
extern std::string globalBixPath;
extern std::string globalBigPath;
extern std::unordered_map<int, std::string> importFilesMap; // key: row index, value: file path

void exportFile(HANDLE hBigFile, const std::string& exportDir, const BigInventoryEntry_t& entry, bool isBulkExport = false);
bool loadBixFile(const std::string &bixPath);
bool importFile(HANDLE hBigFile, HANDLE hNewBigFile, const std::string &filePath, BigInventoryEntry_t &entry, uint32_t &currentOffset);
bool copyOriginalData(HANDLE hBigFile, HANDLE hNewBigFile, BigInventoryEntry_t &entry, uint32_t &currentOffset);


#endif // BIGINVENTORYFILE_H
