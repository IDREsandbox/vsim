﻿#include "LockTable.h"

#include <QDebug>

#include "types_generated.h"

LockTable::LockTable(QObject * parent)
	: QObject(parent),
	m_locked(false),
	m_has_password(false)
{
}

LockTable &LockTable::operator=(const LockTable &other)
{
	m_locked = other.m_locked;
	m_has_password = other.m_has_password;
	m_hash = other.m_hash;
	return *this;
}

bool LockTable::operator==(const LockTable & other) const
{
	// both unlocked
	if (!m_locked && !other.m_locked) return true;

	// both permalocked
	if (isPermanent() && other.isPermanent()) return true;

	// password locked - compare hashes
	if (hasPassword() && other.hasPassword()) {
		return m_hash == other.m_hash;
	}

	return false;
}

void LockTable::readLockTable(const VSim::FlatBuffers::LockTable *fb_lock)
{
	if (fb_lock == nullptr) return;
	
	// unlocked
	m_locked = fb_lock->locked();
	if (!m_locked) return;

	// permalocked
	m_has_password = fb_lock->has_password();
	if (!m_has_password) return;

	// password
	auto *fb_hash = fb_lock->hash();

	// fb hash is broken?
	if (!fb_hash) {
		m_has_password = false;
		return;
	}

	auto fbToStd =
		[](const flatbuffers::Vector<uint8_t> *fb_vec)
		-> std::vector<uint8_t> {
		auto *p = fb_vec->data();
		if (p == nullptr) return {};
		return std::vector<uint8_t>(p, p + fb_vec->size());
	};

	m_hash.m_hash = fbToStd(fb_hash->hash());
	m_hash.m_salt = fbToStd(fb_hash->salt());
	m_hash.m_iterations = fb_hash->iterations();
}

flatbuffers::Offset<VSim::FlatBuffers::LockTable>
	LockTable::createLockTable(
		flatbuffers::FlatBufferBuilder *builder) const
{
	flatbuffers::Offset<VSim::FlatBuffers::PBKDF2Hash> o_hash;
	if (hasPassword()) {
		auto o_hash_hash = builder->CreateVector(m_hash.m_hash);
		auto o_salt = builder->CreateVector(m_hash.m_salt);

		VSim::FlatBuffers::PBKDF2HashBuilder b_hash(*builder);
		b_hash.add_hash(o_hash_hash);
		b_hash.add_salt(o_salt);
		b_hash.add_iterations(m_hash.m_iterations);
		o_hash = b_hash.Finish();
	}

	VSim::FlatBuffers::LockTableBuilder b_table(*builder);
	b_table.add_has_password(m_has_password);
	b_table.add_locked(m_locked);
	if (hasPassword()) b_table.add_hash(o_hash);
	auto o_table = b_table.Finish();

	return o_table;
}

void LockTable::lock()
{
	m_locked = true;
	m_has_password = false;
	m_hash = {};
}

void LockTable::lockWithPasswordHash(HashLock h)
{
	m_locked = true;
	m_has_password = true;
	m_hash = h;
}

bool LockTable::isLocked() const
{
	return m_locked;
}

void LockTable::lockWithPassword(QString s)
{
	m_locked = true;
	m_has_password = true;
	m_hash = HashLock::generateHash(s.toStdString());
}

bool LockTable::checkPassword(QString s) const
{
	if (!m_locked) return false;
	if (!m_has_password) return false; // no pw -> can't unlock
	return m_hash.checkPassword(s.toStdString());
}

bool LockTable::unlock(QString s)
{
	if (!m_locked) return true;
	if (!m_has_password) return false;

	bool ok = m_hash.checkPassword(s.toStdString());
	if (ok) {
		m_locked = false;
		m_has_password = false;
		return true;
	}

	return false;
}

bool LockTable::massUnlock(const std::vector<LockTable*> table, QString password)
{
	if (table.size() == 0) return true;
	
	// make sure they're all pw locked, same hash
	HashLock hash = table[0]->m_hash;

	for (LockTable *lock : table) {
		bool ok = lock->isLocked()
			&& lock->hasPassword()
			&& (lock->m_hash == hash);
		if (!ok) {
			qInfo() << "mass unlock fail - differing";
			return false;
		}
	}

	bool pw_ok = hash.checkPassword(password.toStdString());
	if (!pw_ok) return false;

	for (LockTable *lock : table) {
		lock->m_locked = false;
		lock->m_has_password = false;
		lock->m_hash = {};
	}
	return true;
}

bool LockTable::hasPassword() const
{
	return m_locked && m_has_password;
}

bool LockTable::isPermanent() const
{
	return m_locked && !m_has_password;
}

const HashLock & LockTable::hash() const
{
	return m_hash;
}
