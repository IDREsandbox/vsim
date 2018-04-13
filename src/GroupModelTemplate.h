#ifndef GROUPMODELTEMPLATE_H
#define GROUPMODELTEMPLATE_H

#include <QAbstractItemModel>
#include "GroupEnums.h"
#include "GroupTemplate.h"

template <class T>
class TGroupModel : public QAbstractItemModel {
public:
	TGroupModel(QObject *parent);

	virtual void setGroup(TGroup<T> *group);
	TGroup<T> *group() const;

	// model -> group conversions
	static T *get(const QAbstractItemModel *model, int row);
	static std::shared_ptr<T> getShared(TGroup<T> *base,
		const QAbstractItemModel *model, int row);
	static int baseIndex(const QAbstractItemModel *model, int row);

	// pass in a model index from this model or a proxy
	T *get(const QModelIndex &index) const;
	std::shared_ptr<T> getShared(const QModelIndex &index) const;

	QModelIndex indexOf(T *node) const;

	// overrides
	//virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column,
		const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	//Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant data(const QModelIndex &index, int role) const override;

	// dynamic definition
	//typedef std::function<QVariant(const Group*, int, Qt::ItemDataRole)> GetterFunc;
	//void addColumn(GetterFunc getter);

	// list of stuff
	// addColumn(type?, name, getter, setter, flags);

protected:
	TGroup<T> *m_group;
};

#include "GroupModelTemplate.inl"
#endif
