#ifndef STRING_READER_H
#define STRING_READER_H

#include <string.h>
#include <stdexcept>

class StringReader {
    std::string data;
    size_t pos;
public:
    StringReader(const std::string& _data, size_t _pos = 0) : data(_data), pos(_pos) {
        if (pos != 0 && pos > data.size()) throw std::invalid_argument( "not enough data available" );
    }

    void rewind(size_t p = 0) { pos = p; }
    size_t getPos() { return pos; }
    std::string get() { return get(data.size() - pos); }
    void skip(size_t len) { std::string s = get(len); }

    std::string get(size_t len) {
        if (len > data.size() - pos) throw std::invalid_argument( "not enough data available" );
        std::string res = data.substr(pos, len);
        pos += len;
        return res;
    }

    long getBinaryNumber(size_t len, bool bigEndian = true) {
        std::string s = get(len);
        if (!bigEndian) {
            std::string rs(s.rbegin(), s.rend());
            s = rs;
        }
        long res = 0;
        for (unsigned char c : s) {
            res <<= 8;
            res += c;
        }
        return res;
    }

    unsigned char getUnsignedChar(bool bigEndian = true) {
        return (unsigned char)getBinaryNumber(1,bigEndian);
    }
};

#endif
