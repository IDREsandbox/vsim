#ifndef ERDISPLAY_H
#define ERDISPLAY_H
#include <QWidget>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#include "ui_ERDisplay.h"
#include "resources/EResource.h"

class ERDisplay : public QFrame {
	Q_OBJECT

public:
	ERDisplay(QWidget *parent = nullptr);
	void setInfo(EResource* er);
	void setCategory(ECategory *cat);
	void setCount(int n);

	void setHardVisible(bool visible); // no fading
	bool canFadeIn();
	void fadeIn();
	void fadeOut();

	void setScrollBarStyle(QString s);

	//public slots:
	//void showER();
	//void hideER();

	void reload();

signals:
	void sClose();
	void sCloseAll();
	void sOpen();
	void sGoto();

protected:
	void mousePressEvent(QMouseEvent *event) override;

private:
	Ui::ERDisplay ui;
	EResource* m_er;
	ECategory *m_cat;
	QGraphicsOpacityEffect *m_display_opacity;
	QPropertyAnimation *m_fade_out_anim;
	QPropertyAnimation *m_fade_in_anim;
};

#endif // ERDISPLAY_H