#ifndef SLIDESCROLLWIDGET_H
#define SLIDESCROLLWIDGET_H
#include <QWidget>
#include <QLabel>

class SlideScrollWidget : public QWidget {
	Q_OBJECT

public:
	SlideScrollWidget(QWidget * parent = nullptr);

	void setPicture();
	void setNumber(int n);

signals:
	void sMousePressEvent(QMouseEvent*);
protected:
	void mousePressEvent(QMouseEvent*);

private:
	QLabel *m_number_label;
	
};

#endif // SLIDESCROLLWIDGET_H