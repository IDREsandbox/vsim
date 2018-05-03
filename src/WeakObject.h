#include <QtCore/QCoreApplication>
#include <memory>
#include <QObject>
#include <QUndoStack>
#include <QDebug>
// what happens if u have shared pointers of qobjects?

class WeakObject : public QObject {
public:
	WeakObject(QObject *parent = nullptr);
	~WeakObject() override;
};