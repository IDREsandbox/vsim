#ifndef ECATEGORYMODEL_H
#define ECATEGORYMODEL_H

#include "Core/GroupModelTemplate.h"

class ECategory;

class ECategoryModel : public TGroupModel<ECategory> {
	Q_OBJECT;
public:
	ECategoryModel(QObject *parent);
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int role) const override;
};
#endif