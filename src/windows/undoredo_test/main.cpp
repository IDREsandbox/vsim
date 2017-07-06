#include "undoredo_test.h"
#include <QtWidgets/QApplication>
#include <QList>
#include <QListView>
#include <QStringListModel>
#include <QUndoStack>
#include <string>
#include <QTreeView>
#include <QTableView>

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
	data.push_back(new Thing(40, std::string("cat.obj")));
	data.push_back(new Thing(100, "monkey.flt"));
	data.push_back(new Thing(40, "cow.osg" ));
	data.push_back(new Thing(40, "fun.png" ));

	data[0]->children.push_back(new Other(data[0], 4, "test", "hey"));
	data[0]->children.push_back(new Other(data[0], 5, "test2", "haz"));
	data[0]->children.push_back(new Other(data[0], 6, "test3", "bo"));
	//
	data[1]->children.push_back(new Other(data[1], 4, "swaaa", "brain"));
	data[1]->children.push_back(new Other(data[1], 5, "dog", "hair"));
	data[1]->children.push_back(new Other(data[1], 6, "monkey", "tail"));



	QUndoStack *stack = new QUndoStack();
	VectorModel *model = new VectorModel(data, stack);

	QTreeView *view = new QTreeView;
	view->setModel(model);
	//view->setFlow(QListView::LeftToRight);

	view->show();

	QTableView *tableview = new QTableView;
	tableview->setModel(model);
	tableview->setRootIndex(model->index(1,0));
	tableview->show();

	//model->poke();


	//Thing *t = new Thing{ 40, "eh" };
	//Thing x{ 40, std::string("foo") };
	//Thing y{ 41 };
	//Thing *z = new Thing{ 12 };
	//QStringList strlist = { "helo", "world", "what", "is", "up" };
	//MyModel *model = new MyModel;
	//model->setStringList(strlist);

	view->setDragEnabled(true);
	view->viewport()->setAcceptDrops(true);
	view->setDropIndicatorShown(true);
	view->setDragDropMode(QAbstractItemView::InternalMove);

	//undoredo_test w;
	//w.show();
	return a.exec();
}
