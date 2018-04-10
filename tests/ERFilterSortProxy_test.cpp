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
	void GROUPCOMPARE(const TGroup<EResource> *group, const std::vector<std::shared_ptr<EResource>> &target) {
		QCOMPARE(group->size(), target.size());
		for (unsigned int i = 0; i < group->size(); i++) {
			QCOMPARE(group->child(i), target[i].get());
		}
	}
private slots:

	void sortByTest() {
		EResourceGroup group;

		auto res0 = std::make_shared<EResource>();
		auto res1 = std::make_shared<EResource>();
		auto res2 = std::make_shared<EResource>();
		auto res3 = std::make_shared<EResource>();
		auto res4 = std::make_shared<EResource>();
		res0->setResourceName("foo");
		res1->setResourceName("bar");
		res2->setResourceName("Bab");
		res3->setResourceName("Alphabet");
		res4->setResourceName("Joe");

		group.append(res0);
		group.append(res1);
		group.append(res2);
		group.append(res3);
		ERFilterSortProxy proxy(&group);
		proxy.sortBy(ERFilterSortProxy::ALPHABETICAL);
		proxy.showGlobal(true);
		proxy.showLocal(true);

		GROUPCOMPARE(&proxy, {res3, res2, res1, res0});
		group.append(res4);
		GROUPCOMPARE(&proxy, { res3, res2, res4, res1, res0 });

		// test indicesOf
		std::vector<int> indices;
		std::vector<int> ans;
		indices = proxy.indicesOf({res0.get(), res1.get(), res3.get()});
		ans = {4, 3, 0};
		QVERIFY(indices == ans);

		indices = proxy.indicesOf({nullptr, res4.get(), res2.get() });
		ans = {-1, 2, 1};
		QVERIFY(indices == ans);

		indices = proxy.indicesOf({nullptr, res0.get(), nullptr, res3.get()}, false);
		ans = {4, 0};
		QVERIFY(indices == ans);
	}

	void globalTest() {
		EResourceGroup group;
		auto foo = std::make_shared<EResource>();
		foo->setResourceName("foo");
		foo->setGlobal(true);
		auto bar = std::make_shared<EResource>();
		bar->setResourceName("bar");
		bar->setGlobal(true);
		auto cat = std::make_shared<EResource>();
		cat->setResourceName("cat");
		cat->setGlobal(false);

		group.append(foo);
		group.append(bar);
		group.append(cat);

		ERFilterSortProxy proxy(&group);
		proxy.sortBy(ERFilterSortProxy::NONE);

		proxy.showGlobal(false);
		proxy.showLocal(false);
		GROUPCOMPARE(&proxy, { });

		proxy.showGlobal(false);
		proxy.showLocal(true);
		GROUPCOMPARE(&proxy, { cat });

		proxy.showGlobal(true);
		proxy.showLocal(false);
		GROUPCOMPARE(&proxy, { foo, bar });

		proxy.showGlobal(true);
		proxy.showLocal(true);
		GROUPCOMPARE(&proxy, { foo, bar, cat });
	}
	void removeTest() {
		EResourceGroup group;
		auto foo = std::make_shared<EResource>();
		foo->setResourceName("foo");
		auto bar = std::make_shared<EResource>();
		bar->setResourceName("bar");
		auto cat = std::make_shared<EResource>();
		cat->setResourceName("cat");

		ERFilterSortProxy p1(&group);
		p1.sortBy(ERFilterSortProxy::ALPHABETICAL);

		group.append(foo);
		group.append(bar);
		group.append(cat);

		group.removeMulti({ 0, 1 });
		GROUPCOMPARE(&p1, { cat });
	}
	void chainedProxy() {
		EResourceGroup group;
		auto foo = std::make_shared<EResource>();
		foo->setResourceName("foo");
		foo->setGlobal(false);
		auto bar = std::make_shared<EResource>();
		bar->setResourceName("bar");
		bar->setGlobal(false);
		auto cat = std::make_shared<EResource>();
		cat->setResourceName("cat");
		cat->setGlobal(true);

		ERFilterSortProxy p1(&group);
		p1.showGlobal(false);
		p1.showLocal(true);
		p1.sortBy(ERFilterSortProxy::NONE);

		ERFilterSortProxy p2(&p1);
		p2.showGlobal(true);
		p2.showLocal(true);
		p2.sortBy(ERFilterSortProxy::ALPHABETICAL);

		group.append(foo);
		group.append(bar);
		group.append(cat);

		GROUPCOMPARE(&p1, {foo, bar});
		GROUPCOMPARE(&p2, {bar, foo});
	}
	void giantChainedProxy() {
		EResourceGroup group;
		ERFilterSortProxy p1(&group);
		ERFilterSortProxy p2(&group);
		ERFilterSortProxy p3(&group);
		p3.sortBy(ERFilterSortProxy::ALPHABETICAL);
		Util::tic();
		int x = 10;
		for (int i = 0; i < x; i++) {
			auto res = std::make_shared<EResource>();
			res->setResourceName(std::to_string(i));
			group.append(res);
		}
		qDebug() << "single construction" << x << Util::toc();

		Util::tic();
		auto res = std::make_shared<EResource>();
		group.append(res);
		qDebug() << "single construction +1" << Util::toc();

		Util::tic();
		group.clear();
		qDebug() << "removeChildren" << Util::toc();

		Util::tic();
		std::vector<std::pair<size_t, std::shared_ptr<EResource>>> insertions;
		for (int i = 0; i < x; i++) {
			auto res = std::make_shared<EResource>();
			res->setResourceName(std::to_string(i));
			insertions.push_back({ i, res });
		}
		group.insertMulti(insertions);
		qDebug() << "group construction" << Util::toc();

		Util::tic();
		group.clear();
		qDebug() << "clear:" << Util::toc();
	}
};
QTEST_MAIN(ERFilterSortProxy_test)
#include "ERFilterSortProxy_test.moc"