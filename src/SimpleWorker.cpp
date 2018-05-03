#include "SimpleWorker.h"
#include <QDebug>

SimpleWorker::SimpleWorker(QObject *parent)
    : QObject(parent) {
	setFunc([this]() -> bool {
		return true;
	});
}
SimpleWorker::~SimpleWorker() {
    m_thread.exit();
    m_thread.wait();
}

void SimpleWorker::setFunc(std::function<bool()> f) {
    m_func = f;
}

void SimpleWorker::start() {
    // slave moves the lambda into the thread
    m_slave.moveToThread(&m_thread);
    connect(&m_thread, &QThread::started, &m_slave, [this]() {
		m_result = m_func();
		m_thread.quit();
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
