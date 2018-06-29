#include "Canvas/StyleSettingsWidget.h"

#include <QColorDialog>
#include <QFontDatabase>

#include "Canvas/LabelStyleGroup.h"
#include "Canvas/LabelStyle.h"
#include "Canvas/CanvasScene.h"
#include "Canvas/CanvasContainer.h"
#include "Canvas/CanvasControl.h"
#include "Core/Util.h"

#include <QDebug>

StyleSettingsWidget::StyleSettingsWidget(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	// style
	m_style = std::make_unique<LabelStyle>();

	m_scene = new CanvasScene(this);
	m_scene->setBaseHeight(300);
	m_scene->setEditable(true);

	m_control = new CanvasControl(this);
	m_control->setScene(m_scene);

	// create a canvas, fullscreen it
	m_canvas = new CanvasContainer(ui.previewFrame);
	QVBoxLayout *layout = new QVBoxLayout;
	ui.previewFrame->setLayout(layout);
	layout->addWidget(m_canvas);
	layout->setMargin(0);
	m_canvas->setEditable(true);

	m_canvas->setScene(m_scene);

	m_canvas->setObjectName("Canvas");
	m_canvas->setStyleSheet("#Canvas { background: rgb(50,50,50); }");

	// create a slide
	m_label = m_control->createLabel(m_style.get());
	m_label->setHtml("Sample Label");
	m_label->setRect(-.5, -.3, 1, .6);
	m_scene->addItem(m_label);

	m_font_sizes = QFontDatabase::standardSizes();
	QStringList size_strings;
	for (int s : m_font_sizes) {
		size_strings.append(QString::number(s));
	}
	ui.sizeBox->addItems(size_strings);

	QStringList halign;
	halign << "Left" << "Center" << "Right" << "Justify";
	ui.halign->addItems(halign);

	QStringList valign;
	valign << "Top" << "Center" << "Bottom";
	ui.valign->addItems(valign);

	connect(ui.text_color, &QPushButton::clicked, this,
		[this]() {
		QColorDialog dlg;
		dlg.setCurrentColor(m_style->m_fg_color);
		int result = dlg.exec();
		if (result == QDialog::Rejected) {
			return;
		}
		QColor color = dlg.selectedColor();
		ui.text_color->setStyleSheet(Util::colorToStylesheet(color));
		m_style->m_fg_color = color;
		refresh();
	});
	connect(ui.bg_color, &QPushButton::clicked, this,
		[this]() {
		QColorDialog dlg;
		dlg.setOption(QColorDialog::ShowAlphaChannel, true);
		dlg.setCurrentColor(m_style->m_frame.m_bg_color);
		int result = dlg.exec();
		if (result == QDialog::Rejected) {
			return;
		}
		QColor color = dlg.selectedColor();
		ui.bg_color->setStyleSheet(Util::colorToStylesheet(color));
		m_style->m_frame.m_bg_color = color;
		refresh();
	});
	connect(ui.border_color, &QPushButton::clicked, this,
		[this]() {
		QColorDialog dlg;
		dlg.setCurrentColor(m_style->m_frame.m_frame_color);
		int result = dlg.exec();
		if (result == QDialog::Rejected) {
			return;
		}
		QColor color = dlg.selectedColor();
		ui.border_color->setStyleSheet(Util::colorToStylesheet(color));
		m_style->m_frame.m_has_frame = true;
		m_style->m_frame.m_frame_color = color;
		refresh();
	});
	connect(ui.border_width, QOverload<int>::of(&QSpinBox::valueChanged), this,
		[this](int width) {
		m_style->m_frame.m_has_frame = true;
		m_style->m_frame.m_frame_width = width;
		refresh();
	});
	connect(ui.border_clear, &QAbstractButton::pressed, this,
		[this]() {
		m_style->m_frame.m_has_frame = false;
		refresh();
	});
	connect(ui.fontBox, &QFontComboBox::currentFontChanged, this,
		[this](const QFont &f) {
		m_style->m_font_family = f.family().toStdString();
		refresh();
	});
	connect(ui.sizeBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
		[this](int index) {
		int new_size = m_font_sizes[index];
		m_style->m_point_size = new_size;
		refresh();
	});
	connect(ui.halign, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
		[this](int index) {
		switch (index) {
		case 0:
			m_style->setHAlign(Qt::AlignLeft);
			break;
		case 1:
			m_style->setHAlign(Qt::AlignHCenter);
			break;
		case 2:
			m_style->setHAlign(Qt::AlignRight);
			break;
		case 3:
			m_style->setHAlign(Qt::AlignJustify);
			break;
		default:
			m_style->setHAlign(Qt::AlignLeft);
			break;
		}
		refresh();
	});
	connect(ui.valign, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
		[this](int index) {
		switch (index) {
		case 0:
			m_style->setVAlign(Qt::AlignTop);
			break;
		case 1:
			m_style->setVAlign(Qt::AlignVCenter);
			break;
		case 2:
			m_style->setVAlign(Qt::AlignBottom);
			break;
		default:
			m_style->setVAlign(Qt::AlignTop);
			break;
		}
		refresh();
	});
	connect(ui.margin, QOverload<int>::of(&QSpinBox::valueChanged), this,
		[this](int margin) {
		m_style->m_margin = margin;
		refresh();
	});
	connect(ui.bold, &QAbstractButton::clicked, this,
		[this](bool bold) {
		m_style->m_weight = bold ? QFont::Bold : QFont::Normal;
		refresh();
	});
	connect(ui.italicize, &QAbstractButton::clicked, this,
		[this](bool ital) {
		m_style->m_ital = ital;
		refresh();
	});
	connect(ui.underline, &QAbstractButton::clicked, this,
		[this](bool ul) {
		m_style->m_underline = ul;
		refresh();
	});

	setStyle(m_style.get());
	refresh();
}

