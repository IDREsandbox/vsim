#include <QTest>
#include <QDebug>
#include <osg/Node>
#include "OSGObserver.h"

// a sample observer which counts number of deletions
class MyObserver : public OSGObserver {
public:
	MyObserver(int *out) : m_out(out) {}
	void objectDeleted(void *obj) override {
		(*m_out) += 1;
	}
	int *m_out;
};

class OSGObserver_test : public QObject {
	Q_OBJECT
private slots:

	void observerDeleteTest() {
		{
			osg::ref_ptr<osg::Node> node(new osg::Node);
			{
				auto observer = std::make_unique<OSGObserver>();
				observer->observe(node);
				qDebug() << "about to delete observer";
			}
			qDebug() << "about to delete node";
		}
		qDebug() << "survived";
	}

	void referencedDeleteTest() {
		{
			auto observer = std::make_unique<OSGObserver>();
			{
				osg::ref_ptr<osg::Node> node(new osg::Node);
				observer->observe(node);
				qDebug() << "about to delete observer";
			}
			qDebug() << "about to delete node";
		}
		qDebug() << "survived";
	}

	void overrideTest() {
		int count = 0;
		auto observer = std::make_unique<MyObserver>(&count);
		{
			osg::ref_ptr<osg::Node> node(new osg::Node);
			observer->observe(node);
			osg::ref_ptr<osg::Node> node2(new osg::Node);
			observer->observe(node2);
		}
		QCOMPARE(count, 2);
	}

	// this crashes, when an osg::Observer is destroyed first
	//void crashTest() {
	//	{
	//		osg::ref_ptr<osg::Node> node(new osg::Node);
	//		{
	//			std::unique_ptr<osg::Observer> temporary
	//				= std::make_unique<osg::Observer>();
	//			node->addObserver(temporary.get());
	//			qDebug() << "about to delete observer";
	//		}
	//		qDebug() << "about to delete node";
	//	}
	//	qDebug() << "crashes here?";
	//}

};

QTEST_MAIN(OSGObserver_test)
#include "OSGObserver_test.moc"