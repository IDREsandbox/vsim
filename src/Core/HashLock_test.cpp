#include "HashLock.h"

#include <QTest>
#include <QDebug>
#include <string>

class HashLock_test : public QObject {
	Q_OBJECT;
private:

	private slots:

	void writeReadTest() {
		std::string password = "hello world 123";

		HashLock h1 = HashLock::generateHash(password);

		QByteArray h1h(reinterpret_cast<char*>(h1.m_hash.data()), (int)h1.m_hash.size());
		QByteArray h1s(reinterpret_cast<char*>(h1.m_salt.data()), (int)h1.m_salt.size());
		qDebug() << "hash - " << h1h << "\nsalt - " << h1s << "\n" << h1.m_iterations;
		
		QVERIFY(h1.checkPassword(password));
		QCOMPARE(h1.checkPassword("foo"), false);
	}

	void saltTest() {
		std::string password = "hello world 123";

		HashLock h1 = HashLock::generateHash(password);
		HashLock h2 = HashLock::generateHash(password);

		QVERIFY(h1.m_salt != h2.m_salt);
		QVERIFY(h1.m_hash != h2.m_hash);
	}

};

QTEST_MAIN(HashLock_test)
#include "HashLock_test.moc"