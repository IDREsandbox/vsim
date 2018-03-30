#include <QTest>
#include <QDebug>
#include <QSignalSpy>
#include "VSimSerializer.h"

namespace fb = VSim::FlatBuffers;
class VSimSerializer_test : public QObject {
	Q_OBJECT
private:

private slots:
void writeReadTest() {


}
};

QTEST_MAIN(VSimSerializer_test)
#include "VSimSerializer_test.moc"