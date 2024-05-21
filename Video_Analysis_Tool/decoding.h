#include <string>
#include <openssl/buffer.h>
#include <openssl/evp.h>

class AESCipher {
private:
    std::string key;
    const int maxDecryptedSize = EVP_MAX_BLOCK_LENGTH;

public:
    AESCipher(const std::string& key);
    std::string decrypt(const std::string& enc);
    std::string unpad(const std::string& s);
};