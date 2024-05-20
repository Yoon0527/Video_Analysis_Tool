#pragma once

#include <string>
#include <vector>


class AESCipher {
public:
    AESCipher(const std::string& key);
    std::vector<unsigned char> decrypt(const std::vector<unsigned char>& enc);

private:
    std::vector<unsigned char> key;
    std::vector<unsigned char> sha256(const std::string& key);
    std::vector<unsigned char> base64_decode(const std::string& in);
    std::vector<unsigned char> unpad(const std::vector<unsigned char>& str);

};
