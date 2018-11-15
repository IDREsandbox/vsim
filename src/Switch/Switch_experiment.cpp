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

void readWriteTest() {
	osg::ref_ptr<osgSim::MultiSwitch> ms = new osgSim::MultiSwitch;


	osg::ref_ptr<osg::Node> node1 = new osg::Node;
	osg::ref_ptr<osg::Node> node2 = new osg::Node;
	osg::ref_ptr<osg::Node> node3 = new osg::Node;

	node1->setName("n1");
	node2->setName("n2");
	node3->setName("n3");

	// set0 - 0 0 0
	// set1 - 1 1 1
	// set2 = 1 0 1

	ms->addChild(node1);
	ms->addChild(node2);
	ms->addChild(node3);

	//osgSim::MultiSwitch::SwitchSetList; // vector<vector<bool>>
	//osgSim::MultiSwitch::ValueList; // vector<bool>
	//osgSim::MultiSwitch::SwitchSetNameList; // vector<string>
	std::vector<std::vector<bool>> sets = {
		{ false, false, false },
		{ true, true, true },
		{ true, false, true }
	};
	std::vector<std::string> names = {
		"set0",
		"set1",
		"set2"
	};

	ms->setSwitchSetList(sets);
	for (int i = 0; i < names.size(); i++) {
		ms->setValueName(i, names[i]);
	}

	// possible hack solutions
	// 1. Use Descriptions, it isn't documented so I have no idea what it's for
	//	but we can store a list of strings in it. It's probably for describing
	//  each user object? ex. time_data, other_data, etc_data
	// 2. Use a user data map/stack of strings?
	// 3. Use a tree of user data containers
	// 4. Encode names as a single string "name1;name2;name3"

	// (2) ValueStack is so confusing, its a
	//		Stack of Maps of ValueObjects pointing to ValueObjects
	// (3) obj->container()["switch_names"]->dummycontainer()->descriptions()
	//		we're going to use the Descriptions section of a dummy container
	//		to store a string list

	SwitchListModel::saveCustomSwitchNames(ms);

	// write out to buffer
	std::stringstream ss;
	auto *osgrw = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");
	osgrw->writeNode(*ms, ss);
	auto read_result = osgrw->readNode(ss);
	if (!read_result.success()) {
		std::cout << "read error?\n";
	}
	auto *ms2 = dynamic_cast<osgSim::MultiSwitch*>(read_result.takeNode());

	if (!ms2) {
		qDebug() << "not a multiswitch fail";
		return;
	}
	SwitchListModel::loadCustomSwitchNames(ms2);

	std::vector<std::string> after = SwitchListModel::getSwitchNames(ms2);
	qDebug() << "after:";
	for (std::string s : after) {
		qDebug() << QString::fromStdString(s);
	}
}

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

	auto *a_rwtest = menu->addAction("rw test");
	QObject::connect(a_rwtest, &QAction::triggered, [&]() {
		readWriteTest();
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