#ifndef OSGYEARMODEL_H
#define OSGYEARMODEL_H

#include "OSGGroupModel.h"
// A QAbstractItemModel wrapping the scene graph. The end result should be an
// outliner kind of thing.
// TODO: Edit commands for name, begin, end
// Columns: Name, Type, (Begin, End)

class Model;

class OSGYearModel : public OSGGroupModel {
	Q_OBJECT
public:
	OSGYearModel(QObject *parent = nullptr);
	//virtual void setGroup(Group *group) override;

	void setModel(Model *model);

	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

private:
	Model *m_model;
	//osg::ref_ptr<ModelGroup> m_model_group;
};

#endif
