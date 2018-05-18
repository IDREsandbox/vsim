#ifndef CANVASTOOLBAR_H
#define CANVASTOOLBAR_H

#include <QToolBar>
#include <QComboBox>
#include <QFontComboBox>
#include <QGridLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QToolButton>

class ToolBox : public QWidget {
	Q_OBJECT
public:
	ToolBox(QWidget *parent = nullptr);
protected:
	void paintEvent(QPaintEvent *e);
};

class CanvasToolBar : public QToolBar
{
	Q_OBJECT;

public:
	CanvasToolBar(QWidget *parent = Q_NULLPTR);

public:
	// create box
	ToolBox *m_create_box;

	QToolButton *m_header1;
	QToolButton *m_header2;
	QToolButton *m_body;
	QToolButton *m_label;
	QToolButton *m_image;

	QToolButton *m_delete;

	// frame box
	ToolBox *m_color_box;

	QToolButton *m_background;
	QToolButton *m_clear_background;
	QToolButton *m_border;
	QToolButton *m_clear_border;
	QSpinBox *m_border_width;

	// font box
	ToolBox *m_font_box;

	QFontComboBox *m_font;
	QComboBox *m_font_size;
	QToolButton *m_foreground;

	QToolButton *m_bold;
	QToolButton *m_italicize;
	QToolButton *m_underline;
	QToolButton *m_strikeout;
	QToolButton *m_number;
	QToolButton *m_bullet;

	QComboBox *m_style;
	//QToolButton *m_apply_style;
	QToolButton *m_edit_styles;

	// align box
	ToolBox *m_align_box;

	QToolButton *m_halign_left;
	QToolButton *m_halign_center;
	QToolButton *m_halign_right;
	QToolButton *m_halign_justify;
	QToolButton *m_valign_top;
	QToolButton *m_valign_center;
	QToolButton *m_valign_bottom;

	QPushButton *m_done;

};

#endif