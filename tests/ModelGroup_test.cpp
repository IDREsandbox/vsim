#include <QTest>
#include <QDebug>
#include <QSignalSpy>
#include <vector>
#include "ModelGroup.h"

class ModelGroup_test : public QObject {
	Q_OBJECT
private slots:

};

QTEST_MAIN(ModelGroup_test)
#include "ModelGroup_test.moc"