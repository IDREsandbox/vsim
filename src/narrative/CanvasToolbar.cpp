#include "CanvasToolbar.h"

#include <QPaintEvent>
#include <QPainter>

ToolBox::ToolBox(QWidget *parent)
	: QWidget(parent) {
	setContentsMargins(0, 0, 0, 0);
	setAttribute(Qt::WA_NoSystemBackground, true);
}

void ToolBox::paintEvent(QPaintEvent *e) {
	//QPainter p(this);
	//p.setBrush(QColor(255, 0, 0));
	//p.drawRect(0, 0, size().width(), size().height());
}

void setMargins(QLayout *layout) {
	layout->setContentsMargins(4, 4, 4, 4);
	layout->setSpacing(4);
}

CanvasToolBar::CanvasToolBar(QWidget *parent)
	: QToolBar(parent)
{
	// all icons
	QIcon i_fill("assets/icons/canvas/format_color_fill.png");
	QIcon i_pen("assets/icons/canvas/border_color.png");
	QIcon i_color_text("assets/icons/canvas/format_color_text.png");
	QIcon i_bold("assets/icons/canvas/format_bold.png");
	QIcon i_ital("assets/icons/canvas/format_italic.png");
	QIcon i_uline("assets/icons/canvas/format_underlined.png");
	QIcon i_strike("assets/icons/canvas/format_strikethrough.png");
	QIcon i_number("assets/icons/canvas/format_list_numbered.png");
	QIcon i_bullet("assets/icons/canvas/format_list_bulleted.png");
	QIcon i_left("assets/icons/canvas/format_align_left.png");
	QIcon i_center("assets/icons/canvas/format_align_center.png");
	QIcon i_right("assets/icons/canvas/format_align_right.png");
	QIcon i_justify("assets/icons/canvas/format_align_justify.png");
	QIcon i_top("assets/icons/canvas/vertical_align_top.png");
	QIcon i_vcenter("assets/icons/canvas/vertical_align_center.png");
	QIcon i_bottom("assets/icons/canvas/vertical_align_bottom.png");

	QList<int> sizes = QFontDatabase::standardSizes();


	// create box
	m_create_box = new ToolBox(this);
	QVBoxLayout *create_layout = new QVBoxLayout(m_create_box);
	setMargins(create_layout);

	m_header1 = new QToolButton(m_create_box);
	m_header1->setText("Header 1");
	create_layout->addWidget(m_header1);
	m_header2 = new QToolButton(m_create_box);
	m_header2->setText("Header 2");
	m_header2->setContentsMargins(0, 0, 0, 0);
	create_layout->addWidget(m_header2);
	m_body = new QToolButton(m_create_box);
	m_body->setText("Body");
	create_layout->addWidget(m_body);
	m_label = new QToolButton(m_create_box);
	m_label->setText("Label");
	create_layout->addWidget(m_label);
	m_image = new QToolButton(m_create_box);
	m_image->setText("Image");
	create_layout->addWidget(m_image);

	m_delete = new QToolButton(this);
	m_delete->setText("Delete");
	create_layout->addWidget(m_delete);

	// color box
	m_color_box = new ToolBox(this);

	QHBoxLayout *color_row = new QHBoxLayout(m_color_box);
	setMargins(color_row);

	m_background = new QToolButton(m_color_box);
	m_background->setIcon(i_fill);
	color_row->addWidget(m_background, 0, Qt::AlignLeft);
	m_border = new QToolButton(m_color_box);
	m_border->setIcon(i_pen);
	color_row->addWidget(m_border, 0, Qt::AlignLeft);
	m_border_width = new QSpinBox(m_color_box);
	m_border_width->setMinimum(0);
	m_border_width->setMaximum(100);
	color_row->addWidget(m_border_width, 0, Qt::AlignLeft);
	color_row->addStretch(1);

	// font box
	m_font_box = new ToolBox(this);
	QVBoxLayout *font_rows = new QVBoxLayout(m_font_box);
	setMargins(font_rows);

	m_font = new QFontComboBox(m_font_box);
	font_rows->addWidget(m_font);

	QHBoxLayout *font_size_row = new QHBoxLayout();
	font_rows->addLayout(font_size_row);

	m_foreground = new QToolButton(m_color_box);
	m_foreground->setIcon(i_color_text);
	font_size_row->addWidget(m_foreground, 0, Qt::AlignLeft);
	m_font_size = new QComboBox(m_font_box);
	for (int size : QFontDatabase::standardSizes()) {
		m_font_size->addItem(QString::number(size));
	}
	m_font_size->setMinimumWidth(40);
	font_size_row->addWidget(m_font_size, 0, Qt::AlignLeft);
	font_size_row->addStretch(1);

	QHBoxLayout *mod_row = new QHBoxLayout();
	font_rows->addLayout(mod_row);
	m_bold = new QToolButton(this);
	m_bold->setIcon(i_bold);
	mod_row->addWidget(m_bold, 0, Qt::AlignLeft);
	m_italicize = new QToolButton(this);
	m_italicize->setIcon(i_ital);
	mod_row->addWidget(m_italicize, 0, Qt::AlignLeft);
	m_underline = new QToolButton(this);
	m_underline->setIcon(i_uline);
	mod_row->addWidget(m_underline, 0, Qt::AlignLeft);
	m_strikeout = new QToolButton(this);
	m_strikeout->setIcon(i_strike);
	mod_row->addWidget(m_strikeout, 0, Qt::AlignLeft);
	mod_row->addStretch(1);

	QHBoxLayout *bullet_row = new QHBoxLayout();
	font_rows->addLayout(bullet_row);
	m_number = new QToolButton(this);
	m_number->setIcon(i_number);
	bullet_row->addWidget(m_number, 0, Qt::AlignLeft);
	m_bullet = new QToolButton(this);
	m_bullet->setIcon(i_bullet);
	bullet_row->addWidget(m_bullet, 0, Qt::AlignLeft);
	bullet_row->addStretch(1);

	QComboBox *m_style = new QComboBox(m_font_box);
	m_style->addItem("None");
	m_style->addItem("Header 1");
	m_style->addItem("Header 2");
	m_style->addItem("Body");
	m_style->addItem("Label");
	m_style->addItem("Image");
	font_rows->addWidget(m_style);

	QHBoxLayout *styles_row = new QHBoxLayout();
	font_rows->addLayout(styles_row);

	//m_apply_style = new QToolButton(this);
	//m_apply_style->setText("Apply");
	//styles_row->addWidget(m_apply_style);
	m_edit_styles = new QToolButton(this);
	m_edit_styles->setText("Edit Styles");
	styles_row->addWidget(m_edit_styles, 0, Qt::AlignLeft);

	// align box
	m_align_box = new ToolBox(this);
	m_align_box->setAutoFillBackground(false);
	QVBoxLayout *align_rows = new QVBoxLayout(m_align_box);
	setMargins(align_rows);

	QHBoxLayout *align_row = new QHBoxLayout();
	align_rows->addLayout(align_row);
	QHBoxLayout *valign_row = new QHBoxLayout();
	align_rows->addLayout(valign_row);

	m_halign_left = new QToolButton(this);
	m_halign_left->setIcon(i_left);
	align_row->addWidget(m_halign_left);
	m_halign_center = new QToolButton(this);
	m_halign_center->setIcon(i_center);
	align_row->addWidget(m_halign_center);
	m_halign_right = new QToolButton(this);
	m_halign_right->setIcon(i_right);
	align_row->addWidget(m_halign_right);
	m_halign_justify = new QToolButton(this);
	m_halign_justify->setIcon(i_justify);
	align_row->addWidget(m_halign_justify);
	align_row->addStretch(1);

	m_valign_top = new QToolButton(this);
	m_valign_top->setIcon(i_top);
	valign_row->addWidget(m_valign_top);
	m_valign_center = new QToolButton(this);
	m_valign_center->setIcon(i_vcenter);
	valign_row->addWidget(m_valign_center);
	m_valign_bottom = new QToolButton(this);
	m_valign_bottom->setIcon(i_bottom);
	valign_row->addWidget(m_valign_bottom);
	valign_row->addStretch(1);

	ToolBox *spacer_box = new ToolBox(this);
	spacer_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	QHBoxLayout *spacer_layout = new QHBoxLayout(spacer_box);

	m_done = new QPushButton(spacer_box);
	m_done->setText("Done");
	spacer_layout->addWidget(m_done, 0, Qt::AlignBottom);

	// add boxes to toolbar
	addWidget(m_create_box);
	addSeparator();
	addWidget(m_color_box);
	addSeparator();
	addWidget(m_font_box);
	addSeparator();
	addWidget(m_align_box);
	addSeparator();
	addWidget(spacer_box);
	setOrientation(Qt::Orientation::Vertical);

	//QAction *test = new QAction(this);
	//test->setText("Test Action");
	//addAction(test);

}