#ifndef LOCKTABLE_H
#define LOCKTABLE_H

#include <QObject>
#include <flatbuffers/flatbuffers.h>

#include "HashLock.h"

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

	void lock();
	void lockWithPasswordHash(HashLock h);
	void lockWithPassword(QString s);
	bool checkPassword(QString s) const;
	bool unlock(QString s); // false if failed
	static bool massUnlock(const std::vector<LockTable*> table, QString password);
	static void clumpLocked();

	bool isLocked() const;
	bool hasPassword() const;
	bool isPermanent() const; // permanent if no password
	const HashLock &hash() const;

	// serialization
	void readLockTable(const VSim::FlatBuffers::LockTable *fb_lock);
	flatbuffers::Offset<VSim::FlatBuffers::LockTable>
		createLockTable(flatbuffers::FlatBufferBuilder *builder) const;

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