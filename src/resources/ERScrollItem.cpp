#include "resources/ERScrollItem.h"

#include <QtGlobal>
#include <QRect>
#include <QDebug>
#include <QGraphicsItem>
#include <QElapsedTimer>
#include <QCoreApplication>

#include "resources/EResource.h"
#include "resources/ECategory.h"
#include "Core/LockTable.h"
#include "Core/Util.h"

ERScrollItem::ERScrollItem()
	: FastScrollItem(),
	m_er(nullptr),
	m_cat(nullptr),
	m_lock(nullptr)
{
	auto dir = QCoreApplication::applicationDirPath();
	m_text_icon = QPixmap(dir + "/assets/icons/Text_Annotation_Icon_August2018.png");
	m_file_icon = QPixmap(dir + "/assets/icons/Document_16xMD.png");
	m_url_icon = QPixmap(dir + "/assets/icons/Link_16xMD.png");
	m_goto_icon = QPixmap(dir + "/assets/icons/Arrow2PointIcon_August2018.png");
	m_launch_icon = QPixmap(dir + "/assets/icons/Effects_16xMD.png");
	m_lock_icon = QPixmap(dir + "/assets/icons/Lock_16xMD.png");

	setCacheMode(QGraphicsItem::CacheMode::DeviceCoordinateCache);
}

EResource * ERScrollItem::resource() const
{
	return m_er;
}

void ERScrollItem::setER(EResource *er)
{
	Util::reconnect(this, &m_er, er);
	Util::reconnect(this, &m_lock, er ? er->lockTable() : nullptr);

	if (er) {
		connect(er, &EResource::sResourceNameChanged, this, &ERScrollItem::updateAlias);

		setCat(er->category());
		connect(er, &EResource::sCategoryChanged, this,
			[this](ECategory *old_cat, ECategory *cat) {
			setCat(cat);
		});

		connect(er, &EResource::sErTypeChanged, this, &ERScrollItem::updateAlias);
		connect(er, &EResource::sRepositionChanged, this, &ERScrollItem::updateAlias);
		connect(er, &EResource::sAutoLaunchChanged, this, &ERScrollItem::updateAlias);
		//connect(er, &EResource::sDistanceToChanged, this, &ERScrollItem::updateAlias);

		connect(m_lock, &LockTable::sLockChanged, this, &ERScrollItem::updateAlias);
	}

	update();
}

void ERScrollItem::setCat(ECategory *cat)
{
	if (m_cat) disconnect(m_cat, 0, this, 0);
	m_cat = cat;
	if (cat) {
		connect(cat, &ECategory::sColorChanged, this, &ERScrollItem::updateAlias);
	}
	else {
	}
	update();
}

void ERScrollItem::updateAlias()
{
	update(boundingRect());
}

void ERScrollItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	QElapsedTimer t; t.start();

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

	// select text color
	if (select_color.lightness() > 150) {
		select_text_color = QColor(0, 0, 0);
	}
	else {
		select_text_color = QColor(255, 255, 255);
	}

	QColor use_color;
	QColor use_text_color;
	if (isSelected()) {
		use_color = select_color;
		use_text_color = select_text_color;
	}
	else {
		use_color = color;
		use_text_color = text_color;
	}

	QRectF rect = QRectF(QPointF(0, 0), m_size);

	// paint background
	painter->setBrush(use_color);
	painter->setPen(QPen(Qt::NoPen));
	painter->drawRect(rect);

	// paint selection
	if (isSelected()) {
		painter->setPen(QPen(QColor(175, 175, 255), 3));
		painter->setBrush(Qt::BrushStyle::NoBrush);
		painter->drawRect(rect);
	}

	if (!m_er) return;

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
	name_string = m_er->getResourceName().c_str();
	painter->drawText(text_rect, name_string, opt);

	// paint distance
	if (false) {
		opt.setAlignment(Qt::AlignBottom);
		QString distance_string;
		distance_string = QString::number(m_er->getDistanceTo(), 'f', 2);
		painter->drawText(text_rect, distance_string, opt);
	}

	// paint index
	opt.setAlignment(Qt::AlignBottom | Qt::AlignRight);
	painter->drawText(text_rect, QString::number(m_er->index()), opt);

	// paint icons, right to left
	// [repo] [auto] [type]
	int icon_margin = 0;
	int icon_spacing = 0;
	QSize icon_size(16, 16);

	std::vector<QPixmap> icon_list;

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
	icon_list.push_back(type_icon);

	if (m_er->getReposition()) {
		icon_list.push_back(m_goto_icon);
	}

	if (m_er->getAutoLaunch()) {
		icon_list.push_back(m_launch_icon);
	}

	if (m_er->lockTableConst()->isLocked()) {
		icon_list.push_back(m_lock_icon);
	}

	// paint them in reverse order, right to left
	int right = size().width() - icon_margin;
	for (QPixmap &pm : icon_list) {
		QPoint top_right(right, icon_margin);
		QRect icon_rect;
		icon_rect.setSize(icon_size);
		icon_rect.moveTopRight(top_right);
		painter->drawPixmap(icon_rect, pm);

		// move top-right over
		right -= icon_size.width();
		right -= icon_margin;
	}
}