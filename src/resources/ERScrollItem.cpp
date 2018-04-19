#include "resources/ERScrollItem.h"
#include "resources/EResource.h"
#include "resources/ECategory.h"

// some crazy alias
static constexpr void(QGraphicsItem::* const updateAlias)(const QRectF &)
	= qOverload<const QRectF &>(&QGraphicsItem::update);

ERScrollItem::ERScrollItem()
	: FastScrollItem(),
	m_er(nullptr),
	m_cat(nullptr)
{
	m_text_icon = QPixmap("assets/icons/Text_16xSM.png");
	m_file_icon = QPixmap("assets/icons/LinkFile_16x.png");
	m_url_icon = QPixmap("assets/icons/Link_16xSM.png");
	m_goto_icon = QPixmap("assets/icons/View_16xSM.png");
	m_launch_icon = QPixmap("assets/icons/Effects_16xSM.png");
}

EResource * ERScrollItem::resource() const
{
	return m_er;
}

void ERScrollItem::setER(EResource *er)
{
	if (m_er) disconnect(m_er, 0, this, 0);

	m_er = er;
	if (er) {
		connect(er, &EResource::sResourceNameChanged, this, &updateAlias);

		setCat(er->category());
		connect(er, &EResource::sCategoryChanged, this,
			[this](ECategory *old_cat, ECategory *cat) {
			setCat(cat);
		});

		//setDistance(er->getDistanceTo());
		//connect(er, &EResource::sDistanceToChanged, this, &ERScrollItem::setDistance);

		connect(er, &EResource::sErTypeChanged, this, &updateAlias);
		connect(er, &EResource::sRepositionChanged, this, &updateAlias);
		connect(er, &EResource::sAutoLaunchChanged, this, &updateAlias);
	}
	update();
}

void ERScrollItem::setCat(ECategory *cat)
{
	if (m_cat) disconnect(m_cat, 0, this, 0);
	m_cat = cat;
	if (cat) {
		connect(cat, &ECategory::sColorChanged, this, &updateAlias);
	}
	else {
	}
	update();
}

void ERScrollItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	
	// base color
	QColor color = QColor(50, 50, 50);
	if (m_cat) color = m_cat->getColor();

	QColor select_color;
	QColor text_color;
	QColor select_text_color;
	

	// lighter or darker?
	if (color.lightness() > 150) { // bright colors
		select_color = color.darker(225);
		text_color = QColor(0, 0, 0);
	}
	else { // dark colors
		select_color = color.lighter(180);
		text_color = QColor(255, 255, 255);
	}

	// text color
	if (select_color.lightness() > 150) {
		select_text_color = QColor(0, 0, 0);
	}
	else {
		select_text_color = QColor(255, 255, 255);
	}

	QColor use_color;
	QColor use_text_color;
	if (isSelected()) {
		use_color = color;
		use_text_color = text_color;
	}
	else {
		use_color = select_color;
		use_text_color = select_color;
	}

	// paint background
	painter->setBrush(use_color);
	painter->setPen(QPen(Qt::NoPen));
	painter->drawRect(QRectF(QPointF(0, 0), m_size));

	// paint text
	int text_margin = 5;
	int margin = 5;
	QRectF text_rect = QRectF(margin, margin,
		m_size.width() - 2 * margin,
		m_size.height() - 2 * margin);

	QTextOption opt;
	opt.setWrapMode(QTextOption::WordWrap);
	opt.setAlignment(Qt::AlignCenter);
	painter->setPen(use_text_color);
	QString name_string = "Invalid";
	if (m_er) name_string = m_er->getResourceName().c_str();
	painter->drawText(text_rect, name_string, opt);

	// paint distance
	opt.setAlignment(Qt::AlignBottom);
	QString distance_string;
	if (m_er) {
		distance_string = QString::number(m_er->getDistanceTo(), 'f', 2);
	}
	painter->drawText(text_rect, distance_string, opt);

	// paint icons, right to left
	// [repo] [auto] [type]
	int icon_margin = 5;
	int icon_spacing = 2;
	int icon_size = 16;

	if (m_er) {
		QRect icon_rect = QRect(0, margin, icon_size, icon_size);
		QPixmap type_icon;
		switch (m_er->getERType()) {
		case EResource::ANNOTATION:
			type_icon = m_text_icon;
			break;
		case EResource::FILE:
			type_icon = m_file_icon;
			break;
		case EResource::URL:
			type_icon = m_url_icon;
			break;
		}
		
		float left = m_size.width() - icon_margin - icon_size;
		icon_rect.setX(left);
		painter->drawPixmap(icon_rect, type_icon);

		left += icon_size;
		left += icon_spacing;
		icon_rect.setX(left);

		if (m_er->getReposition()) {
			painter->drawPixmap(icon_rect, m_goto_icon);

			left += icon_size;
			left += icon_spacing;
			icon_rect.setX(left);
		}

		if (m_er->getAutoLaunch()) {
			painter->drawPixmap(icon_rect, m_launch_icon);
		}
	}
}