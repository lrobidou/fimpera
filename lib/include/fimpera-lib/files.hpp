#pragma once

#include <sys/stat.h>
#include <iostream>

#include <fstream>
#include <string>
#include <vector>
#include <cstring>

inline bool fileExists(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

namespace hidden {

template <typename T>
inline void writeToFile(std::ofstream& fout, const T& x) {
    fout.write(reinterpret_cast<const char*>(&x), sizeof(x));
}

template <>
inline void writeToFile<std::string>(std::ofstream& fout, const std::string& s) {
    writeToFile(fout, s.length());
    for (const char& c : s) {
        fout.write((const char*)&c, sizeof(unsigned char));
    }
}

template <>
inline void writeToFile<std::vector<bool>>(std::ofstream& fout, const std::vector<bool>& v) {
    std::vector<bool>::size_type n = v.size();
    writeToFile(fout, n);

    // Buffer size (adjust as needed)
    const int bufferSize = 8192;
    char buffer[bufferSize];

    // Pack boolean values into 64 bits and write to the buffer
    uint64_t packedBits = 0;
    int bitPosition = 0;
    int bufferIndex = 0;

    for (bool value : v) {
        packedBits |= (static_cast<uint64_t>(value ? 1 : 0) << bitPosition);
        bitPosition++;

        if (bitPosition == 64) {
            // Write to the buffer
            std::memcpy(&buffer[bufferIndex], &packedBits, sizeof(uint64_t));
            bufferIndex += sizeof(uint64_t);

            // Reset variables for the next iteration
            packedBits = 0;
            bitPosition = 0;

            // Check if the buffer is full
            if (bufferIndex == bufferSize) {
                // Write the buffer to the file
                fout.write(buffer, bufferSize);
                bufferIndex = 0;
            }
        }
    }

    // Write any remaining bits in the buffer
    if (bitPosition > 0) {
        std::memcpy(&buffer[bufferIndex], &packedBits, sizeof(uint64_t));
        bufferIndex += sizeof(uint64_t);
    }

    if (bufferIndex > 0) {
        // std::cout << "OVERFLOW: bufferIndex " << bufferIndex << std::endl;
        // while (bufferIndex < bufferSize) {
        //     buffer[bufferIndex] = 0;
        //     bufferIndex++;
        // }
        // Write the remaining buffer to the file
        fout.write(buffer, bufferIndex);
    }

}

template <typename T, typename... Args>
inline void writeToFile(std::ofstream& fout, T t, Args &... args) {
    writeToFile(fout, t);
    writeToFile(fout, args...);
}

}  // namespace hidden

template <typename... Args>
void writeToFile(std::ofstream& fout, Args &... args) {
    hidden::writeToFile(fout, args...);
}

template <typename T>
inline T getFromFile(std::ifstream& fin) {
    T x;
    fin.read(reinterpret_cast<char*>(&x), sizeof(x));
    return x;
}

template <>
inline std::string getFromFile<std::string>(std::ifstream& fin) {
    const std::size_t& length = getFromFile<std::size_t>(fin);
    char ct;
    std::string s = "";
    for (std::size_t i = 0; i < length; i++) {
        fin.read((char*)&ct, sizeof(unsigned char));
        s += ct;
    }
    return s;
}



// time to index and dump = 283
// time to index and dump = 156.366 (u64)
// 143.905 (buffer)


// 1665