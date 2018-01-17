#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include <QAbstractItemModel>
#include <osg/Node>

class Group;

// Turn a osg::Group into a Qt model

class GroupModel : public QAbstractItemModel {
	Q_OBJECT
public:
	enum UserDataRole {
		PointerRole = Qt::UserRole
	};
	template <class T>
	static T get(QAbstractItemModel *model, int row) {
		return dynamic_cast<T>(
			static_cast<osg::Node*>(
				model->data(model->index(row, 0), PointerRole).value<void*>()));
	}

	GroupModel(QObject *parent);
	virtual void setGroup(Group *group);

	// tree like
	void setHierarchal(bool hierarchal);
	osg::Node *getNode(const QModelIndex &index) const;
	QModelIndex indexOf(osg::Node *node) const;

	// overrides
	//virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	virtual QModelIndex parent(const QModelIndex &index) const override;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
	virtual QVariant data(const QModelIndex &index, int role) const override;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

	// dynamic definition
	//typedef std::function<QVariant(const Group*, int, Qt::ItemDataRole)> GetterFunc;
	//void addColumn(GetterFunc getter);

	// list of stuff
	// addColumn(type?, name, getter, setter, flags);

protected:
	virtual void connectGroup(Group *g);

	Group *m_group;

	bool m_hierarchal;
};
#endif