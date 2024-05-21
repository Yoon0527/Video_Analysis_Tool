#include "ModelLoader.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>

ModelLoader::ModelLoader(const std::string& key) : key(key) {
    ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        handleErrors();
    }
}

ModelLoader::~ModelLoader() {
    EVP_CIPHER_CTX_free(ctx);
}

std::vector<unsigned char> ModelLoader::readFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("파일을 열 수 없습니다: " + filePath);
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<unsigned char> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("파일을 읽는 데 실패했습니다: " + filePath);
    }
    return buffer;
}

std::string ModelLoader::decrypt(const std::vector<unsigned char>& encrypted) {
    unsigned char iv[AES_BLOCK_SIZE] = { 0 };

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(key.data()), iv)) {
        handleErrors();
    }

    std::vector<unsigned char> decrypted(encrypted.size());
    int len;
    if (1 != EVP_DecryptUpdate(ctx, decrypted.data(), &len, encrypted.data(), encrypted.size())) {
        handleErrors();
    }
    int plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, decrypted.data() + len, &len)) {
        handleErrors();
    }
    plaintext_len += len;

    decrypted.resize(plaintext_len);
    return std::string(decrypted.begin(), decrypted.end());
}

void ModelLoader::handleErrors() {
    ERR_print_errors_fp(stderr);
    abort();
}

Ort::Session ModelLoader::loadModel(const std::string& filePath) {
    std::vector<unsigned char> encryptedModel = readFile(filePath);
    std::string decryptedModel = decrypt(encryptedModel);

    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "ModelLoader");
    Ort::SessionOptions sessionOptions;
    return Ort::Session(env, decryptedModel.data(), decryptedModel.size(), sessionOptions);
}
