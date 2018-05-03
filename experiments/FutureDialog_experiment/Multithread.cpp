 #include "Multithread.h"
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QtConcurrent>
#include <QDialog>
#include <chrono>
#include <thread>
#include "FutureDialog.h"
size_t hardThing(size_t iters) {
	float counter = 1.0f;
	for (size_t i = 0; i < iters; i++) {
		counter += iters / 1.397f;
	}
	qDebug() << "done" << counter;
	return counter;
}

int sleepFor(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	return ms;
}

size_t COUNT = 10100100100;

class DoThread : public QThread {
public:
	DoThread(QObject *parent = nullptr)
		: QThread(parent) {
	}
	float value() {
		return x;
	}
protected:
	void run() override
	{
		x = hardThing(COUNT);
	}
private:
	float x;
};

class Worker : public QObject {
public:
	Worker(QObject *parent = nullptr)
		: QObject(parent)
	{
	}
	~Worker() {
		qDebug() << "waiting for thread on worker" << this;
		m_thread.exit();
		m_thread.wait();
		qDebug() << "del" << this;
	}
	void start() {
		m_thread.start();
	}
	DoThread m_thread;
};

class CCWorker : public QObject {
public:
	CCWorker(bool auto_delete = true, QObject *parent = nullptr)
		: QObject(parent)
	{
		watcher = new QFutureWatcher<size_t>(this);

		connect(watcher, &QFutureWatcherBase::finished, this, [this]() {
			qDebug() << "hey";
			deleteLater();
		});
	}
	~CCWorker() {
		qDebug() << "future destroy";
		//future.cancel();
		//future.waitForFinished();
		qDebug() << "future destroy - done";
	}
	void start() {
		future = QtConcurrent::run(hardThing, COUNT);
	}

	QFuture<size_t> future; // = QtConcurrent::run(hardThing, COUNT);
	QFutureWatcher<size_t> *watcher;
};

Multithread::Multithread(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.do1, &QAbstractButton::pressed, this, [this]() {
		hardThing(COUNT);
	});

	connect(ui.do2, &QAbstractButton::pressed, this, [this]() {
		Worker *w = new Worker(this);
		w->start();
		qDebug() << "after start";

		connect(&w->m_thread, &QThread::finished, this, [this, w]() {
			w->deleteLater();
			qDebug() << "doneee" << w->m_thread.value();
		});
	});

	connect(ui.do3, &QAbstractButton::pressed, this, [this]() {
		CCWorker *w = new CCWorker(this);
		w->start();
	});

	connect(ui.do4, &QAbstractButton::pressed, this, [this]() {
		qDebug() << "running 4";
		auto *watcher = new QFutureWatcher<size_t>(this);
		{
			QFuture<size_t> f = QtConcurrent::run(&hardThing, COUNT);
			watcher->setFuture(f);
			connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
				qDebug() << "watcher finished 4" << watcher->result();
				watcher->deleteLater();
			});
		}
		qDebug() << "after qfuture destructor";
		//qDebug() << "watcher thing" << watcher->future();
	});

	connect(ui.do5, &QAbstractButton::pressed, this, [this]() {
		qDebug() << "run 5";
		QFuture<int> f = QtConcurrent::run(&sleepFor, 1000);
		FutureDialog dlg;
		dlg.setFuture(f);
		dlg.exec();
		qDebug() << "dialog result" << dlg.result();
	});

	QTimer *t = new QTimer(this);
	t->setInterval(0);
	t->start();
	connect(t, &QTimer::timeout, this, [this]() {
		ui.dial->setValue((ui.dial->value() + 1) % ui.dial->maximum());
	});
}
