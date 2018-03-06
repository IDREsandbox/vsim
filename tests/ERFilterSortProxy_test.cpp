#include <QTest>
#include <QDebug>
#include <QSignalSpy>
#include "resources/ERFilterSortProxy.h"
#include "resources/EResourceGroup.h"
#include "resources/EResource.h"
#include "Util.h"

class ERFilterSortProxy_test : public QObject {
	Q_OBJECT
private:
	void reset() {
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
		osg::ref_ptr<Group> group = new EResourceGroup;
		osg::ref_ptr<EResource> res0 = new EResource;
		osg::ref_ptr<EResource> res1 = new EResource;
		osg::ref_ptr<EResource> res2 = new EResource;
		osg::ref_ptr<EResource> res3 = new EResource;
		osg::ref_ptr<EResource> res4 = new EResource;
		res0->setResourceName("foo");
		res1->setResourceName("bar");
		res2->setResourceName("Bab");
		res3->setResourceName("Alphabet");
		res4->setResourceName("Joe");

		group->addChild(res0);
		group->addChild(res1);
		group->addChild(res2);
		group->addChild(res3);
		osg::ref_ptr<ERFilterSortProxy> proxy = new ERFilterSortProxy(group);
		proxy->sortBy(ERFilterSortProxy::ALPHABETICAL);
		proxy->showGlobal(true);
		proxy->showLocal(true);

		GROUPCOMPARE(proxy, {res3, res2, res1, res0});
		group->addChild(res4);
		GROUPCOMPARE(proxy, { res3, res2, res4, res1, res0 });

	}
	//void insertSignalTests() {
	//	reset();
	//	group->addChild(res0);
	//	group->addChild(res1);
	//	group->addChild(res2);
	//	group->addChild(res3);
	//	proxy = new ERFilterSortProxy(group);
	//	proxy->sortBy(ERFilterSortProxy::ALPHABETICAL);
	//	proxy->showGlobal(true);
	//	proxy->showLocal(true);
	//	// {Alphabet, Bab, bar, foo}
	//	// add Joe, should be index 2
	//	QSignalSpy newSpy(proxy, &Group::sNew);
	//	group->addChild(res4);
	//	qDebug() << "nothng?";
	//	proxy->debug();
	//	// sNew(2)
	//	QCOMPARE(newSpy.size(), 1);
	//	QCOMPARE(newSpy.takeFirst().at(0).toInt(), 2);
	//}
	//void globalTest() {
	//	reset();

	//	reset();
	//	osg::ref_ptr<Group> group = new EResourceGroup;
	//	osg::ref_ptr<EResource> res0 = new EResource;
	//	osg::ref_ptr<EResource> res1 = new EResource;
	//	osg::ref_ptr<EResource> res2 = new EResource;
	//	osg::ref_ptr<EResource> res3 = new EResource;
	//	osg::ref_ptr<EResource> res4 = new EResource;
	//	res0->setName("n0");
	//	res1->setName("n1");
	//	res2->setName("n2");
	//	res3->setName("n3");
	//	res0->setGlobal(true);
	//	res1->setGlobal(false);
	//	res2->setGlobal(false);
	//	res3->setGlobal(true);

	//	ERFilterSortProxy proxy(group);

	//	proxy.showGlobal(true);
	//	proxy.showLocal(true);
	//	GROUPCOMPARE(&proxy, { res0, res1, res2, res3 });


	//	proxy.showGlobal(false);
	//	proxy.showLocal(true);
	//	GROUPCOMPARE(&proxy, { res1, res2 });

	//	proxy.showGlobal(true);
	//	proxy.showLocal(false);
	//	GROUPCOMPARE(&proxy, { res0, res3 });
	//}

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
		proxy->sortBy(ERFilterSortProxy::NONE);

		proxy->showGlobal(false);
		proxy->showLocal(false);
		GROUPCOMPARE(proxy, { });

		proxy->showGlobal(false);
		proxy->showLocal(true);
		GROUPCOMPARE(proxy, { cat });

		proxy->showGlobal(true);
		proxy->showLocal(false);
		GROUPCOMPARE(proxy, { foo, bar });

		proxy->showGlobal(true);
		proxy->showLocal(true);
		GROUPCOMPARE(proxy, { foo, bar, cat });
	}
	void removeTest() {
		osg::ref_ptr<Group> group = new EResourceGroup;
		osg::ref_ptr<EResource> foo = new EResource;
		foo->setResourceName("foo");
		osg::ref_ptr<EResource> bar = new EResource;
		bar->setResourceName("bar");
		osg::ref_ptr<EResource> cat = new EResource;
		cat->setResourceName("cat");

		osg::ref_ptr<ERFilterSortProxy> p1 = new ERFilterSortProxy(group);
		p1->sortBy(ERFilterSortProxy::ALPHABETICAL);

		group->addChild(foo);
		group->addChild(bar);
		group->addChild(cat);

		group->removeChildrenSet({ 0, 1 });
		GROUPCOMPARE(p1, { cat });
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
		p1->showGlobal(false);
		p1->showLocal(true);
		p1->sortBy(ERFilterSortProxy::NONE);

		osg::ref_ptr<ERFilterSortProxy> p2 = new ERFilterSortProxy(p1);
		p2->showGlobal(true);
		p2->showLocal(true);
		p2->sortBy(ERFilterSortProxy::ALPHABETICAL);

		group->addChild(foo);
		group->addChild(bar);
		group->addChild(cat);

		GROUPCOMPARE(p1, {foo, bar});
		GROUPCOMPARE(p2, {bar, foo});
	}
	void giantChainedProxy() {
		osg::ref_ptr<Group> group = new EResourceGroup;
		osg::ref_ptr<ERFilterSortProxy> p1 = new ERFilterSortProxy(group);
		osg::ref_ptr<ERFilterSortProxy> p2 = new ERFilterSortProxy(group);
		osg::ref_ptr<ERFilterSortProxy> p3 = new ERFilterSortProxy(group);
		p3->sortBy(ERFilterSortProxy::ALPHABETICAL);
		Util::tic();
		int x = 1;
		for (int i = 0; i < x; i++) {
			EResource *res = new EResource;
			res->setResourceName(std::to_string(i));
			group->addChild(res);
		}
		qDebug() << "single construction" << x << Util::toc();

		Util::tic();
		EResource *res = new EResource;
		group->addChild(res);
		qDebug() << "single construction +1" << Util::toc();

		Util::tic();
		group->removeChildren(0, group->getNumChildren());
		qDebug() << "removeChildren" << Util::toc();

		Util::tic();
		std::vector<std::pair<size_t, osg::Node*>> insertions;
		for (int i = 0; i < x; i++) {
			EResource *res = new EResource;
			res->setResourceName(std::to_string(i));
			insertions.push_back({ i, res });
		}
		group->insertChildrenSet(insertions);
		qDebug() << "group construction" << Util::toc();

		Util::tic();
		group->clear();
		qDebug() << "clear:" << Util::toc();
	}
};
QTEST_MAIN(ERFilterSortProxy_test)
#include "ERFilterSortProxy_test.moc"