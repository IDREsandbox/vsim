#include "HashLock.h"

#include <cryptopp/osrng.h>
#include <cryptopp/pwdbased.h>
#include <stdint.h>
#include <QDebug>

HashLock HashLock::generateHashPBKDF2(std::string password,
	size_t salt_size,
	size_t hash_size,
	size_t iterations)
{
	const CryptoPP::byte *pw_bytes =
		reinterpret_cast<const CryptoPP::byte*>(password.c_str());

	// generate random salt
	std::vector<uint8_t> salt(salt_size, 0);
	CryptoPP::OS_GenerateRandomBlock(true, salt.data(), salt_size);

	std::vector<uint8_t> hash(hash_size, 0);

	CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256> pbkdf2;

	unsigned int nits = pbkdf2.DeriveKey(hash.data(), hash.size(), 0,
		pw_bytes, password.size(),
		salt.data(), salt.size(), (unsigned int)iterations);

	HashLock out;
	out.m_salt = salt;
	out.m_hash = hash;
	out.m_iterations = nits;

	return out;
}

bool HashLock::operator==(const HashLock & other) const
{
	return m_salt == other.m_salt
		&& m_hash == other.m_hash
		&& m_iterations == other.m_iterations;
}

HashLock HashLock::generateHash(std::string password)
{
	size_t iterations = 20000;
#ifdef _DEBUG
	iterations = 1000;
#endif // DEBUG
	return generateHashPBKDF2(password, 256, 256, iterations);
}

bool HashLock::checkPassword(const std::string & password) const
{
	const CryptoPP::byte *pw_bytes =
		reinterpret_cast<const CryptoPP::byte*>(password.c_str());

	CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256> pbkdf2;

	std::vector<uint8_t> hash_bytes(m_hash.size(), 0);

	pbkdf2.DeriveKey(hash_bytes.data(), hash_bytes.size(), 0,
		pw_bytes, password.size(),
		m_salt.data(), m_salt.size(),
		m_iterations);

	// compare hashes
	return m_hash == hash_bytes;
}

void HashLock::debug()
{
	QByteArray h1h(reinterpret_cast<char*>(m_hash.data()), (int)m_hash.size());
	QByteArray h1s(reinterpret_cast<char*>(m_salt.data()), (int)m_salt.size());
	qInfo() << "hash - " << h1h << "\nsalt - " << h1s << "\n" << m_iterations;
}
