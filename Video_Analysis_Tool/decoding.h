#pragma once

#include <string>
#include <vector>


class AESCipher {
public:
	AESCipher(const std::string& key);
	std::string decrypt(const std::string& enc);

private:
    std::vector<unsigned char> key;
    std::vector<unsigned char> sha256(const std::string& key);
    std::vector<unsigned char> base64_decode(const std::string& in);
    std::string unpad(const std::string& str);
};
