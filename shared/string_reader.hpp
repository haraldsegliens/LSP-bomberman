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
    long getRemainingSize() { return data.size()-pos-1; }

    std::string get(size_t len) {
        if (len > data.size() - pos) throw std::invalid_argument( "not enough data available" );
        std::string res = data.substr(pos, len);
        pos += len;
        return res;
    }

    std::string getNullFixedString(size_t len) {
        //find first nullchar
        std::size_t nullChar = std::string::npos;
        for(size_t i = pos; i < pos + len-1; i++) {
            if(data[i] == '\0') {
                nullChar = i;
                break;
            }
        }
        if(nullChar == std::string::npos) {
            return get(len);
        } else if(nullChar-pos < len) {
            return get(nullChar-pos + 1);
        } else {
            return get(len);
        }
    }

    float getDFloat(size_t len) {
        return (float)getBinaryNumber(len)/10.0f;
    }

    int getBinaryNumber(size_t len, bool bigEndian = true) {
        std::string s = get(len);
        if (!bigEndian) {
            std::string rs(s.rbegin(), s.rend());
            s = rs;
        }
        int res = 0;
        for (unsigned char c : s) {
            res <<= 8;
            res += c;
        }
        return res;
    }
};

#endif
