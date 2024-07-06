#include "QuickCompression.h"
#include <unordered_map>
#include <string>
#include <limits>
#include <cstring> // Include this header for memcpy
#include <iostream> // For debugging output
#include <algorithm> // For std::find

#include <deque>
#include <thread>
#include "windows.h"
#include <mutex>
#include <condition_variable>
#include <sstream>


const uint32_t Signature = 0x51434D50; // 'QCMP'


namespace ReadValue {
    template <typename T>
    T Read(const uint8_t* buffer, Endian endian, size_t& offset, size_t bufferSize) {
        if (offset + sizeof(T) > bufferSize) {
            throw std::runtime_error("ReadValue::Read: Attempt to read beyond buffer size");
        }

        T value = *reinterpret_cast<const T*>(buffer + offset);
        if (endian == Endian::Big) {
            if constexpr (sizeof(T) == 2) {
                value = _byteswap_ushort(value);
            } else if constexpr (sizeof(T) == 4) {
                value = _byteswap_ulong(value);
            } else if constexpr (sizeof(T) == 8) {
                value = _byteswap_uint64(value);
            }
        }
        offset += sizeof(T);
        return value;
    }

    template <typename T>
    void Write(std::vector<uint8_t>& buffer, T value, Endian endian) {
        if (endian == Endian::Big) {
            if constexpr (sizeof(T) == 2) {
                value = _byteswap_ushort(value);
            } else if constexpr (sizeof(T) == 4) {
                value = _byteswap_ulong(value);
            } else if constexpr (sizeof(T) == 8) {
                value = _byteswap_uint64(value);
            }
        }
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
        buffer.insert(buffer.end(), bytes, bytes + sizeof(T));
    }
}


void QuickCompression::DecompressData(const uint8_t* data, size_t compressedSize, std::vector<uint8_t>& uncompressedBytes) {
    size_t offset = 0;

    uint32_t magic = ReadValue::Read<uint32_t>(data, Endian::Little, offset, compressedSize);
    if (magic != Signature && _byteswap_ulong(magic) != Signature) {
        throw std::runtime_error("Invalid file format");
    }

    Endian endian = (magic == Signature) ? Endian::Little : Endian::Big;

    uint16_t type = ReadValue::Read<uint16_t>(data, endian, offset, compressedSize);
    uint16_t version = ReadValue::Read<uint16_t>(data, endian, offset, compressedSize);
    uint32_t dataOffset = ReadValue::Read<uint32_t>(data, endian, offset, compressedSize);
    uint32_t extraSize = ReadValue::Read<uint32_t>(data, endian, offset, compressedSize);
    int64_t actualCompressedSize = ReadValue::Read<int64_t>(data, endian, offset, compressedSize);
    int64_t uncompressedSize = ReadValue::Read<int64_t>(data, endian, offset, compressedSize);
    uint64_t uncompressedHash = ReadValue::Read<uint64_t>(data, endian, offset, compressedSize);

    offset += 24; // Skip 6 * 4 bytes

    if (type != 1 || version != 1) {
        throw std::runtime_error("Unsupported type or version");
    }

    if (dataOffset != 64) {
        throw std::runtime_error("Invalid data offset");
    }

    std::vector<uint8_t> compressedBytes(data + dataOffset, data + compressedSize);
    std::array<uint16_t, 32> lengths = {};
    std::array<uint16_t, 32> offsets = {};
    size_t x = 0, y = 0, z = 0;

    uncompressedBytes.resize(uncompressedSize);

    while (y < uncompressedSize) {
        uint8_t op = compressedBytes[x++];

        if (op < 32) {
            size_t length = std::min<size_t>(op + 1, uncompressedSize - y);
            std::memcpy(uncompressedBytes.data() + y, compressedBytes.data() + x, length);
            x += length;
            y += length;
        } else {
            uint8_t mode = (op >> 5) & 0x07;
            uint8_t index = (op & 0x1F);

            uint16_t length, offset;
            if (mode == 1) {
                length = lengths[index];
                offset = offsets[index];
            } else {
                offset = static_cast<uint16_t>(compressedBytes[x++]) | (index << 8);
                if (mode == 7) {
                    length = static_cast<uint16_t>(compressedBytes[x++]);
                } else {
                    length = static_cast<uint16_t>(mode);
                }
                length += 1;

                offsets[z] = offset;
                lengths[z] = length;
                z = (z + 1) % 32;
            }

            for (size_t i = 0, j = y - offset; i < length && y < uncompressedSize; i++, j++) {
                uncompressedBytes[y++] = uncompressedBytes[j];
            }
        }
    }

    if (y != uncompressedSize) {
        throw std::runtime_error("Decompression failed");
    }
}

