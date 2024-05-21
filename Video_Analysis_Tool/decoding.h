//#include <string>
//#include <openssl/buffer.h>
//#include <openssl/evp.h>
//#include <vector>
//
//class AESCipher {
//private:
//    std::string key;
//    const int maxDecryptedSize = EVP_MAX_BLOCK_LENGTH;
//
//public:
//    AESCipher(const std::string& key);
//    void handleErrors();
//    //std::string decrypt(const std::string& enc);
//    std::string decrypt(const std::vector<unsigned char>& encrypted);
//    std::string unpad(const std::string& s);
//};