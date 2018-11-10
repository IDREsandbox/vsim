#include <QApplication>
#include <QWidget>
#include <QResizeEvent>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>

#include <QTextEdit>
#include <QTextBrowser>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QMainWindow>
#include <QAction>
#include <QMenuBar>
#include <QDebug>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QMainWindow window;
	window.resize(400, 400);

	QAction *a_reload = window.menuBar()->addAction("reload style");
	a_reload->setShortcut(QKeySequence(Qt::Key_F1));

	QWidget *root = new QWidget(&window);
	window.setCentralWidget(root);
	QVBoxLayout *layout = new QVBoxLayout(window.centralWidget());

	QScrollArea *harea = new QScrollArea(root);
	QFrame *hspacer = new QFrame(harea);
	hspacer->setGeometry(0, 0, 3000, 100);
	harea->setWidget(hspacer);
	layout->addWidget(harea);

	QTextEdit *varea = new QTextEdit(root);
	QString text;
	for (int i = 0; i < 300; i++) {
		text += "a line\n";
	}
	varea->setText(text);
	layout->addWidget(varea);

	QTextBrowser *varea2 = new QTextBrowser(root);
	varea2->setText(text);
	layout->addWidget(varea2);

	auto reloadStyle = [&]() {
		QFile File("assets/scrollbar.qss");
		File.open(QFile::ReadOnly);
		QString style = QLatin1String(File.readAll());
		window.setStyleSheet(style);

		//harea->setStyleSheet();
	};

	QObject::connect(a_reload, &QAction::triggered, [&](){
		reloadStyle();
	});

	reloadStyle();
	window.show();
	return a.exec();
}