void QuickCompression::DecompressFromBuffer(const std::string& inputFilePath, size_t bufferOffset, const std::string& outputFilePath) {
    HANDLE hFile = CreateFileA(inputFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to open input file");
    }

    LARGE_INTEGER li;
    li.QuadPart = bufferOffset;
    if (!SetFilePointerEx(hFile, li, nullptr, FILE_BEGIN)) {
        CloseHandle(hFile);
        throw std::runtime_error("Failed to set file pointer");
    }

    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize)) {
        CloseHandle(hFile);
        throw std::runtime_error("Failed to get file size");
    }

    std::vector<uint8_t> compressedData(fileSize.QuadPart - bufferOffset);
    DWORD bytesRead;
    if (!ReadFile(hFile, compressedData.data(), static_cast<DWORD>(compressedData.size()), &bytesRead, nullptr)) {
        CloseHandle(hFile);
        throw std::runtime_error("Failed to read compressed data");
    }

    if (bytesRead != compressedData.size()) {
        throw std::runtime_error("Mismatch in expected read size");
    }

    CloseHandle(hFile);

    std::vector<uint8_t> uncompressedBytes;
    DecompressData(compressedData.data(), compressedData.size(), uncompressedBytes);

    std::ofstream output(outputFilePath, std::ios::binary);
    if (!output) {
        throw std::runtime_error("Failed to open output file");
    }

    output.write(reinterpret_cast<const char*>(uncompressedBytes.data()), uncompressedBytes.size());
}





























class CRC64 {
public:
    CRC64();
    uint64_t compute(const std::vector<uint8_t>& data);

private:
    static constexpr uint64_t Polynomial = 0x42F0E1EBA9EA3693;
    uint64_t table[256];

    void generateTable();
};

CRC64::CRC64() {
    generateTable();
}

void CRC64::generateTable() {
    for (uint64_t i = 0; i < 256; ++i) {
        uint64_t crc = i;
        for (uint64_t j = 0; j < 8; ++j) {
            if (crc & 1) {
                crc = (crc >> 1) ^ Polynomial;
            } else {
                crc >>= 1;
            }
        }
        table[i] = crc;
    }
}

uint64_t CRC64::compute(const std::vector<uint8_t>& data) {
    uint64_t crc = 0xFFFFFFFFFFFFFFFF;
    for (uint8_t byte : data) {
        crc = table[(crc ^ byte) & 0xFF] ^ (crc >> 8);
    }
    return ~crc;
}

const size_t MaxMatchLength = 258;
const size_t MaxWindowSize = 8192;

QuickCompression::QuickCompression() : chunkSize(4096) {}

uint32_t QuickCompression::CalculateHash(const uint8_t* data, size_t length) {
    // Using a simple rolling hash function (FNV-1a variant)
    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < length; ++i) {
        hash ^= data[i];
        hash *= 16777619u;
    }
    return hash;
}

void QuickCompression::EndianSwap(UFG::qCompressHeader& header) {
    // Swap endianness of the header fields as necessary
    header.mCompressedNumBytes = _byteswap_uint64(header.mCompressedNumBytes);
    header.mUncompressedNumBytes = _byteswap_uint64(header.mUncompressedNumBytes);
    header.mInPlaceExtraNumBytes = _byteswap_uint64(header.mInPlaceExtraNumBytes);
    header.mUncompressedChecksum = _byteswap_uint64(header.mUncompressedChecksum);
}

