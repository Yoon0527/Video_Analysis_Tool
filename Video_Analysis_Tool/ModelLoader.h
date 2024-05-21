#pragma once
#include <string>
#include <vector>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <onnxruntime_cxx_api.h>

class ModelLoader {
public:
    ModelLoader(const std::string& key);
    ~ModelLoader();
    Ort::Session loadModel(const std::string& filePath);

private:
    std::string key;
    EVP_CIPHER_CTX* ctx;

    std::vector<unsigned char> readFile(const std::string& filePath);
    std::string decrypt(const std::vector<unsigned char>& encrypted);
    void handleErrors();
};