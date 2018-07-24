#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>

#include "SwitchManager.h"
#include "SwitchWindow.h"
#include "SwitchModel.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QMainWindow window;
	auto *mb = new QMenuBar(&window);
	window.setMenuBar(mb);
	auto *menu = mb->addMenu("stuff");

	auto *a_open = menu->addAction("open");
	QObject::connect(a_open, &QAction::triggered, []() {

	});


	SwitchWindow *view = new SwitchWindow(&window);
	window.setCentralWidget(view);

	SwitchListModel *model = new SwitchListModel(&window);

	view->setModel(model);

	
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

	window.show();

	return a.exec();
}