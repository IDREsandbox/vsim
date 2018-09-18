#include "Canvas/CanvasToolBar.h"

#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include <QCoreApplication>
#include <QLabel>

#include "Gui/TitledComboBox.h"

// ComboBoxLimiter filters out scroll wheel events
// this is because scrolling dumps undo commands on every change
// TODO: find solution, need a previewChanges kind of thing
class ComboBoxLimiter : public QObject {
public:
	ComboBoxLimiter(QObject *parent = nullptr)
		: QObject(parent) {
	}
	static ComboBoxLimiter *install(QObject *target) {
		auto *cbl = new ComboBoxLimiter(target);
		target->installEventFilter(cbl);
		return cbl;
	}

	bool eventFilter(QObject *obj, QEvent *e) override
	{
		QEvent::Type type = e->type();

		switch (type) {
		case QEvent::Wheel:
			return true;
		default: break;
		// case QEvent::QKeyEvent:
		}
		return false;
	}
};

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
	QString dir = QCoreApplication::applicationDirPath();
	// all icons
	QIcon i_fill(dir + "/assets/icons/canvas/format_color_fill.png");
	QIcon i_pen(dir + "/assets/icons/canvas/border_color.png");
	QIcon i_clear(dir + "/assets/icons/canvas/format_color_reset.png");
	QIcon i_color_text(dir + "/assets/icons/canvas/format_color_text.png");
	QIcon i_bold(dir + "/assets/icons/canvas/format_bold.png");
	QIcon i_ital(dir + "/assets/icons/canvas/format_italic.png");
	QIcon i_uline(dir + "/assets/icons/canvas/format_underlined.png");
	QIcon i_strike(dir + "/assets/icons/canvas/format_strikethrough.png");
	QIcon i_number(dir + "/assets/icons/canvas/format_list_numbered.png");
	QIcon i_bullet(dir + "/assets/icons/canvas/format_list_bulleted.png");
	QIcon i_link(dir + "/assets/icons/canvas/link.png");
	QIcon i_link_off(dir + "/assets/icons/canvas/link_off.png");
	QIcon i_left(dir + "/assets/icons/canvas/format_align_left.png");
	QIcon i_center(dir + "/assets/icons/canvas/format_align_center.png");
	QIcon i_right(dir + "/assets/icons/canvas/format_align_right.png");
	QIcon i_justify(dir + "/assets/icons/canvas/format_align_justify.png");
	QIcon i_top(dir + "/assets/icons/canvas/vertical_align_top.png");
	QIcon i_vcenter(dir + "/assets/icons/canvas/vertical_align_center.png");
	QIcon i_bottom(dir + "/assets/icons/canvas/vertical_align_bottom.png");

	m_text_sizes = QFontDatabase::standardSizes();

	// create box
	m_create_box = new ToolBox(this);
	QVBoxLayout *create_layout = new QVBoxLayout(m_create_box);
	setMargins(create_layout);

	QHBoxLayout *create_row = new QHBoxLayout();
	create_layout->addLayout(create_row);
	m_header1 = new QToolButton(m_create_box);
	m_header1->setText("Header 1");
	create_row->addWidget(m_header1);
	m_header2 = new QToolButton(m_create_box);
	m_header2->setText("Header 2");
	m_header2->setContentsMargins(0, 0, 0, 0);
	create_row->addWidget(m_header2);
	create_row->addStretch(1);

	create_row = new QHBoxLayout();
	create_layout->addLayout(create_row);
	m_body = new QToolButton(m_create_box);
	m_body->setText("Body");
	create_row->addWidget(m_body);
	m_label = new QToolButton(m_create_box);
	m_label->setText("Label");
	create_row->addWidget(m_label);
	create_row->addStretch(1);

	m_edit_styles = new QToolButton(this);
	m_edit_styles->setText("Edit Styles");
	create_layout->addWidget(m_edit_styles, 0, Qt::AlignLeft);

	m_image = new QToolButton(m_create_box);
	m_image->setText("Add Image");
	create_layout->addWidget(m_image);

	// color box
	m_color_box = new ToolBox(this);
	QVBoxLayout *color_rows = new QVBoxLayout(m_color_box);
	setMargins(color_rows);

	QHBoxLayout *color_row = new QHBoxLayout();
	color_rows->addLayout(color_row);

	m_background = new QToolButton(m_color_box);
	m_background->setIcon(i_fill);
	color_row->addWidget(m_background, 0, Qt::AlignLeft);
	m_clear_background = new QToolButton(m_color_box);
	m_clear_background->setIcon(i_clear);
	color_row->addWidget(m_clear_background, 0, Qt::AlignLeft);
	color_row->addStretch(1);

	QHBoxLayout *border_row = new QHBoxLayout();
	color_rows->addLayout(border_row);

	m_border = new QToolButton(m_color_box);
	m_border->setIcon(i_pen);
	border_row->addWidget(m_border, 0, Qt::AlignLeft);
	m_clear_border = new QToolButton(m_color_box);
	m_clear_border->setIcon(i_clear);
	border_row->addWidget(m_clear_border, 0, Qt::AlignLeft);

	m_border_width = new QSpinBox(m_color_box);
	m_border_width->setMinimum(0);
	m_border_width->setMaximum(100);
	m_border_width->setFixedWidth(20);
	m_border_width->setMaximumWidth(20);
	m_border_width->resize(20, 10);
	border_row->addWidget(m_border_width, 0, Qt::AlignLeft);
	border_row->addStretch(1);

	m_style = new TitledComboBox(m_color_box);
	m_style->setTitle("Revert to ...");
	//ComboBoxLimiter::install(m_style);
	m_style->setFocusPolicy(Qt::NoFocus);
	m_style->addItem("Header 1");
	m_styles.push_back(LabelType::HEADER1);
	m_style->addItem("Header 2");
	m_styles.push_back(LabelType::HEADER2);
	m_style->addItem("Body");
	m_styles.push_back(LabelType::BODY);
	m_style->addItem("Label");
	m_styles.push_back(LabelType::LABEL);
	//m_style->addItem("None");
	//m_styles.push_back(LabelType::NONE);
	color_rows->addWidget(m_style);

	m_delete = new QToolButton(this);
	m_delete->setText("Delete");
	color_rows->addWidget(m_delete, 0, Qt::AlignLeft);

	// font box
	m_font_box = new ToolBox(this);
	QVBoxLayout *font_rows = new QVBoxLayout(m_font_box);
	setMargins(font_rows);

	m_font = new QFontComboBox(m_font_box);
	//ComboBoxLimiter::install(m_font);
	m_font->setFocusPolicy(Qt::NoFocus); 
	font_rows->addWidget(m_font);

	QHBoxLayout *font_size_row = new QHBoxLayout();
	font_rows->addLayout(font_size_row);

	m_font_size = new QComboBox(m_font_box);
	for (int size : m_text_sizes) {
		m_font_size->addItem(QString::number(size));
	}
	m_font_size->setMinimumWidth(40);
	//ComboBoxLimiter::install(m_font_size);
	m_font_size->setFocusPolicy(Qt::NoFocus);
	font_size_row->addWidget(m_font_size, 0, Qt::AlignLeft);
	font_size_row->addStretch(1);

	QHBoxLayout *mod_row = new QHBoxLayout();
	font_rows->addLayout(mod_row);
	m_bold = new QToolButton(this);
	m_bold->setIcon(i_bold);
	m_bold->setCheckable(true);
	mod_row->addWidget(m_bold, 0, Qt::AlignLeft);
	m_italicize = new QToolButton(this);
	m_italicize->setIcon(i_ital);
	m_italicize->setCheckable(true);
	mod_row->addWidget(m_italicize, 0, Qt::AlignLeft);
	m_underline = new QToolButton(this);
	m_underline->setIcon(i_uline);
	m_underline->setCheckable(true);
	mod_row->addWidget(m_underline, 0, Qt::AlignLeft);
	m_strikeout = new QToolButton(this);
	m_strikeout->setIcon(i_strike);
	m_strikeout->setCheckable(true);
	mod_row->addWidget(m_strikeout, 0, Qt::AlignLeft);
	m_foreground = new QToolButton(this);
	m_foreground->setIcon(i_color_text);
	mod_row->addWidget(m_foreground, 0, Qt::AlignLeft);
	mod_row->addStretch(1);

	QHBoxLayout *bullet_row = new QHBoxLayout();
	font_rows->addLayout(bullet_row);
	m_number = new QToolButton(this);
	m_number->setIcon(i_number);
	bullet_row->addWidget(m_number, 0, Qt::AlignLeft);
	m_bullet = new QToolButton(this);
	m_bullet->setIcon(i_bullet);
	bullet_row->addWidget(m_bullet, 0, Qt::AlignLeft);
	m_link = new QToolButton(this);
	m_link->setIcon(i_link);
	bullet_row->addWidget(m_link, 0, Qt::AlignLeft);
	m_link_off = new QToolButton(this);
	m_link_off->setIcon(i_link_off);
	bullet_row->addWidget(m_link_off, 0, Qt::AlignLeft);
	bullet_row->addStretch(1);

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

	m_coord_box = new ToolBox(this);
	QGridLayout *coord_layout = new QGridLayout(m_coord_box);
	// x [] y []
	// w [] h []
	QLabel *x_label = new QLabel("x", m_coord_box);
	QLabel *y_label = new QLabel("y", m_coord_box);
	QLabel *w_label = new QLabel("w", m_coord_box);
	QLabel *h_label = new QLabel("h", m_coord_box);
	m_x_spinbox = new QDoubleSpinBox(m_coord_box);
	m_y_spinbox = new QDoubleSpinBox(m_coord_box);
	m_w_spinbox = new QDoubleSpinBox(m_coord_box);
	m_h_spinbox = new QDoubleSpinBox(m_coord_box);
	x_label->setStyleSheet("background:none;");
	y_label->setStyleSheet("background:none;");
	w_label->setStyleSheet("background:none;");
	h_label->setStyleSheet("background:none;");
	m_x_spinbox->setMinimum(-10000);
	m_x_spinbox->setMaximum(10000);
	m_x_spinbox->setDecimals(1);
	m_y_spinbox->setMinimum(-10000);
	m_y_spinbox->setMaximum(10000);
	m_y_spinbox->setDecimals(1);
	m_w_spinbox->setMinimum(.1);
	m_w_spinbox->setMaximum(10000);
	m_w_spinbox->setDecimals(1);
	m_h_spinbox->setMinimum(.1);
	m_h_spinbox->setMaximum(10000);
	m_h_spinbox->setDecimals(1);
	//m_x_spinbox->setMinimumWidth(50);
	//m_y_spinbox->setMinimumWidth(50);
	//m_w_spinbox->setMinimumWidth(50);
	//m_h_spinbox->setMinimumWidth(50);
	//m_x_spinbox->setFixedWidth(50);
	//m_y_spinbox->setFixedWidth(50);
	//m_w_spinbox->setFixedWidth(50);
	//m_h_spinbox->setFixedWidth(50);

	coord_layout->addWidget(x_label, 0, 0);
	coord_layout->addWidget(m_x_spinbox, 0, 1);
	coord_layout->addWidget(y_label, 0, 2);
	coord_layout->addWidget(m_y_spinbox, 0, 3);
	coord_layout->addWidget(w_label, 1, 0);
	coord_layout->addWidget(m_w_spinbox, 1, 1);
	coord_layout->addWidget(h_label, 1, 2);
	coord_layout->addWidget(m_h_spinbox, 1, 3);
	coord_layout->setColumnStretch(1, 1);
	coord_layout->setColumnStretch(3, 1);

	ToolBox *spacer_box = new ToolBox(this);
	spacer_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	QHBoxLayout *spacer_layout = new QHBoxLayout(spacer_box);
	setMargins(spacer_layout);

	m_done = new QPushButton(spacer_box);
	m_done->setText("Done");
	m_done->setShortcut(QKeySequence(Qt::Key_Escape));
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
	addWidget(m_coord_box);
	addSeparator();
	addWidget(spacer_box);
	setOrientation(Qt::Orientation::Vertical);

	//m_font->hide();
	//m_style->hide();

	//QAction *test = new QAction(this);
	//test->setText("Test Action");
	//addAction(test);

	connect(m_font, QOverload<int>::of(&QFontComboBox::activated),
		this, [this](int index) {
		emit sFont(m_font->currentFont());
	});

	connect(m_font_size, QOverload<int>::of(&QComboBox::activated),
		this, [this](int index) {
		int size = 12;
		if (index >= 0 && index < m_text_sizes.size()) {
			size = m_text_sizes[index];
		}
		emit sTextSize(size);
	});

	connect(m_style, QOverload<int>::of(&QComboBox::activated),
		this, [this](int index) {
		LabelType type = LabelType::NONE;
		if (index >= 0 && index < m_styles.size()) {
			type = m_styles[index];
		}
		emit sLabelType(type);
	});
}

