#ifndef ERSCROLLBOX_H
#define ERSCROLLBOX_H

#include <map>
#include "FastScrollBox.h"
#include "GroupTemplate.h"

class EResource;
class EResourceGroup;
class ERScrollItem;
//template<> class TGroup<EResource>;

class ERScrollBox : public FastScrollBox {
	Q_OBJECT

public:
	ERScrollBox(QWidget * parent = nullptr);

	void setGroup(TGroup<EResource> *group);
	void reload();

	void setSelection(const std::vector<EResource*> &selection);
	std::vector<EResource*> getSelection() const;

signals:
	void sOpen();

protected:
	void itemMouseDoubleClickEvent(FastScrollItem *item,
		QGraphicsSceneMouseEvent *event) override;

private:
	void insertForIndices(const std::vector<size_t> &ind);
	ERScrollItem *createItem(EResource *res);

private:
	TGroup<EResource> *m_group;

	std::map<EResource*, ERScrollItem*> m_map;
};

#endif // ERSCROLLBOX_H