#define FLATBUFFERS_DEBUG_VERIFICATION_FAILURE
#include "Canvas/CanvasScene.h"
#include "Canvas/CanvasEditor.h"

#include <QApplication>
#include <iostream>
#include <QTextDocument>
#include <QDebug>
#include <QMainWindow>
#include <QFile>
#include <QMenuBar>
#include <QFileDialog>
#include <sstream>
#include <fstream>
#include "Core/SimpleCommandStack.h"
#include "Canvas/CanvasSerializer.h"


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
		QFile File(QCoreApplication::applicationDirPath()
			+ "/assets/darkstyle.qss");
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
	QAction *debug = new QAction(&window);
	debug->setText("debug");
	debug->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
	QAction *apply_buttons = new QAction(&window);
	apply_buttons->setText("restyle buttons");
	apply_buttons->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));

	QAction *save = new QAction(&window);
	save->setText("save");
	save->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
	QAction *load = new QAction(&window);
	load->setText("open");
	load->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
	QAction *clear = new QAction(&window);
	clear->setText("clear");
	clear->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));

	mb->addMenu(menu);
	menu->addAction(undo);
	menu->addAction(redo);
	menu->addAction(debug);
	menu->addAction(apply_buttons);
	menu->addAction(save);
	menu->addAction(load);
	menu->addAction(clear);

	window.setMenuBar(mb);

	CanvasScene *scene = new CanvasScene(&window);
	scene->setBaseHeight(600);
	scene->setEditable(true);

	auto image = std::make_shared<CanvasImage>();
	image->move(.2, -.2);
	image->setPixmap(QPixmap("assets/karnak.jpg"));
	image->setPen(QPen(QBrush(QColor(0, 0, 0)), scene->toScene(10)));
	image->setEditable(true);
	scene->addItem(image);

	auto rect3 = std::make_shared<CanvasItem>();
	rect3->setRect(QRectF(-.2, 0, .2, .2));
	rect3->setBrush(QBrush(QColor(0, 0, 255, 255)));
	rect3->setPen(QPen(QBrush(QColor(0, 0, 0)), scene->toScene(10)));
	rect3->setBorderAround(true);
	rect3->setEditable(true);
	rect3->setPrefersFixedRatio(false);
	scene->addItem(rect3);

	auto text = std::make_shared<CanvasLabel>();
	text->setVAlign(Qt::AlignCenter);
	text->document()->setPlainText("helloooo\nWORLd");
	text->setRect(QRectF(0, 0, .2, .2));
	text->setBrush(QBrush(QColor(255, 0, 0, 100)));
	text->setEditable(true);
	scene->addItem(text);

	CanvasEditor *editor = new CanvasEditor(&window);
	window.setCentralWidget(editor);
	editor->setScene(scene);
	editor->setStack(stack);

	reloadStyle();
	window.show();

	//editor->showToolBar(false);

	QObject::connect(debug, &QAction::triggered, [editor]() {
		editor->debug();
	});
	QObject::connect(apply_buttons, &QAction::triggered, [editor]() {
		editor->applyStylesToButtons();
	});

	namespace fb = VSim::FlatBuffers;
	QObject::connect(save, &QAction::triggered, [&]() {
		QString path = QFileDialog::getSaveFileName(&window, "Save As");

		std::ofstream out(path.toStdString(), std::ios::binary);
		if (!out.good()) {
			return false;
		}

		flatbuffers::FlatBufferBuilder builder;
		auto o_canvas = CanvasSerializer::createCanvas(&builder, scene);
		fb::FinishCanvasBuffer(builder, o_canvas);

		const char *buf = reinterpret_cast<const char*>(builder.GetBufferPointer());
		out.write(buf, builder.GetSize());

		return out.good();

	});
	QObject::connect(load, &QAction::triggered, [&]() {
		QString path = QFileDialog::getOpenFileName(&window, "Open");


		std::ifstream in(path.toStdString(), std::ios::binary);

		if (!in.good()) {
			qDebug() << "file open error";
			return;
		}

		// read into buffer
		std::stringstream ss;
		ss << in.rdbuf();
		std::string s = ss.str();
		const uint8_t *buf = reinterpret_cast<const uint8_t*>(s.c_str());

		bool fb_ok = fb::VerifyCanvasBuffer(flatbuffers::Verifier(buf, s.length()));
		if (!fb_ok) {
			qDebug() << "fb verify" << fb_ok;
			//return;
		}

		qstack->clear();

		CanvasScene scene_buf;

		auto *canvas_buf = fb::GetCanvas(buf);
		CanvasSerializer::readCanvas(canvas_buf, &scene_buf);

		scene->copy(scene_buf);
	});

	QObject::connect(clear, &QAction::triggered, [&]() {
		scene->clear();
		qstack->clear();
	});

	return a.exec();
}