QRectF CanvasToolBar::getRect() const
{
	return QRectF(m_x_spinbox->value(),
		m_y_spinbox->value(),
		m_w_spinbox->value(),
		m_h_spinbox->value());
}

void CanvasToolBar::setRect(QRectF rect)
{
	if (m_x_spinbox->hasFocus()) return;
	if (m_y_spinbox->hasFocus()) return;
	if (m_w_spinbox->hasFocus()) return;
	if (m_h_spinbox->hasFocus()) return;

	bool enable = !rect.isNull();
	m_x_spinbox->setEnabled(enable);
	m_y_spinbox->setEnabled(enable);
	m_w_spinbox->setEnabled(enable);
	m_h_spinbox->setEnabled(enable);
	m_x_spinbox->blockSignals(true);
	m_y_spinbox->blockSignals(true);
	m_w_spinbox->blockSignals(true);
	m_h_spinbox->blockSignals(true);
	m_x_spinbox->setValue(rect.x());
	m_y_spinbox->setValue(rect.y());
	m_w_spinbox->setValue(rect.width());
	m_h_spinbox->setValue(rect.height());
	m_x_spinbox->blockSignals(false);
	m_y_spinbox->blockSignals(false);
	m_w_spinbox->blockSignals(false);
	m_h_spinbox->blockSignals(false);
}
