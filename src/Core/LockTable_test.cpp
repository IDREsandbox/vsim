#include <QTest>
#include <QDebug>

#include "LockTable.h"

class LockTable_test : public QObject {
	Q_OBJECT
private slots:

	void basicTest() {
		// permalock
		LockTable plock;
		plock.lock();
		QCOMPARE(plock.unlock(""), false);
		QCOMPARE(plock.isLocked(), true);
		QCOMPARE(plock.isPermanent(), true);

		// password locked
		LockTable lock;
		QCOMPARE(lock.isLocked(), false);

		lock.lockWithPassword("foobar");
		QCOMPARE(lock.isLocked(), true);
		QCOMPARE(lock.isPermanent(), false);
		QCOMPARE(lock.hasPassword(), true);

		bool bad_pw_success = lock.unlock("zzz");
		QCOMPARE(bad_pw_success, false);
		QCOMPARE(lock.isLocked(), true);

		bool good_pw_success = lock.unlock("foobar");
		QCOMPARE(good_pw_success, true);
		QCOMPARE(lock.isLocked(), false);

		// precomputed hash locked
		LockTable hlock;
		LockTable hlock2;
		LockTable hlock3;
		HashLock hash = HashLock::generateHash("password");

		hlock.lockWithPasswordHash(hash);
		QCOMPARE(hlock.isLocked(), true);
		QCOMPARE(hlock.isPermanent(), false);
		QCOMPARE(hlock.hasPassword(), true);

		hlock2.lockWithPasswordHash(hash);
		hlock3.lockWithPasswordHash(hash);
		QCOMPARE(hlock.hash(), hlock2.hash());
		QCOMPARE(hlock.hash(), hlock3.hash());
	}

	void writeReadTest() {

		auto writeReadVerify =
			[](const LockTable &lock)
		{
			// write flatbuffer
			flatbuffers::FlatBufferBuilder builder;
			auto o_lock = lock.createLockTable(&builder);
			builder.Finish(o_lock);

			// read flatbuffer
			const VSim::FlatBuffers::LockTable *fb_lock
				= flatbuffers::GetRoot<VSim::FlatBuffers::LockTable>(
					builder.GetBufferPointer());

			LockTable lock2;
			lock2.readLockTable(fb_lock);

			QCOMPARE(lock, lock2);
		};

		LockTable lock;
		lock.lockWithPassword("hello");
		writeReadVerify(lock);

		LockTable lock2;
		lock2.lockWithPassword("");
		writeReadVerify(lock2);

		LockTable lock3;
		lock3.lock();
		writeReadVerify(lock3);

		LockTable lock4;
		writeReadVerify(lock4);

	}

};

QTEST_MAIN(LockTable_test)
#include "LockTable_test.moc"