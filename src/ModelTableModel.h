#ifndef MODELTABLEMODEL_H
#define MODELTABLEMODEL_H

#include <QAbstractItemModel>
#include "ModelGroup.h"

// A QAbstractItemModel wrapping the scene graph. The end result should be an
// outliner kind of thing.
// Columns: Name, Type, (Begin, End)
class ModelTableModel : public QAbstractItemModel {
	Q_OBJECT
public:
	ModelTableModel(ModelGroup *group, QObject *parent = 0);
	~ModelTableModel();

	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	Qt::ItemFlags flags(const QModelIndex &index) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	//bool setHeaderData(int section, Qt::Orientation orientation,
	//	const QVariant &value, int role = Qt::EditRole) override;

	//bool insertColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;
	//bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;
	//bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
	//bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;

	QModelIndex indexOf(osg::Node *) const;
	osg::Node *getNode(QModelIndex) const;

	void setGroup(ModelGroup *group);

	static void removeUserValue(osg::Node *node, const std::string &name);
private:
	osg::ref_ptr<ModelGroup> m_group;
};

#endif
