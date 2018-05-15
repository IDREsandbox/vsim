#include "narrative/CanvasContainer.h"
#include "narrative/CanvasScene.h"
#include "narrative/CanvasToolBar.h"
#include "narrative/CanvasControl.h"

#include <QApplication>
#include <iostream>
#include <QTextDocument>
#include <QDebug>
#include <QMainWindow>
#include <QFile>
#include <QMenuBar>
#include "SimpleCommandStack.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QMainWindow window;
	window.resize(800, 600);

	QMenuBar *mb = new QMenuBar(&window);
	window.setMenuBar(mb);

	//mb->show();
	QMenu *menu = new QMenu("stuff", mb);
	QAction *reload = menu->addAction("Reload style");
	reload->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
	auto reloadStyle = [&window]() {
		QFile File("assets/darkstyle.qss");
		File.open(QFile::ReadOnly);
		QString StyleSheet = QLatin1String(File.readAll());
		window.setStyleSheet(StyleSheet);
	};
	QObject::connect(reload, &QAction::triggered, reloadStyle);

	SimpleCommandStack *stack = new SimpleCommandStack(&window);
	QUndoStack *qstack = stack->stack();
	QAction *undo = qstack->createUndoAction(menu);
	undo->setShortcut(QKeySequence::Undo);
	QAction *redo = qstack->createRedoAction(menu);
	redo->setShortcut(QKeySequence::Redo);

	mb->addMenu(menu);
	menu->addAction(undo);
	menu->addAction(redo);

	window.setMenuBar(mb);

	CanvasToolBar *tb = new CanvasToolBar(&window);

	window.addToolBar(Qt::ToolBarArea::LeftToolBarArea, tb);

	CanvasScene *canvas = new CanvasScene(&window);
	canvas->setBaseHeight(600);
	canvas->setEditable(true);

	CanvasContainer *container = new CanvasContainer(&window);
	// container->setStyleSheet("background:rgba(0, 0, 0, 0);");
	container->setScene(canvas);
	container->setEditable(true);
	container->show();

	window.setCentralWidget(container);

	auto image = std::make_shared<CanvasImage>();
	image->move(.2, -.2);
	image->setPixmap(QPixmap("assets/karnak.jpg"));
	image->setPen(QPen(QBrush(QColor(0, 0, 0)), canvas->toScene(10)));
	image->setEditable(true);
	canvas->addItem(image);

	auto rect3 = std::make_shared<CanvasItem>();
	rect3->setRect(QRectF(-.2, 0, .2, .2));
	rect3->setBrush(QBrush(QColor(0, 0, 255, 255)));
	rect3->setPen(QPen(QBrush(QColor(0, 0, 0)), canvas->toScene(10)));
	rect3->setBorderAround(true);
	rect3->setEditable(true);
	rect3->setPrefersFixedRatio(false);
	canvas->addItem(rect3);

	auto text = std::make_shared<CanvasLabel>();
	text->setVAlign(Qt::AlignCenter);
	text->document()->setPlainText("helloooo\nWORLd");
	text->setRect(QRectF(0, 0, .2, .2));
	text->setBrush(QBrush(QColor(255, 0, 0, 100)));
	text->setEditable(true);
	canvas->addItem(text);

	CanvasControl *cc = new CanvasControl(&window);
	cc->setScene(canvas);
	cc->setStack(stack);

	std::pair<QAbstractButton*, LabelType> label_map[] = {
		{ tb->m_header1, LabelType::HEADER1 },
		{ tb->m_header2, LabelType::HEADER2 },
		{ tb->m_body, LabelType::BODY },
		{ tb->m_label, LabelType::LABEL }
	};
	for (auto &pair : label_map) {
		QObject::connect(pair.first, &QAbstractButton::pressed, [cc, pair]() {
			cc->createLabel(pair.second);
		});
	}
	QObject::connect(tb->m_image, &QAbstractButton::pressed, [cc]() {
		// open up a file dialog
		cc->createImage(QPixmap("assets/karnak.jpg"));
	});
	QObject::connect(tb->m_delete, &QAbstractButton::pressed, [cc]() {
		cc->removeItems();
	});
	
	QObject::connect(tb->m_bold, &QAbstractButton::pressed, cc, &CanvasControl::toggleBold);
	QObject::connect(tb->m_italicize, &QAbstractButton::pressed, cc, &CanvasControl::toggleItalic);
	QObject::connect(tb->m_strikeout, &QAbstractButton::pressed, cc, &CanvasControl::toggleStrikeOut);
	QObject::connect(tb->m_underline, &QAbstractButton::pressed, cc, &CanvasControl::toggleUnderline);

	QObject::connect(tb->m_bullet, &QAbstractButton::pressed, cc, &CanvasControl::listBullet);
	QObject::connect(tb->m_number, &QAbstractButton::pressed, cc, &CanvasControl::listOrdered);

	QObject::connect(tb->m_border, &QAbstractButton::pressed, [cc]() {
		// exec a color dialog
	});
	QObject::connect(tb->m_border_width, &QAbstractSpinBox::editingFinished,
		[tb, cc]() {
		cc->setBorderWidth(tb->m_border_width->value());
	});
	//QObject::connect(tb->m_border_width, qOverload<int>(&QSpinBox::valueChanged),
	//	[cc](int value) {
	//	cc->setBorderWidth(value);
	//});
	QObject::connect(tb->m_background, &QAbstractButton::pressed, [cc]() {
		// exec a color dialog
		// with alpha
	});

	std::pair<QAbstractButton*,Qt::AlignmentFlag> align_map[] = {
		{ tb->m_halign_left, Qt::AlignLeft },
		{ tb->m_halign_center, Qt::AlignCenter },
		{ tb->m_halign_right, Qt::AlignRight },
		{ tb->m_halign_justify, Qt::AlignJustify }
	};
	for (auto pair : align_map) {
		QObject::connect(pair.first, &QAbstractButton::pressed, [cc, pair]() {
			cc->setHAlign(pair.second);
		});
	}

	std::pair<QAbstractButton*, Qt::AlignmentFlag> valign_map[] = {
		{ tb->m_valign_top, Qt::AlignTop },
		{ tb->m_valign_center, Qt::AlignVCenter },
		{ tb->m_valign_bottom, Qt::AlignBottom }
	};
	for (auto pair : valign_map) {
		QObject::connect(pair.first, &QAbstractButton::pressed, [cc, pair]() {
			cc->setVAlign(pair.second);
		});
	}

	reloadStyle();
	window.show();

	return a.exec();
}
