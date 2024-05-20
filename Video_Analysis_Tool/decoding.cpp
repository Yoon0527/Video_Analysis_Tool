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
	SHA256_Init(&sha256); // SHA-256 �ʱ�ȭ
	SHA256_Update(&sha256, key.c_str(), key.size()); // ������ ������Ʈ
	SHA256_Final(hash, &sha256); // �ؽ� ��� �Ϸ�
	return std::vector<unsigned char>(hash, hash + SHA256_DIGEST_LENGTH); // �ؽ� ����� ���ͷ� ��ȯ
}

// �е��� �����ϴ� �Լ� (PKCS7 �е� ��� ����)
std::string AESCipher::unpad(const std::string& str) {
	size_t padding_length = static_cast<size_t>(str[str.size() - 1]); // ������ ����Ʈ�� �е� ���̷� ����
	return str.substr(0, str.size() - padding_length); // �е��� ������ ���ڿ� ��ȯ
}

// AESCipher Ŭ���� ������: Ű�� SHA-256 �ؽ÷� ��ȯ�Ͽ� ����
AESCipher::AESCipher(const std::string& key) {
	this->key = sha256(key);
}

// ��ȣȭ�� �ؽ�Ʈ�� ��ȣȭ�ϴ� �Լ�
std::string AESCipher::decrypt(const std::string& enc) {
	std::vector<unsigned char> decoded_enc = base64_decode(enc); // Base64 ���ڵ�

	AES_KEY decryptKey;
	AES_set_decrypt_key(this->key.data(), 256, &decryptKey); // ��ȣȭ Ű ����

	unsigned char iv[AES_BLOCK_SIZE];
	memset(iv, 0x00, AES_BLOCK_SIZE); // �ʱ�ȭ ���͸� 0���� ���� (���� IV ���)

	std::vector<unsigned char> decrypted(decoded_enc.size());
	AES_cbc_encrypt(decoded_enc.data(), decrypted.data(), decoded_enc.size(), &decryptKey, iv, AES_DECRYPT); // AES ��ȣȭ ����

	std::string decrypted_str(decrypted.begin(), decrypted.end()); // ��ȣȭ�� ������ ���ڿ��� ��ȯ
	return unpad(decrypted_str); // �е� ���� �� ��ȯ
}