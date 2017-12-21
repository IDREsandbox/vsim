#include <QTest>
#include <QDebug>
#include <QSignalSpy>
#include "resources/ERFilterSortProxy.h"
#include "resources/EResourceGroup.h"
#include "resources/EResource.h"
class ERFilterSortProxy_test : public QObject {
	Q_OBJECT
private:
	osg::ref_ptr<EResourceGroup> group;
	osg::ref_ptr<ERFilterSortProxy> proxy;
	osg::ref_ptr<EResource> res0;
	osg::ref_ptr<EResource> res1;
	osg::ref_ptr<EResource> res2;
	osg::ref_ptr<EResource> res3;
	osg::ref_ptr<EResource> res4;
	void reset() {
		group = new EResourceGroup;
		res0 = new EResource;
		res1 = new EResource;
		res2 = new EResource;
		res3 = new EResource;
		res4 = new EResource;
		res0->setResourceName("foo");
		res1->setResourceName("bar");
		res2->setResourceName("Bab");
		res3->setResourceName("Alphabet");
		res4->setResourceName("Joe");
		res0->setGlobal(false);
		res1->setGlobal(false);
		res2->setGlobal(true);
		res3->setGlobal(true);
		res4->setGlobal(false);
	}
	void GROUPCOMPARE(Group *group, std::vector<osg::Node*> target) {
		QCOMPARE(group->getNumChildren(), target.size());
		for (unsigned int i = 0; i < group->getNumChildren(); i++) {
			if (i >= target.size()) return;
			QCOMPARE(group->child(i), target[i]);
		}
	}
private slots:

	void sortByTest() {
		reset();
		group->addChild(res0);
		group->addChild(res1);
		group->addChild(res2);
		group->addChild(res3);
		proxy = new ERFilterSortProxy(group);
		proxy->sortBy(ERFilterSortProxy::ALPHABETICAL);
		proxy->filterGlobal(ERFilterSortProxy::SHOW_BOTH);
		proxy->debug();

		GROUPCOMPARE(proxy, {res3, res2, res1, res0});
		group->addChild(res4);
		GROUPCOMPARE(proxy, { res3, res2, res4, res1, res0 });

	}
	void insertSignalTests() {
		reset();
		group->addChild(res0);
		group->addChild(res1);
		group->addChild(res2);
		group->addChild(res3);
		proxy = new ERFilterSortProxy(group);
		proxy->sortBy(ERFilterSortProxy::ALPHABETICAL);
		proxy->filterGlobal(ERFilterSortProxy::SHOW_BOTH);
		// {Alphabet, Bab, bar, foo}
		// add Joe, should be index 2
		QSignalSpy newSpy(proxy, &Group::sNew);
		group->addChild(res4);
		qDebug() << "nothng?";
		proxy->debug();
		// sNew(2)
		QCOMPARE(newSpy.size(), 1);
		QCOMPARE(newSpy.takeFirst().at(0).toInt(), 2);
	}
	void changeGlobalTest() {
		reset();

		proxy = new ERFilterSortProxy(group);
		proxy->filterGlobal(ERFilterSortProxy::SHOW_GLOBAL);
		QSignalSpy newSpy(proxy, &Group::sNew);
		QSignalSpy deleteSpy(proxy, &Group::sDelete);

		// add irrelevant thing
		// global/local, no signals expected
		EResource *res = new EResource;
		res->setGlobal(false);
		group->addChild(res);
		QCOMPARE(newSpy.size(), 0);

		// change item to fit
		// global/global sNew(0)
		res->setGlobal(true); // this change should propagate
		QCOMPARE(newSpy.size(), 1);
		QCOMPARE(newSpy.takeFirst().at(0).toInt(), 0);
		newSpy.clear();

		// change item to break
		// global/local sDelete(0)
		res->setGlobal(false);
		QCOMPARE(deleteSpy.size(), 1);
		deleteSpy.clear();

		// change filter to fit
		// both/local sNew(0)
		proxy->filterGlobal(ERFilterSortProxy::SHOW_BOTH);
		QCOMPARE(newSpy.size(), 1);

		// change filter to break
		// global/local sDelete(0)
		proxy->filterGlobal(ERFilterSortProxy::SHOW_LOCAL);
		QCOMPARE(deleteSpy.size(), 1);
		deleteSpy.clear();
	}

	void globalTest() {
		osg::ref_ptr<Group> group = new EResourceGroup;
		osg::ref_ptr<EResource> foo = new EResource;
		foo->setResourceName("foo");
		foo->setGlobal(true);
		osg::ref_ptr<EResource> bar = new EResource;
		bar->setResourceName("bar");
		bar->setGlobal(true);
		osg::ref_ptr<EResource> cat = new EResource;
		cat->setResourceName("cat");
		cat->setGlobal(false);

		group->addChild(foo);
		group->addChild(bar);
		group->addChild(cat);

		osg::ref_ptr<ERFilterSortProxy> proxy = new ERFilterSortProxy(group);

		proxy->filterGlobal(ERFilterSortProxy::SHOW_LOCAL);
		proxy->sortBy(ERFilterSortProxy::NONE);
		GROUPCOMPARE(proxy, { cat });

		proxy->filterGlobal(ERFilterSortProxy::SHOW_GLOBAL);
		GROUPCOMPARE(proxy, { foo, bar });

		proxy->filterGlobal(ERFilterSortProxy::SHOW_BOTH);
		GROUPCOMPARE(proxy, { foo, bar, cat });
	}
	void chainedProxy() {
		osg::ref_ptr<Group> group = new EResourceGroup;
		osg::ref_ptr<EResource> foo = new EResource;
		foo->setResourceName("foo");
		foo->setGlobal(false);
		osg::ref_ptr<EResource> bar = new EResource;
		bar->setResourceName("bar");
		bar->setGlobal(false);
		osg::ref_ptr<EResource> cat = new EResource;
		cat->setResourceName("cat");
		cat->setGlobal(true);

		osg::ref_ptr<ERFilterSortProxy> p1 = new ERFilterSortProxy(group);
		p1->filterGlobal(ERFilterSortProxy::SHOW_LOCAL);
		p1->sortBy(ERFilterSortProxy::NONE);

		osg::ref_ptr<ERFilterSortProxy> p2 = new ERFilterSortProxy(p1);
		p2->filterGlobal(ERFilterSortProxy::SHOW_BOTH);
		p2->sortBy(ERFilterSortProxy::ALPHABETICAL);

		group->addChild(foo);
		group->addChild(bar);
		group->addChild(cat);

		qDebug() << "proxy 1 -------";
		p1->debug();
		qDebug() << "proxy 2 -------";
		p2->debug();

		GROUPCOMPARE(p1, {foo, bar});
		GROUPCOMPARE(p2, {bar, foo});
	}
};
QTEST_MAIN(ERFilterSortProxy_test)
#include "ERFilterSortProxy_test.moc"