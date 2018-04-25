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

	void addToSelection(EResource *res, bool top = true);
	void deselect(EResource *res) const;
	bool has(EResource *res) const;
	bool setTop(EResource *res);
	void setSelection(const std::vector<EResource*> &selection);
	std::vector<EResource*> getSelection() const;

signals:
	void sOpen();
	void sTopChanged(EResource *res);
	void sSelectionChanged();

protected:
	void itemMouseDoubleClickEvent(FastScrollItem *item,
		QGraphicsSceneMouseEvent *event) override;

private:
	ERScrollItem *remap(EResource *res) const;
	void insertForIndices(const std::vector<size_t> &ind);
	ERScrollItem *createItem(EResource *res);

private:
	TGroup<EResource> *m_group;

	std::map<EResource*, ERScrollItem*> m_map;
	EResource *m_old_top;

	std::vector<EResource*> m_old_selection;
};

#endif // ERSCROLLBOX_H