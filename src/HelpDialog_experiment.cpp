#include "HelpDialog.h"
#include "ScrollBoxItem.h"
#include "Core/VSimInfo.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <iostream>
#include <QTimer>
#include <QPushButton>
#include <QAction>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//VSimInfo::initPath(argv[0]);

	qDebug() << "default assets" << VSimInfo::assets();

	QWidget window(nullptr);
	window.resize(200, 200);

	QAction *reload = new QAction(&window);
	window.addAction(reload);
	reload->setShortcut(Qt::CTRL + Qt::Key_R);
	reload->setShortcutContext(Qt::ShortcutContext::ApplicationShortcut);

	QPushButton *pb = new QPushButton(&window);
	pb->setShortcut(Qt::CTRL + Qt::Key_R);
	pb->setText("reload");

	HelpDialog dlg(&window);
	dlg.show();
	dlg.move(window.pos() + QPoint(100,100));

	QObject::connect(pb, &QPushButton::pressed, reload, &QAction::trigger);
	QObject::connect(reload, &QAction::triggered, [&]() {
		qDebug() << "reload trigger";
		dlg.reload();
	});

	window.show();

	return a.exec();
}
