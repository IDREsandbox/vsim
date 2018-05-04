#ifndef OSGYEARMODEL_H
#define OSGYEARMODEL_H

#include "OSGGroupModel.h"
// A QAbstractItemModel wrapping the scene graph. The end result should be an
// outliner kind of thing.
// TODO: Edit commands for name, begin, end
// Columns: Name, Type, (Begin, End)

class Model;
class OSGNodeWrapper;

class OSGYearModel : public OSGGroupModel {
	Q_OBJECT
public:
	OSGYearModel(QObject *parent = nullptr);
	//virtual void setGroup(Group *group) override;

	//void setModel(Model *model);

	enum Column {
		NAME,
		TYPE,
		BEGIN,
		END
	};
	static constexpr const char *ColumnStrings[] = {
		"Name",
		"Type",
		"Begin Year",
		"End Year"
	};

	void setBase(OSGNodeWrapper *base);

	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

signals:
	void sNodeYearChanged(osg::Node *node, int year, bool begin);

private:
	OSGNodeWrapper *m_base;
};

#endif
