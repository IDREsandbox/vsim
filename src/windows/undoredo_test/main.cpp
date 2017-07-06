#include "undoredo_test.h"
#include <QtWidgets/QApplication>
#include <QList>
#include <QListView>
#include <QStringListModel>
#include <QUndoStack>
#include <string>

#include "VectorModel.hpp"

class MyModel : public QStringListModel {
	virtual Qt::ItemFlags flags(const QModelIndex &index) const {
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	}
};


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	std::vector<Thing*> data;
	data.push_back(new Thing{ 40, std::string( "cat.obj" )});
	data.push_back(new Thing{ 100, "monkey.flt" });
	data.push_back(new Thing{ 40, "cow.osg" });
	data.push_back(new Thing{ 40, "fun.png" });

	Thing *t = new Thing{ 40, "eh" };
	Thing x{ 40, std::string("foo") };

	//Thing y{ 41 };
	//Thing *z = new Thing{ 12 };

	QUndoStack *stack = new QUndoStack();
	
	//QStringList strlist = { "helo", "world", "what", "is", "up" };


	VectorModel *model = new VectorModel(data, stack);

	//MyModel *model = new MyModel;
	//model->setStringList(strlist);

	QListView *view = new QListView;
	view->setModel(model);
	//view->setFlow(QListView::LeftToRight);

	view->show();

	model->poke();

	//view->setDragEnabled(true);
	//view->viewport()->setAcceptDrops(true);
	//view->setDropIndicatorShown(true);
	//view->setDragDropMode(QAbstractItemView::InternalMove);

	//undoredo_test w;
	//w.show();
	return a.exec();
}
