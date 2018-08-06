#ifndef SIMPLEWORKER_H
#define SIMPLEWORKER_H

#include <QThread>
#include <QObject>
#include <functional>

class WorkerObject : public QObject {
	Q_OBJECT;
public:
	WorkerObject(QObject *parent = nullptr);
signals:
	void sDone();
};

class SimpleWorker : public QObject {
	Q_OBJECT;
public:
	SimpleWorker(QObject *parent = nullptr);
	~SimpleWorker() override;
	void setFunc(std::function<bool()>);
	void start();
	bool result() const;
	QThread *thread();


	// an object that lives in the thread
	// if you want objects to live in thread then set
	// parent to this slave
	// gets deleted when SimpleWorker is deleted
	WorkerObject *slave();

signals:
	void sDone();

private:
	QThread m_thread;
	WorkerObject *m_slave; // guy sent over to the thread
	std::function<bool()> m_func;
	bool m_result;
};

#endif