void QuickCompression::CompressData(const std::vector<uint8_t>& uncompressedBytes, std::vector<uint8_t>& compressedBytes, size_t& compressedSize, size_t& extraSize) {
    size_t inputLen = uncompressedBytes.size();
    std::unordered_map<uint32_t, std::vector<size_t>> hashTable;
    std::vector<std::pair<uint16_t, uint16_t>> cachedSections(32, {0, 0});
    size_t currCachedSection = 0;

    std::vector<uint8_t> verbatimBytes;
    size_t packPos = 0;

    size_t inPlacePadding = 0;
    size_t inPlacePosition = 0;

    while (packPos < inputLen) {
        size_t longestMatchLen = 0;
        size_t longestMatchPos = 0;

        if (packPos + 3 <= inputLen) {
            uint32_t key = CalculateHash(&uncompressedBytes[packPos], 3);
            if (hashTable.find(key) != hashTable.end()) {
                for (size_t matchPos : hashTable[key]) {
                    if (matchPos >= packPos || packPos - matchPos >= MaxWindowSize) {
                        continue;
                    }
                    size_t matchLen = 3;
                    while (matchLen < MaxMatchLength && packPos + matchLen < inputLen && uncompressedBytes[matchPos + matchLen] == uncompressedBytes[packPos + matchLen]) {
                        matchLen++;
                    }
                    if (matchLen > longestMatchLen) {
                        longestMatchLen = matchLen;
                        longestMatchPos = packPos - matchPos;
                        if (longestMatchLen == MaxMatchLength) {
                            break;
                        }
                    }
                }
            }
            hashTable[key].push_back(packPos);
        }

        if (longestMatchLen < 3) {
            verbatimBytes.push_back(uncompressedBytes[packPos]);
            packPos++;
        } else {
            while (!verbatimBytes.empty()) {
                size_t lenToWrite = std::min(verbatimBytes.size(), size_t(32));
                compressedBytes.push_back(static_cast<uint8_t>(lenToWrite - 1));
                compressedBytes.insert(compressedBytes.end(), verbatimBytes.begin(), verbatimBytes.begin() + lenToWrite);
                verbatimBytes.erase(verbatimBytes.begin(), verbatimBytes.begin() + lenToWrite);
            }

            auto entryToFind = std::make_pair(static_cast<uint16_t>(longestMatchLen), static_cast<uint16_t>(longestMatchPos));
            auto it = std::find(cachedSections.begin(), cachedSections.end(), entryToFind);
            if (it == cachedSections.end()) {
                if (longestMatchLen >= 7) {
                    compressedBytes.push_back(0xE0 | (longestMatchPos >> 8));
                    compressedBytes.push_back(longestMatchPos & 0xFF);
                    compressedBytes.push_back(longestMatchLen - 1);
                } else {
                    compressedBytes.push_back(((longestMatchLen - 1) << 5) | (longestMatchPos >> 8));
                    compressedBytes.push_back(longestMatchPos & 0xFF);
                }

                cachedSections[currCachedSection] = entryToFind;
                currCachedSection = (currCachedSection + 1) % 32;
            } else {
                compressedBytes.push_back(0x20 | std::distance(cachedSections.begin(), it));
            }

            packPos += longestMatchLen;
        }

        size_t compressedLength = compressedBytes.size();
        size_t uncompressedPos = packPos;
        size_t inPlacePos = compressedLength - packPos;
        size_t padding = inPlacePos + 1;

        if (padding > inPlacePadding) {
            inPlacePadding = padding;
        }
        inPlacePosition = compressedLength;
    }

    while (!verbatimBytes.empty()) {
        size_t lenToWrite = std::min(verbatimBytes.size(), size_t(32));
        compressedBytes.push_back(static_cast<uint8_t>(lenToWrite - 1));
        compressedBytes.insert(compressedBytes.end(), verbatimBytes.begin(), verbatimBytes.begin() + lenToWrite);
        verbatimBytes.erase(verbatimBytes.begin(), verbatimBytes.begin() + lenToWrite);
    }

    size_t paddingNeeded = (4096 - (compressedBytes.size() % 4096)) % 4096;
    compressedBytes.insert(compressedBytes.end(), paddingNeeded, 0);
    compressedSize = compressedBytes.size();

    if (compressedSize > inputLen) {
        extraSize += compressedSize - inputLen;
    }

    size_t inPlaceExtraBytes = inPlacePosition + inPlacePadding;
    if (inPlaceExtraBytes > inputLen) {
        extraSize += inPlaceExtraBytes - inputLen;
    }

    extraSize += -(extraSize + inputLen - compressedSize) & 7;
}