void StyleSettingsWidget::refresh()
{
	//m_style->applyToNarrativeLabel(m_label.get());
	m_control->applyLabelStyle(m_label.get(), m_style.get());
}

void StyleSettingsWidget::setStyle(const LabelStyle * style)
{
	if (style != m_style.get()) m_style->copy(style);

	// update all the ui stuff

	ui.text_color->setStyleSheet(Util::colorToStylesheet(style->m_fg_color));
	ui.bg_color->setStyleSheet(Util::colorToStylesheet(style->m_frame.m_bg_color));
	ui.border_color->setStyleSheet(Util::colorToStylesheet(style->m_frame.m_frame_color));
	ui.border_width->setValue(style->m_frame.m_frame_width);

	ui.margin->setValue(style->m_margin);
	//ui.width->setValue();
	//ui.height->setValue();

	QFontDatabase fdb;
	QFont f = fdb.font(QString::fromStdString(style->m_font_family), "", style->m_point_size);
	ui.fontBox->setCurrentFont(f);

	// search for matching point size
	int size_index = ui.sizeBox->findText(QString::number(style->m_point_size));
	if (size_index >= 0) {
		ui.sizeBox->setCurrentIndex(size_index);
	}

	// halign
	Qt::Alignment al = style->m_align;
	//halign << "Left" << "Center" << "Right" << "Justify";
	if (al & Qt::AlignHCenter) {
		ui.halign->setCurrentIndex(1);
	}
	else if (al & Qt::AlignRight) {
		ui.halign->setCurrentIndex(2);
	}
	else if (al & Qt::AlignJustify) {
		ui.halign->setCurrentIndex(3);
	}
	else { // default to left
		ui.halign->setCurrentIndex(0);
	}
	//valign << "Top" << "Center" << "Bottom";
	if (al & Qt::AlignVCenter) {
		ui.valign->setCurrentIndex(1);
	}
	else if (al & Qt::AlignBottom) {
		ui.valign->setCurrentIndex(2);
	}
	else { // default to top
		ui.valign->setCurrentIndex(0);
	}

	ui.bold->setChecked(m_style->m_weight > 50);
	ui.italicize->setChecked(m_style->m_ital);
	ui.underline->setChecked(m_style->m_underline);

	refresh();
}

LabelStyle * StyleSettingsWidget::getStyle() const
{
	return m_style.get();
}
