#ifndef MODELGROUPMODEL_H
#define MODELGROUPMODEL_H

#include <QAbstractItemModel>

#include "Core/GroupTemplate.h"

class ModelGroup;
class Model;

// wraps a ModelGroup into a QAbstractItemModel
// allowing for qt views of imported models
class ModelGroupModel : public QAbstractItemModel {
	Q_OBJECT;
public:
	ModelGroupModel(QObject *parent = nullptr);

	enum Column {
		NAME,
		PATH, // DisplayRole
		EMBEDDED, // CheckStateRole
		// FORMAT,
		// YEAR_BEGIN,
		// YEAR_END
	};
	static constexpr const char *ColumnStrings[] = {
		"Name",
		"Path",
		"Embedded"
	};

	void setBase(ModelGroup *group);

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	//bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

private:
	//ModelGroup *m_base;
	TGroup<Model> *m_base;
};

#endif