std::vector<uint8_t> QuickCompression::CompressToBuffer(const std::string& inputFilePath) {
    std::ifstream input(inputFilePath, std::ios::binary | std::ios::ate);
    if (!input) {
        throw std::runtime_error("Failed to open input file");
    }

    size_t fileSize = input.tellg();
    std::cerr << "File size: " << fileSize << std::endl;
    input.seekg(0, std::ios::beg);

    std::vector<uint8_t> uncompressedBytes(fileSize);
    input.read(reinterpret_cast<char*>(uncompressedBytes.data()), fileSize);
    std::cerr << "Read uncompressed bytes: " << uncompressedBytes.size() << std::endl;

    std::vector<uint8_t> compressedBytes;
    compressedBytes.reserve(fileSize);  // Reserve the same size as uncompressedBytes for safety
    std::cerr << "Reserved compressed bytes: " << compressedBytes.capacity() << std::endl;

    size_t compressedSize = 0;
    size_t extraSize = 0;

    try {
        CompressData(uncompressedBytes, compressedBytes, compressedSize, extraSize);
    } catch (const std::bad_alloc& e) {
        std::cerr << "Memory allocation failed during compression: " << e.what() << std::endl;
        throw;
    }

    std::cerr << "Compressed size: " << compressedSize << std::endl;
    std::cerr << "Extra size: " << extraSize << std::endl;

    CRC64 crc64;
    uint64_t uncompressedChecksum = crc64.compute(uncompressedBytes);

    UFG::qCompressHeader header;
    header.mID = Signature;
    header.mType = 1;
    header.mVersion = 1;
    header.mDataOffset = 64;
    header.mCompressedNumBytes = compressedSize;
    header.mUncompressedNumBytes = fileSize;
    header.mInPlaceExtraNumBytes = extraSize;
    header.mUncompressedChecksum = uncompressedChecksum;

    // how calculate left over bytes isn't understood, trying this...
    header.mInPlaceExtraNumBytes = (8-(compressedSize % 8)) % 8;
    if ((header.mUncompressedNumBytes - header.mCompressedNumBytes) != 0 && header.mInPlaceExtraNumBytes == 0) {header.mInPlaceExtraNumBytes = 8;}


    // Swap endianness of the header
    //EndianSwap(header);

    std::vector<uint8_t> finalBuffer;
    finalBuffer.reserve(64 + compressedSize);
    std::cerr << "Reserved final buffer size: " << finalBuffer.capacity() << std::endl;

    // Header
    ReadValue::Write<uint32_t>(finalBuffer, header.mID, Endian::Little);
    ReadValue::Write<uint16_t>(finalBuffer, header.mType, Endian::Little); // Type
    ReadValue::Write<uint16_t>(finalBuffer, header.mVersion, Endian::Little); // Version
    ReadValue::Write<uint32_t>(finalBuffer, header.mDataOffset, Endian::Little); // Data offset
    ReadValue::Write<uint32_t>(finalBuffer, header.mInPlaceExtraNumBytes, Endian::Little); // Extra size
    ReadValue::Write<int64_t>(finalBuffer, header.mCompressedNumBytes, Endian::Little); // Compressed size
    ReadValue::Write<int64_t>(finalBuffer, header.mUncompressedNumBytes, Endian::Little); // Uncompressed size
    ReadValue::Write<uint64_t>(finalBuffer, header.mUncompressedChecksum, Endian::Little); // Uncompressed hash

    for (size_t i = 0; i < 6; ++i) {
        ReadValue::Write<uint32_t>(finalBuffer, 0, Endian::Little);
    }

    // Insert compressed data after header
    finalBuffer.insert(finalBuffer.end(), compressedBytes.begin(), compressedBytes.end());

    return finalBuffer;
}

void QuickCompression::CompressToFile(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::vector<uint8_t> compressedBytes = CompressToBuffer(inputFilePath);

    std::ofstream output(outputFilePath, std::ios::binary);
    if (!output) {
        throw std::runtime_error("Failed to open output file");
    }

    output.write(reinterpret_cast<const char*>(compressedBytes.data()), compressedBytes.size());
}
