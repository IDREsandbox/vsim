#ifndef ECATEGORYLEGEND_H
#define ECATEGORYLEGEND_H

#include <QFrame>
#include <QListWidget>

class ECategoryGroup;

class ECategoryLegend : public QFrame {
	Q_OBJECT;
public:
	ECategoryLegend(QWidget *parent = nullptr);

	void setCategoryGroup(ECategoryGroup *cats);

	// show/hide

	// sets bottom right anchor
	void setAnchor(QPoint p);

	void refresh();

protected:
	void paintEvent(QPaintEvent *e) override;
	void resizeEvent(QResizeEvent *e) override;

private:
	void reposition();

private:
	class ListView : public QListWidget {
	public:
		ListView(QWidget *parent);
		QSize sizeHint() const override;
	};

private:
	ECategoryGroup *m_cats;
	QPoint m_anchor;

	ListView *m_list;
};

#endif