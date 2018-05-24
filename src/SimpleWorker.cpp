#include "SimpleWorker.h"
#include <QDebug>

SimpleWorker::SimpleWorker(QObject *parent)
    : QObject(parent) {
	setFunc([this]() -> bool {
		return true;
	});

	m_slave = new WorkerObject();
	connect(&m_thread, &QThread::finished, m_slave, &QObject::deleteLater);
	//connect(m_slave, &WorkerObject::sDone, this, &SimpleWorker::sDone);
	connect(m_slave, &WorkerObject::sDone, this, [this]() {
		emit sDone();
	});
	m_slave->moveToThread(&m_thread);
}

SimpleWorker::~SimpleWorker() {
	// finishes thread
	// -> deleteLater
    m_thread.quit();
    m_thread.wait();
}

void SimpleWorker::setFunc(std::function<bool()> f) {
    m_func = f;
}

void SimpleWorker::start() {
	// connecting to slave (in thread) makes the lambda also run in the thread
	connect(&m_thread, &QThread::started, m_slave, [this]() {
		m_result = m_func();
		emit m_slave->sDone();
		//m_thread.quit();
	});
    //connect(&m_thread, &QThread::finished, this, [this]() {
    //	this->deleteLater();
    //});
    m_thread.start();
}
bool SimpleWorker::result() const
{
	return m_result;
}
QThread *SimpleWorker::thread() {
    return &m_thread;
}

WorkerObject *SimpleWorker::slave()
{
	return m_slave;
}

WorkerObject::WorkerObject(QObject * parent)
	: QObject(parent)
{
}
