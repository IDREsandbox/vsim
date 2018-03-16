#include "resources/ERScrollItem.h"

ERScrollItem::ERScrollItem(QWidget * parent)
	: ScrollBoxItem(parent),
	m_er(nullptr),
	m_cat(nullptr)
{
	ui.setupUi(this);
}

void ERScrollItem::setER(EResource *er)
{
	if (m_er) disconnect(m_er, 0, this, 0);

	m_er = er;
	if (er) {
		ui.title->setText(QString::fromStdString(er->getResourceName()));
		connect(er, &EResource::sResourceNameChanged, this,
			[this]() {
			ui.title->setText(QString::fromStdString(m_er->getResourceName()));
		});

		setCat(er->category());
		connect(er, &EResource::sCategoryChanged, this,
			[this](ECategory *old_cat, ECategory *cat) {
			setCat(cat);
		});
	}
}

void ERScrollItem::setCat(ECategory *cat)
{
	if (m_cat) disconnect(m_cat, 0, this, 0);
	m_cat = cat;
	if (cat) {
		setColor(cat->getColor());
		connect(cat, &ECategory::sColorChanged, this, &ERScrollItem::setColor);
	}
	else {
		setColor(QColor(20, 20, 20));
	}
}

int ERScrollItem::widthFromHeight(int height)
{
	return height;
}

void ERScrollItem::setColor(QColor color)
{
	QColor select_color;
	QColor text_color;
	QColor select_text_color;
	// lighter or darker?
	if (color.lightness() > 150) { // bright colors
		select_color = color.darker(200);
		text_color = QColor(0, 0, 0);
	}
	else { // dark colors
		select_color = color.lighter(150);
		text_color = QColor(255, 255, 255);
	}

	// text color
	if (select_color.lightness() > 150) {
		select_text_color = QColor(0, 0, 0);
	}
	else {
		select_text_color = QColor(255, 255, 255);
	}

	QString s = "background:rgb(%1, %2, %3);";
	setDeselectStyle(QString()
		+ "background:" + colorString(color) + ";"
		+ "color:" + colorString(text_color) + ";"
	);
	setSelectStyle(QString()
		+ "background:" + colorString(select_color) + ";"
		+ "color:" + colorString(select_text_color) + ";"
	);
}

QString ERScrollItem::colorString(QColor color)
{
	QString s = "rgb(%1, %2, %3)";
	return s.arg(color.red()).arg(color.green()).arg(color.blue());
}