#ifndef GROUPSIGNALS_H
#define GROUPSIGNALS_H

#include <memory>
#include <vector>
#include <set>
#include <QObject>

class GroupSignals : public QObject {
	Q_OBJECT;
public:
	GroupSignals(QObject *parent = nullptr);

	bool restrictedToCurrent() const;
	virtual void setRestrictToCurrent(bool restrict);

signals:
	// emitted when cleared
	void sAboutToReset();
	void sReset();

	void sItemsMoved(std::vector<std::pair<size_t, size_t>>); // <from, to> items sorted by from
	void sEdited(const std::set<size_t> &edited); // for proxy/big listeners

	// single
	// void sNew(int index); // inserted node at index
	// void sDelete(int index); // deleted node at index
	// void sSet(int index); // changed (set) node at index

	// continuous, baseline, used for qt
	void sAboutToInsert(size_t index, size_t count);
	void sInserted(size_t index, size_t count);
	void sAboutToRemove(size_t index, size_t count);
	void sRemoved(size_t index, size_t count);

	// arbitrary indices, used for proxy
	//void sAboutToInsertMulti(const std::vector<size_t> &); // unused
	void sInsertedMulti(const std::vector<size_t> &);
	void sAboutToRemoveMulti(const std::vector<size_t> &);
	void sRemovedMulti(const std::vector<size_t> &);

	void sRestrictToCurrentChanged(bool restrict);

	// cant use types because of template
	//// arbitrary indices, used for er proxy, group
	//void sAboutToInsertMulti(const std::vector<std::pair<size_t, T*>> &);
	//void sInsertedMulti(const std::vector<std::pair<size_t, T*>> &);
	//void sAboutToRemoveMulti(const std::vector<size_t> &);
	//void sRemovedMulti(const std::vector<size_t> &);

	//// sets, used for canvas
	//void sAboutToInsertMultiP(const std::set<T*> &);
	//void sInsertedMultiP(const std::set<T*> &);
	//void sAboutToRemoveMultiP(const std::set<T*> &nodes);
	//void sRemovedMultiP(const std::set<T*> &nodes);

private:
	bool m_restricted;
};

#endif