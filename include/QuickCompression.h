#ifndef QUICKCOMPRESSION_H
#define QUICKCOMPRESSION_H

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <stdexcept>
#include <unordered_map>
#include <string>
#include <utility>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>
#include <queue>

enum class Endian {
    Little,
    Big
};

namespace UFG {
    struct qCompressHeader {
        uint32_t mID;
        uint16_t mType;
        uint16_t mVersion;
        uint32_t mDataOffset;
        uint32_t mInPlaceExtraNumBytes;
        int64_t mCompressedNumBytes;
        int64_t mUncompressedNumBytes;
        uint64_t mUncompressedChecksum;
    };
}

namespace ReadValue {
    template <typename T>
    T Read(const uint8_t* buffer, Endian endian, size_t& offset);

    template <typename T>
    void Write(std::vector<uint8_t>& buffer, T value, Endian endian);
}

class QuickCompression {
public:
    QuickCompression();

    void DecompressFromBuffer(const std::string& inputFilePath, size_t bufferOffset, const std::string& outputFilePath);


    std::vector<uint8_t> CompressToBuffer(const std::string& inputFilePath);
    void CompressToFile(const std::string& inputFilePath, const std::string& outputFilePath);


 void DecompressData(const uint8_t* data, size_t compressedSize, std::vector<uint8_t>& uncompressedBytes);

private:
    const size_t windowSize = 8196;
    const size_t minMatchLength=3;
    const size_t maxMatchLength=258;
    const size_t numThreads=4;



    const size_t chunkSize;

    uint32_t CalculateHash(const uint8_t* data, size_t length);
    void EndianSwap(UFG::qCompressHeader& header);
    void CompressData(const std::vector<uint8_t>& uncompressedBytes, std::vector<uint8_t>& compressedBytes, size_t& compressedSize, size_t& extraSize);
};

#endif // QUICKCOMPRESSION_H
