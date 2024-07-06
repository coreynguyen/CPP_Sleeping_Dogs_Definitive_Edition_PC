#ifndef FILENAMES_H
#define FILENAMES_H

#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>

std::vector<uint32_t> generate_crc32_table();
extern const uint32_t g_CRC32Table[256];
uint32_t StringHashUpper32(const char* p_Str, uint32_t p_PrevHash = 0xFFFFFFFF);

// Function to load file paths and their hashes from filelist.txt
extern std::unordered_map<uint32_t, std::string> fileHashMap;
void initializeFileList();
void loadFileList(const std::string& filePath);
size_t getFileHashMapCount();
// Function to get the file path corresponding to a hash

bool isHexChar(char c);
std::string getFilePathFromHash(uint32_t hash);
uint32_t getHashFromFilename(const std::string &filename);

#endif // FILENAMES_H
