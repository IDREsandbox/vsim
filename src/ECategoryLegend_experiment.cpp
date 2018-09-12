#include <QApplication>
#include <QWidget>
#include <QResizeEvent>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>

#include "ECategoryLegend.h"
#include "resources/ECategoryGroup.h"
#include "resources/ECategory.h"

class Window : public QWidget {
public:
	Window() : QWidget(nullptr) {
	}
	void setLegend(ECategoryLegend *leg) {
		m_legend = leg;
	}

protected:
	void resizeEvent(QResizeEvent *e) override {
		if (!m_legend) return;
		qDebug() << "resizing" << e;
		m_legend->setAnchor(rect().bottomRight() - QPoint(10, 10));
	}
private:
	ECategoryLegend *m_legend = nullptr;
};

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	Window window;
	window.resize(400, 400);
	ECategoryGroup group;
	ECategoryLegend *legend = new ECategoryLegend(&window);
	window.setLegend(legend);

	auto *full_layout = new QVBoxLayout(&window);
	//QVBoxLayout *button_layout = new QVBoxLayout;
	//full_layout->addLayout(button_layout);

	auto *add = new QPushButton(&window);
	add->setText("add");
	full_layout->addWidget(add, 0, Qt::AlignLeft | Qt::AlignTop);
	QObject::connect(add, &QPushButton::pressed, [&]() {
		qDebug() << "adding";
		auto cat = std::make_shared<ECategory>();
		cat->setCategoryName("wwwww ww");
		group.append(cat);
	});

	auto *remove = new QPushButton(&window);
	remove->setText("remove");
	full_layout->addWidget(remove, 0, Qt::AlignLeft | Qt::AlignTop);
	QObject::connect(remove, &QPushButton::pressed, [&]() {
		group.remove(group.size() - 1, 1);
	});

	auto *edit = new QPushButton(&window);
	edit->setText("edit");
	full_layout->addWidget(edit, 0, Qt::AlignLeft | Qt::AlignTop);
	QObject::connect(edit, &QPushButton::pressed, [&]() {
		auto *cat = group.child(1);
		if (!cat) return;
		cat->setCategoryName("lmao change name");
		cat->setColor(QColor(255, 0, 255));
	});

	full_layout->addStretch(1);

	std::vector<std::pair<const char *, QColor>> map = {
		{ "first category", QColor(255, 0, 0) },
		{ "really really long category name lmao, like really the longest name ever", QColor(0,255,0) },
		{ "third cat", QColor(0,0,255) },
	};

	for (auto &p : map) {
		auto cat = std::make_shared<ECategory>();
		cat->setCategoryName(p.first);
		cat->setColor(p.second);
		group.append(cat);
	}

	legend->setCategoryGroup(&group);
	legend->show();

	window.show();
	return a.exec();
}