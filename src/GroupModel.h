#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include <functional>
#include <QAbstractItemModel>

class Group;

// Turn a osg::Group into a Qt model

class GroupModel : public QAbstractItemModel {
	Q_OBJECT
public:
	enum UserDataRole {
		PointerRole = Qt::UserRole
	};

	GroupModel(QObject *parent);

	// overrides
	//virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	virtual QModelIndex parent(const QModelIndex &index) const override;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
	virtual QVariant data(const QModelIndex &index, int role) const override;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

	void setGroup(Group *group);

	// dynamic definition
	//typedef std::function<QVariant(const Group*, int, Qt::ItemDataRole)> GetterFunc;
	//void addColumn(GetterFunc getter);

	// list of stuff
	// addColumn(type?, name, getter, setter, flags);

	virtual int itemType(int index) const;

protected:
	//int m_num_columns;
	//std::vector<int> m_types;
	//std::vector<GetterFunc> m_getters;

	Group *m_group;
};
#endif