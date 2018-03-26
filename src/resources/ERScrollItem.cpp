#include "resources/ERScrollItem.h"

ERScrollItem::ERScrollItem(QWidget * parent)
	: ScrollBoxItem(parent),
	m_er(nullptr),
	m_cat(nullptr)
{
	ui.setupUi(this);

	m_icon_container = new QWidget(this);
	QHBoxLayout *icon_layout = new QHBoxLayout(m_icon_container);
	m_icon_container->setLayout(icon_layout);
	icon_layout->setMargin(2);
	icon_layout->setSpacing(0);
	icon_layout->setDirection(QBoxLayout::Direction::RightToLeft);
	ui.gridLayout->addWidget(m_icon_container, 0, 0);
	ui.gridLayout->setAlignment(m_icon_container, Qt::AlignTop | Qt::AlignRight);
	m_icon_container->show();

	m_type_icon = new QLabel(m_icon_container);
	m_type_icon->setFixedSize(16, 16);
	icon_layout->addWidget(m_type_icon);
	m_type_icon->show();

	m_goto_icon = new QLabel(m_icon_container);
	QPixmap goto_img("assets/icons/View_16xSM.png");
	m_goto_icon->setPixmap(goto_img);
	m_goto_icon->setFixedSize(16, 16);
	icon_layout->addWidget(m_goto_icon);

	m_launch_icon = new QLabel(m_icon_container);
	QPixmap launch_img("assets/icons/Effects_16xSM.png");
	m_launch_icon->setPixmap(launch_img);
	m_launch_icon->setFixedSize(16, 16);
	icon_layout->addWidget(m_launch_icon);

	m_distance_label = new QLabel(this);
	ui.gridLayout->addWidget(m_distance_label, 0, 0);
	ui.gridLayout->setAlignment(m_distance_label, Qt::AlignHCenter | Qt::AlignBottom);
	m_distance_label->setText("distance");
	m_distance_label->show();

	showTypeIcon(EResource::ANNOTATION);
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

		setDistance(er->getDistanceTo());
		connect(er, &EResource::sDistanceToChanged, this, &ERScrollItem::setDistance);

		showTypeIcon(er->getERType());
		connect(er, &EResource::sErTypeChanged, this, &ERScrollItem::showTypeIcon);

		showGotoIcon(er->getReposition());
		connect(er, &EResource::sRepositionChanged, this, &ERScrollItem::showGotoIcon);

		showAutoLaunchIcon(er->getAutoLaunch());
		connect(er, &EResource::sAutoLaunchChanged, this, &ERScrollItem::showAutoLaunchIcon);
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

void ERScrollItem::showTypeIcon(EResource::ERType type)
{
	QString path;
	switch (type) {
	case EResource::ANNOTATION:
		path = "assets/icons/Text_16xSM.png";
		break;
	case EResource::FILE:
		path = "assets/icons/LinkFile_16x.png";
		break;
	case EResource::URL:
		path = "assets/icons/Link_16xSM.png";
		break;
	}
	QPixmap pm(path);
	m_type_icon->setPixmap(pm);
}

void ERScrollItem::showGotoIcon(bool show)
{
	m_goto_icon->setVisible(show);
}

void ERScrollItem::showAutoLaunchIcon(bool show)
{
	m_launch_icon->setVisible(show);
}

void ERScrollItem::setDistance(double dist)
{
	m_distance_label->setText(QString::number(dist, 'f', 2));
}
