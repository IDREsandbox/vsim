#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QFileDialog>
#include <osgDB/ReadFile>
#include <QDebug>

#include "SwitchManager.h"
#include "SwitchWindow.h"
#include "SwitchListModel.h"


osg::ref_ptr<osg::Node> loaded;

class NoisyObs : public osg::Observer {

	void objectDeleted(void *obj) {
		qDebug() << "obs deleted" << obj;
	}
};

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QMainWindow window;

	auto *mb = new QMenuBar(&window);
	window.setMenuBar(mb);
	auto *menu = mb->addMenu("stuff");

	SwitchManager *manager = new SwitchManager(&window);
	if (loaded) {
		manager->addNodeRecursive(loaded);
	}

	auto *a_open = menu->addAction("open");
	QObject::connect(a_open, &QAction::triggered, [&]() {
		QString path = QFileDialog::getOpenFileName(&window);
		auto node = osgDB::readRefNodeFile(path.toStdString());
		qDebug() << "opening" << path;
		if (!node) {
			qDebug() << "failed to load" << path;
			return;
		}
		loaded = node;
		manager->addNodeRecursive(node);
	});

	auto *a_clear = menu->addAction("clear");
	QObject::connect(a_clear, &QAction::triggered, [&]() {
		loaded = nullptr;
	});

	auto *a_clear2 = menu->addAction("clear manager");
	QObject::connect(a_clear2, &QAction::triggered, [&]() {
		manager->clear();
	});

	auto *a_clear3 = menu->addAction("remove rec");
	QObject::connect(a_clear3, &QAction::triggered, [&]() {
		manager->removeNodeRecursive(loaded);
	});


	SwitchWindow *view = new SwitchWindow(&window);
	window.setCentralWidget(view);

	SwitchListModel *model = manager->listModel();

	view->setModel(model);
	view->setWindowFlags(0);

	std::vector<std::pair<std::string, std::vector<std::string>>> init =
	{
		{
			"switch 1",
			{
			"node 1",
			"node 2",
			"node 3"
			}
		},
		{
			"switch 2",
			{
			"node 2 1",
			"node 2 2",
			"node 2 3",
			"node 2 4"
			}
		}
	};

	std::vector<osg::ref_ptr<osg::Switch>> refs;

	for (auto &pair : init) {
		osg::ref_ptr<osg::Switch> sw(new osg::Switch);
		refs.push_back(sw);
		sw->setName(pair.first);
		
		for (std::string &s : pair.second) {
			osg::ref_ptr<osg::Node> node(new osg::Node);
			node->setName(s);
			sw->addChild(node);
		}
		model->addSwitch(sw.get());
	}

	std::vector<std::string> multi_names = {
		"m node 1",
		"m node 2",
		"m node 3",
		"m node 4",
		"m node 5",
	};
	osg::ref_ptr<osgSim::MultiSwitch> multi_switch(new osgSim::MultiSwitch);
	multi_switch->setName("multiswitch");

	for (std::string &s : multi_names) {
		osg::ref_ptr<osg::Node> child(new osg::Node);
		child->setName(s);
		multi_switch->addChild(child);
	}

	std::vector<std::pair<std::string, std::vector<bool>>> multi_init =
	{
		{ "ms set 1", { true, true, false, false, true } },
		{ "ms set 2", { true, true, true, true, true } },
		{ "ms set 3", { false, false, false, false, false } },
		{ "ms set 4", { false, false, true, false, true } }
	};

	// add switches
	osgSim::MultiSwitch::SwitchSetList multi_list;
	for (auto &pair : multi_init) {
		multi_list.push_back(pair.second);
	}
	// set switch names
	multi_switch->setSwitchSetList(multi_list);
	for (int i = 0; i < multi_init.size(); i++) {
		multi_switch->setValueName(i, multi_init[i].first);
	}
	model->addMultiSwitch(multi_switch);

	window.show();

	return a.exec();
}