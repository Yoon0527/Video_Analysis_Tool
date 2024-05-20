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
	SHA256_Init(&sha256); // SHA-256 초기화
	SHA256_Update(&sha256, key.c_str(), key.size()); // 데이터 업데이트
	SHA256_Final(hash, &sha256); // 해시 계산 완료
	return std::vector<unsigned char>(hash, hash + SHA256_DIGEST_LENGTH); // 해시 결과를 벡터로 반환
}

// 패딩을 제거하는 함수 (PKCS7 패딩 방식 가정)
std::string AESCipher::unpad(const std::string& str) {
	size_t padding_length = static_cast<size_t>(str[str.size() - 1]); // 마지막 바이트를 패딩 길이로 간주
	return str.substr(0, str.size() - padding_length); // 패딩을 제거한 문자열 반환
}

// AESCipher 클래스 생성자: 키를 SHA-256 해시로 변환하여 저장
AESCipher::AESCipher(const std::string& key) {
	this->key = sha256(key);
}

// 암호화된 텍스트를 복호화하는 함수
std::string AESCipher::decrypt(const std::string& enc) {
	std::vector<unsigned char> decoded_enc = base64_decode(enc); // Base64 디코딩

	AES_KEY decryptKey;
	AES_set_decrypt_key(this->key.data(), 256, &decryptKey); // 복호화 키 설정

	unsigned char iv[AES_BLOCK_SIZE];
	memset(iv, 0x00, AES_BLOCK_SIZE); // 초기화 벡터를 0으로 설정 (고정 IV 사용)

	std::vector<unsigned char> decrypted(decoded_enc.size());
	AES_cbc_encrypt(decoded_enc.data(), decrypted.data(), decoded_enc.size(), &decryptKey, iv, AES_DECRYPT); // AES 복호화 수행

	std::string decrypted_str(decrypted.begin(), decrypted.end()); // 복호화된 데이터 문자열로 변환
	return unpad(decrypted_str); // 패딩 제거 후 반환
}