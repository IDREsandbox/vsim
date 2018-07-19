#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QDebug>
#include <QPushButton>
#include <QOpenGLWidget>
//#include <QOpenGLFunctions_2_0>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QGraphicsProxyWidget>

#include "LayerWidget.h"

class OpenGLWidget : public QOpenGLWidget {
public:
	OpenGLWidget(QWidget *parent = nullptr)
		: QOpenGLWidget(parent) {
		QTimer *t = new QTimer(this);
		t->setSingleShot(false);
		t->setInterval(10);
		t->start();
		connect(t, &QTimer::timeout, this, [this]() {
			m_counter++;
			update();
		});
	}

protected:
	bool event(QEvent *e) override {
		//qDebug() << "gl widget event" << e->type();
		return QOpenGLWidget::event(e);
	}
	void initializeGL() override {
		glClearColor(0, 0, 0, 1);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);
	}
	void paintGL() override {
		qDebug() << "paint gl";
		QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
		f->glClearColor(1.0f, (m_counter % 1000) / 1000.0, 1.0f, 1.0f);
		f->glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_TRIANGLES);
		glColor3f(0.0, 0.0, (m_counter % 9) / 8.0);
		glVertex3f(-0.5, -0.5, 0);
		glVertex3f(0.5, -0.5, 0);
		glVertex3f(0.0, 0.5, 0);
		glEnd();
	}

private:
	int m_counter = 0;
};

class NoisyWidget : public QWidget {
public:

	NoisyWidget(QWidget *parent = nullptr)
		: QWidget(parent),
		m_paint(true)
	{

	}

	void setName(QString name) {
		m_name = name;
	}
	void setBrush(QBrush brush) {
		m_brush = brush;
	}
	void paintEvent(QPaintEvent *e) override {
		qDebug() << "paint" << m_name;
		if (!m_paint) return;
		QPainter painter(this);
		painter.setBrush(m_brush);
		painter.drawRect(0, 0, width(), height());
	}
	void setPaint(bool paint) {
		m_paint = paint;
	}

public:
	QString m_name;
	QBrush m_brush;
	bool m_paint;
};


class ColorfulWidget : public QFrame {
public:

	ColorfulWidget(QFrame *parent = nullptr)
		: QFrame(parent)
	{
		QTimer *t = new QTimer(this);
		t->setSingleShot(false);
		t->setInterval(1000);
		t->start();
		connect(t, &QTimer::timeout, this, [this]() {
			m_counter++;
			update();
		});
	}

protected:
	void paintEvent(QPaintEvent *e) override {
		QBrush brush(QColor((255 / 4) * (m_counter % 4), (255 / 7) * (m_counter % 7), 0, 150));

		QPainter painter(this);
		painter.setBrush(brush);
		painter.drawRect(0, 0, width(), height());
	}

public:
	int m_counter = 0;
};

QAction *addPoker(QMenu *menu, QString name, QWidget *widget, QKeySequence key) {

	QAction *action;
	action = menu->addAction(name);
	action->setShortcut(QKeySequence(key));
	QObject::connect(action, &QAction::triggered, widget, [widget, name]() {
		qDebug() << "request" << name;
		widget->update(0, 0, 100, 100);
		widget->repaint();
	});
	return action;
}

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	QMainWindow window(nullptr);

	QWidget *root = new QWidget();
	window.setCentralWidget(root);
	QGridLayout *layout = new QGridLayout(root);


	auto *mb = new QMenuBar(&window);
	window.setMenuBar(mb);

	auto *menu = mb->addMenu("stuff");

	window.show();
	window.resize(400, 400);

	addPoker(menu, "window", &window, Qt::Key_1);

	NoisyWidget *bg = new NoisyWidget();
	//bg->setBrush(QBrush(Qt::GlobalColor::red, Qt::BrushStyle::Dense2Pattern));
	bg->setBrush(QBrush(Qt::GlobalColor::red, Qt::BrushStyle::Dense2Pattern));
	bg->setName("bg");
	addPoker(menu, "bg", bg, Qt::Key_2);

	OpenGLWidget *gl = new OpenGLWidget();
	gl->setGeometry(50, 50, 120, 310);
	addPoker(menu, "gl", gl, Qt::Key_R);
	layout->addWidget(gl, 0, 0);

	NoisyWidget *layer1 = new NoisyWidget();
	layer1->setBrush(QBrush(Qt::black, Qt::BrushStyle::CrossPattern));
	layer1->setName("layer1");
	addPoker(menu, "layer1", layer1, Qt::Key_3);

	QPushButton *pb = new QPushButton(layer1);
	pb->setGeometry(200, 50, 100, 50);
	pb->setText("button");
	pb->show();

	NoisyWidget *layer2 = new NoisyWidget();
	//layer2->setBrush(QBrush(QColor(0, 0, 150, 0), Qt::BrushStyle::Dense1Pattern));
	layer2->setBrush(QBrush(Qt::transparent));
	layer2->setName("layer2");
	layer2->resize(100, 100);
	layer2->setPaint(false);
	//layer2->setStyleSheet("background: rgba(0, 0, 255, 100);");
	addPoker(menu, "layer2", layer2, Qt::Key_4);

	ColorfulWidget *layer3 = new ColorfulWidget();
	layer3->setGeometry(100, 100, 200, 200);

	QPushButton *pb2 = new QPushButton(layer2);
	pb2->setGeometry(10, 10, 60, 30);
	pb2->setText("button2");
	pb2->show();


	LayerWidget *lw = new LayerWidget(&window);
	//window.setCentralWidget(lw);
	layout->addWidget(lw, 0, 0);
	addPoker(menu, "container", lw, Qt::Key_C);

	lw->hide();

	//lw->view()->setStyleSheet("background: transparent;");

	//auto *scene = lw->scene();

	// it just doesn't work q.q
	//QGraphicsRectItem *item = new QGraphicsRectItem();
	//item->setRect(0, 0, 400, 400);
	//item->setCacheMode(QGraphicsItem::CacheMode::DeviceCoordinateCache);
	//QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(item);
	//proxy->setWidget(gl);
	//scene->addItem(item);

	//addPoker(menu, "view", lw->view(), Qt::Key_V);

	//auto *view_cache = menu->addAction("view cache");
	//QObject::connect(view_cache, &QAction::triggered, [lw]() {
	//	lw->view()->resetCachedContent();
	//});
	//lw->view()->indire

	//auto *gl_layer = lw->addLayer(layer0);
	//gl_layer->setCacheMode(QGraphicsItem::NoCache);
	//gl_layer->setZValue(4.0);
	//lw->setBase(bg);
	//lw->addLayer(layer0);
	//lw->setBase(layer0);

	lw->addLayer(layer3);
	lw->addLayer(layer1);
	lw->addLayer(layer2);


	NoisyWidget *plain = new NoisyWidget(&window);
	plain->setBrush(QBrush(QColor(0, 255, 255, 150)));
	plain->setName("plain");
	plain->setGeometry(200, 200, 200, 200);
	plain->show();


	return a.exec();
}