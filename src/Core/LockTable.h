#ifndef LOCKTABLE_H
#define LOCKTABLE_H

#include <QObject>
#include <flatbuffers/flatbuffers.h>

#include "HashLock.h"
#include "types_generated.h"

namespace VSim {
	namespace FlatBuffers {
		struct LockTable;
	}
}

class LockTable : public QObject {
	Q_OBJECT;
public:
	LockTable(QObject *parent = nullptr);

	LockTable &operator=(const LockTable &other);
	bool operator==(const LockTable &other) const;

	bool lock();
	bool lockWithPasswordHash(HashLock h);
	bool lockWithPassword(QString s);
	bool checkPassword(QString s) const;
	bool unlock(QString s); // false if failed

	bool isLocked() const;
	bool hasPassword() const;
	bool isPermanent() const; // permanent if no password
	const HashLock &hash() const;

	// serialization
	void readLockTable(const VSim::FlatBuffers::LockTable *fb_lock);
	flatbuffers::Offset<VSim::FlatBuffers::LockTable>
		createLockTable(flatbuffers::FlatBufferBuilder *builder) const;

	void readLockTableT(const VSim::FlatBuffers::LockTableT *fb_lock);
	void createLockTableT(VSim::FlatBuffers::LockTableT *fb_lock) const;

public: // static methods
	// Locks a bunch of locks with the same hash, allowing them to be unlocked
	// as a bundle. This isn't the case otherwise - since each would have
	// a different random salt.
	// successes outputs the number of successful locks
	// fails outputs the number of failed locks

	static bool massUnlock(const std::vector<LockTable*> &locks, QString password);
	//static void clumpLocked();

	//static void massLock(const std::vector<LockTable*> &locks, QString password,
	//	int *out_success = nullptr,
	//	int *out_fail = nullptr);


	//static void massLockWithHash(const std::vector<LockTable*> &locks, HashLock hash,
	//	int *out_success = nullptr,
	//	int *out_fail = nullptr);

	//static void massLock();

signals:
	void sLockChanged(bool locked);

private:
	bool m_locked;
	bool m_has_password;
	HashLock m_hash;
	// algo?
	// expiration date?
};

#endif