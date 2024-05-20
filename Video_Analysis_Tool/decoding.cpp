#include "decoding.h"
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/buffer.h>
#include <cstring>
#include <vector>

std::vector<unsigned char> AESCipher::base64_decode(const std::string& in) {
    BIO* bio, * b64;
    int decodeLen = (int)in.size() * 3 / 4;
    std::vector<unsigned char> out(decodeLen);

    bio = BIO_new_mem_buf(in.data(), (int)in.size());
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    int outlen = BIO_read(bio, out.data(), (int)in.size());
    out.resize(outlen);

    BIO_free_all(bio);

    return out;
}

std::vector<unsigned char> AESCipher::sha256(const std::string& key) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, key.c_str(), key.size());
    SHA256_Final(hash, &sha256);
    return std::vector<unsigned char>(hash, hash + SHA256_DIGEST_LENGTH);
}

std::vector<unsigned char> AESCipher::unpad(const std::vector<unsigned char>& str) {
    size_t padding_length = static_cast<size_t>(str[str.size() - 1]);
    return std::vector<unsigned char>(str.begin(), str.end() - padding_length);
}

AESCipher::AESCipher(const std::string& key) {
    this->key = sha256(key);
}

std::vector<unsigned char> AESCipher::decrypt(const std::vector<unsigned char>& enc) {
    AES_KEY decryptKey;
    AES_set_decrypt_key(this->key.data(), 256, &decryptKey);

    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0x00, AES_BLOCK_SIZE);

    std::vector<unsigned char> decrypted(enc.size());
    AES_cbc_encrypt(enc.data(), decrypted.data(), enc.size(), &decryptKey, iv, AES_DECRYPT);

    return unpad(decrypted);
}