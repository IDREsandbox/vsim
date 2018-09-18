#ifndef FUTUREDIALOG_H
#define FUTUREDIALOG_H

#include <QMainWindow>
#include <QtConcurrent>

#include "ui_FutureDialog.h"

class SimpleWorker;

class FutureDialog : public QDialog
{
	Q_OBJECT
public:
	FutureDialog(QWidget *parent = nullptr);

	// connect to a watcher
	// dialog closes on finish
	void setWatcher(QFutureWatcherBase *watcher);

	template <class T>
	void spin(const QFuture<T> &future) {
		setFuture(future);
		exec();
	}

	// creates the watcher and everything
	template <class T>
	void setFuture(const QFuture<T> &future) {
		auto *watcher = new QFutureWatcher<T>(this);
		setWatcher(watcher);
		watcher->setFuture(future);
	}

	void watchThread(QThread *thread);

	void watchWorker(SimpleWorker *worker);

	void setText(const QString &text);

	bool wasAccepted() const;
	bool canceled() const;
	void setCancellable(bool cancel);

protected:
	void closeEvent(QCloseEvent *event) override;

private:
	void onFinish();

private:
	Ui::FutureDialog ui;
	QFutureWatcherBase *m_watcher;
	bool m_will_accept;
	QString m_text;
	bool m_waiting;
};

#endif