#include "StyleSettingsWidget.h"

#include <QColorDialog>
#include <QFontDatabase>

#include "LabelStyle.h"
#include "Util.h"
#include "LabelStyle.h"
#include "narrative/NarrativeCanvas.h"
#include "narrative/NarrativeSlide.h"
#include "narrative/NarrativeSlideLabel.h"
#include <QDebug>

StyleSettingsWidget::StyleSettingsWidget(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	// style
	m_style = std::make_unique<LabelStyle>();

	// create a canvas, fullscreen it
	m_canvas = new NarrativeCanvas(ui.previewFrame);
	QVBoxLayout *layout = new QVBoxLayout;
	ui.previewFrame->setLayout(layout);
	layout->addWidget(m_canvas);
	layout->setMargin(0);
	m_canvas->setEditable(true);
	m_canvas->setBaseHeight(300);

	m_canvas->setStyleSheet("NarrativeCanvas { background: rgb(50,50,50); }");

	// create a slide
	m_slide = std::make_shared<NarrativeSlide>();
	m_label = std::make_shared<NarrativeSlideLabel>();
	m_slide->append(m_label);
	m_label->setRect(QRectF(-.5, -.25, 1, .5));

	m_canvas->setSlide(m_slide.get());

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

	connect(ui.colorPicker, &QPushButton::clicked, this,
		[this]() {
		QColorDialog dlg;
		dlg.setCurrentColor(m_style->m_fg_color);
		int result = dlg.exec();
		if (result == QDialog::Rejected) {
			return;
		}
		QColor color = dlg.selectedColor();
		ui.colorPicker->setStyleSheet(Util::colorToStylesheet(color));
		m_style->m_fg_color = color;
		refresh();
	});
	connect(ui.colorPicker_bg, &QPushButton::clicked, this,
		[this]() {
		QColorDialog dlg;
		dlg.setOption(QColorDialog::ShowAlphaChannel, true);
		dlg.setCurrentColor(m_style->m_bg_color);
		int result = dlg.exec();
		if (result == QDialog::Rejected) {
			return;
		}
		QColor color = dlg.selectedColor();
		ui.colorPicker_bg->setStyleSheet(Util::colorToStylesheet(color));
		m_style->m_bg_color = color;
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
	m_label->applyStyle(m_style.get());
}

void StyleSettingsWidget::setStyle(const LabelStyle * style)
{
	if (style != m_style.get()) m_style->copy(style);

	// update all the ui stuff

	ui.colorPicker->setStyleSheet(Util::colorToStylesheet(style->m_fg_color));
	ui.colorPicker_bg->setStyleSheet(Util::colorToStylesheet(style->m_bg_color));

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
