#include "outliner_experiment.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include "ModelOutliner.h"
#include "TreeModel.h"
#include <QStringList>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Outliner_experiment w;

	QFile file("./data.txt");
	file.open(QIODevice::ReadOnly);
	TreeModel model({"one", "two", "three", "four", "five"}, file.readAll(), &w);
	file.close();

	ModelOutliner *outliner = new ModelOutliner(&w);

	//auto *outliner = new QTreeView(&w);
	//outliner->setModel(&model);
	//outliner->setWindowFlag(Qt::Window);

	outliner->setModel(&model);
	outliner->setGeometry(100, 100, 600, 700);
	outliner->setSelectionMode(QAbstractItemView::ExtendedSelection);
	outliner->setSelectionBehavior(QAbstractItemView::SelectItems);
	outliner->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	outliner->expandAll();
	
	outliner->show();
	w.show();
	
	return a.exec();
}
