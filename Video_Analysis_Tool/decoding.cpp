//#include "decoding.h"
//
//#include <openssl/aes.h>
//#include <openssl/sha.h>
//#include <openssl/bio.h>
//#include <openssl/err.h>
//#include <openssl/evp.h>
//#include <stdexcept>
//
//AESCipher::AESCipher(const std::string& key) {
//    unsigned char hash[SHA256_DIGEST_LENGTH];
//    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
//    EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL);
//    EVP_DigestUpdate(mdctx, key.c_str(), key.length());
//    EVP_DigestFinal_ex(mdctx, hash, NULL);
//    EVP_MD_CTX_free(mdctx);
//    this->key = std::string(reinterpret_cast<char*>(hash), SHA256_DIGEST_LENGTH);
//}
//
//std::string AESCipher::unpad(const std::string& s) {
//    char padSize = s[s.length() - 1];
//    return s.substr(0, s.length() - padSize);
//}
//
////std::string AESCipher::decrypt(const std::string& enc) {
////    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
////    if (!ctx) {
////        // Handle error (failed to create context)
////        return "";
////    }
////
////    BIO* mem = BIO_new_mem_buf(enc.c_str(), enc.length());
////    if (!mem) {
////        EVP_CIPHER_CTX_free(ctx);
////        // Handle error (failed to create BIO)
////        return "";
////    }
////
////    BIO* b64 = BIO_new(BIO_f_base64());
////    mem = BIO_push(b64, mem);
////
////    int outLen = enc.length() + EVP_MAX_BLOCK_LENGTH; // Max possible output length
////    char* decrypted = new char[outLen];
////
////    int decryptedLen = 0;
////    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(key.c_str()), reinterpret_cast<const unsigned char*>("0000000000000000"))) {
////        // Handle error (failed to initialize decryption)
////        delete[] decrypted;
////        BIO_free_all(mem);
////        EVP_CIPHER_CTX_free(ctx);
////        return "";
////    }
////
////    if (!EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(decrypted), &decryptedLen, reinterpret_cast<unsigned char*>(const_cast<char*>(enc.c_str())), enc.length())) {
////        // Handle error (decryption update failed)
////        delete[] decrypted;
////        BIO_free_all(mem);
////        EVP_CIPHER_CTX_free(ctx);
////        return "";
////    }
////
////    int finalLen = 0;
////    if (!EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(decrypted) + decryptedLen, &finalLen)) {
////        // Handle error (decryption finalization failed)
////        delete[] decrypted;
////        BIO_free_all(mem);
////        EVP_CIPHER_CTX_free(ctx);
////        return "";
////    }
////    decryptedLen += finalLen;
////
////    EVP_CIPHER_CTX_free(ctx);
////    BIO_free_all(mem);
////
////    // Convert decrypted data to string and return
////    std::string decryptedStr(decrypted, decryptedLen);
////    delete[] decrypted;
////    return unpad(decryptedStr);
////}
//void AESCipher::handleErrors() {
//    ERR_print_errors_fp(stderr);
//    abort();
//}
//std::string AESCipher::decrypt(const std::vector<unsigned char>& encrypted) {
//    unsigned char iv[AES_BLOCK_SIZE] = { 0 };
//    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
//
//    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(key.data()), iv)) {
//        handleErrors();
//    }
//
//    std::vector<unsigned char> decrypted(encrypted.size());
//    int len;
//    if (1 != EVP_DecryptUpdate(ctx, decrypted.data(), &len, encrypted.data(), encrypted.size())) {
//        handleErrors();
//    }
//    int plaintext_len = len;
//
//    if (1 != EVP_DecryptFinal_ex(ctx, decrypted.data() + len, &len)) {
//        handleErrors();
//    }
//    plaintext_len += len;
//
//    decrypted.resize(plaintext_len);
//    return std::string(decrypted.begin(), decrypted.end());
//}