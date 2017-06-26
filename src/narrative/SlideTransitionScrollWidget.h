#ifndef SLIDETRANSITIONSCROLLWIDGET_H
#define SLIDETRANSITIONSCROLLWIDGET_H
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class SlideTransitionScrollWidget : public QWidget {
	Q_OBJECT

public:
	SlideTransitionScrollWidget(QWidget * parent = nullptr);

	void setDuration(float);

signals:
	void sMousePressEvent(QMouseEvent*);
protected:
	void mousePressEvent(QMouseEvent*);

private:
	QVBoxLayout *m_layout;
	QLabel *m_duration_label;
};

#endif // SLIDETRANSITIONSCROLLWIDGET_H