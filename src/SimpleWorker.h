#ifndef SIMPLEWORKER_H
#define SIMPLEWORKER_H

#include <QThread>
#include <QObject>
#include <functional>

class SimpleWorker : public QObject {
public:
	SimpleWorker(QObject *parent = nullptr);
	~SimpleWorker() override;
	void setFunc(std::function<bool()>);
	void start();
	bool result() const;
	QThread *thread();

private:
	QThread m_thread;
	QObject m_slave; // guy sent over to the thread
	std::function<bool()> m_func;
	bool m_result;
};

#endif