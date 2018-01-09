#ifndef ECATEGORYMODEL_H
#define ECATEGORYMODEL_H

#include "resources/ECategory.h"
#include "resources/ECategoryGroup.h"

#include "GroupModel.h"

class ECategoryModel : public GroupModel {
	Q_OBJECT;
public:
	ECategoryModel(QObject *parent);
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int role) const override;

};
#endif