#ifndef HASHLOCK_H
#define HASHLOCK_H

#include <stdint.h>
#include <vector>

struct HashLock {
	static HashLock generateHash(std::string password);
	static HashLock generateHashPBKDF2(std::string password,
		size_t salt_size,
		size_t hash_size,
		size_t iterations);

	bool operator==(const HashLock &other) const;

	//HashLock();
	//HashLock(std::vector<uint8_t> salt, std::vector<uint8_t> hash, uint32_t iterations);

	bool checkPassword(const std::string &password) const;

	void debug();

	std::vector<uint8_t> m_salt;
	std::vector<uint8_t> m_hash;
	uint32_t m_iterations;

	// enum algorithm?
};


#endif