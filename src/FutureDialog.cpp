#include "FutureDialog.h"

#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QtConcurrent>
#include <QDialog>
#include <QPushButton>
#include <QCloseEvent>

FutureDialog::FutureDialog(QWidget *parent)
	: QDialog(parent),
	m_will_accept(true),
	m_waiting(false)
{
	ui.setupUi(this);
	setModal(true);
	setText("loading");

	setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	//setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	connect(ui.buttonBox, &QDialogButtonBox::rejected, this, [this]() {
		m_will_accept = false;
		setText(m_text);
	});
}

void FutureDialog::setWatcher(QFutureWatcherBase * watcher)
{
	m_waiting = true;
	m_watcher = watcher;
	connect(watcher, &QFutureWatcherBase::finished, this, &FutureDialog::onFinish);
}

void FutureDialog::watchThread(QThread * thread)
{
	m_waiting = true;
	connect(thread, &QThread::finished, this, &FutureDialog::onFinish);
}

void FutureDialog::setText(const QString &text)
{
	m_text = text;
	QString out;
	if (m_will_accept) {
		out = text;
	}
	else {
		out = text + " - cancelling";
	}
	ui.label->setText(out);
}

bool FutureDialog::wasAccepted() const
{
	return result() == QDialog::Accepted;
}

bool FutureDialog::canceled() const
{
	return result() == QDialog::Rejected;
}

void FutureDialog::setCancellable(bool cancel)
{
	if (cancel) {
		ui.buttonBox->setStandardButtons(QDialogButtonBox::Cancel);
	}
	else {
		ui.buttonBox->setStandardButtons({});
	}
}

void FutureDialog::closeEvent(QCloseEvent * event)
{
	if (m_waiting) {
		event->accept();
		return;
	}
	QDialog::closeEvent(event);
}

void FutureDialog::onFinish()
{
	done(m_will_accept ? QDialog::Accepted : QDialog::Rejected);
	m_waiting = false;
}


