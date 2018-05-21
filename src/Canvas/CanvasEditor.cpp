#include "CanvasEditor.h"

#include "Canvas/CanvasContainer.h"
#include "Canvas/CanvasScene.h"
#include "Canvas/CanvasToolBar.h"
#include "Canvas/CanvasControl.h"
#include "Canvas/LabelStyleGroup.h"
#include "Canvas/LabelStyle.h"
#include <QDebug>
#include <QResizeEvent>
#include <QtGlobal>
#include <QApplication>
#include <QLineEdit>
#include <QColorDialog>

class InternalMainWindow : public QMainWindow {
public:
	InternalMainWindow(QWidget *parent = nullptr)
		: QMainWindow(parent) {
		setWindowFlags(0);
		//setAttribute(Qt::WA_NoBackground);
		setContextMenuPolicy(
			Qt::NoContextMenu
		);
		setObjectName("window");
		setStyleSheet("#window{background: transparent;}");
	}

	void calcMask() {
		auto cr = childrenRegion();
		if (cr.isNull()) {
			setAttribute(Qt::WA_TransparentForMouseEvents);
		}
		else {
			setAttribute(Qt::WA_TransparentForMouseEvents, false);
			setMask(cr);
		}
	}

protected:
	void resizeEvent(QResizeEvent *e) {
		QMainWindow::resizeEvent(e);
		calcMask();
	}
	bool event(QEvent *e) {
		QEvent::Type type = e->type();
		if (type == QEvent::LayoutRequest) {
			calcMask();
		}
		return QMainWindow::event(e);
	}
};

CanvasEditor::CanvasEditor(QWidget * parent)
	: QWidget(parent),
	m_styles(nullptr)
{
	QGridLayout *layout = new QGridLayout(this);
	layout->setMargin(0);

	m_container = new CanvasContainer(this);
	layout->addWidget(m_container, 0, 0);
	m_container->setEditable(true);

	m_default_styles = std::make_unique<LabelStyleGroup>();

	// there are 2 ways to let mouse events pass underneath this toolbar layer
	// 1. set parent to container (easier)
	//    this is what we're going to do
	//    edit: doesn't work, events don't propagate to siblings (the graphics view)
	// 2. use a mask, whenever children change, update mask
	//    this is what MainWindow.cpp does

	m_internal_window = new InternalMainWindow(this);
	layout->addWidget(m_internal_window, 0, 0);
	m_internal_window->show();

	m_tb = new CanvasToolBar(m_internal_window);
	m_internal_window->addToolBar(Qt::ToolBarArea::LeftToolBarArea, m_tb);

	m_cc = new CanvasControl(this);
	//cc->setScene(canvas);
	//cc->setStack(stack);

	// connecting toolbar to control
	m_button_type_map = {
		{ m_tb->m_header1, LabelType::HEADER1 },
		{ m_tb->m_header2, LabelType::HEADER2 },
		{ m_tb->m_body,    LabelType::BODY    },
		{ m_tb->m_label,   LabelType::LABEL   }
	};
	for (auto &pair : m_button_type_map) {
		connect(pair.first, &QAbstractButton::clicked, [this, pair]() {
			m_cc->createLabel(m_styles->getStyle(pair.second));
		});
	}
	connect(m_tb->m_image, &QAbstractButton::clicked, [this]() {
		// open up a file dialog
		m_cc->createImage(QPixmap("assets/karnak.jpg"));
	});
	connect(m_tb->m_delete, &QAbstractButton::clicked, [this]() {
		m_cc->removeItems();
	});

	connect(m_tb->m_bold, &QAbstractButton::clicked, m_cc, &CanvasControl::toggleBold);
	connect(m_tb->m_italicize, &QAbstractButton::clicked, m_cc, &CanvasControl::toggleItalic);
	connect(m_tb->m_strikeout, &QAbstractButton::clicked, m_cc, &CanvasControl::toggleStrikeOut);
	connect(m_tb->m_underline, &QAbstractButton::clicked, m_cc, &CanvasControl::toggleUnderline);

	connect(m_tb->m_bullet, &QAbstractButton::clicked, m_cc, &CanvasControl::listBullet);
	connect(m_tb->m_number, &QAbstractButton::clicked, m_cc, &CanvasControl::listOrdered);

	connect(m_tb->m_background, &QAbstractButton::clicked, this, [this]() {
		QColor c = QColorDialog::getColor(m_cc->allBackgroundColor(), this,
			"Background Color", QColorDialog::ShowAlphaChannel);
		if (c.isValid()) m_cc->setBackgroundColor(c);
	});
	connect(m_tb->m_clear_background, &QAbstractButton::clicked,
		m_cc, &CanvasControl::clearBackground);
	connect(m_tb->m_foreground, &QAbstractButton::clicked, [this]() {
		QColor c = QColorDialog::getColor(m_cc->allTextColor(), this, "Text Color");
		if (c.isValid()) m_cc->setTextColor(c);
	});
	connect(m_tb->m_border, &QAbstractButton::clicked, [this]() {
		QColor c = QColorDialog::getColor(m_cc->allBorderColor(), this, "Border Color");
		if (c.isValid()) {
			m_cc->setBorderColor(c);
		}
	});
	connect(m_tb->m_border_width, &QAbstractSpinBox::editingFinished,
		[this]() {
		m_cc->setBorderWidth(m_tb->m_border_width->value());
	});
	connect(m_tb->m_clear_border, &QAbstractButton::clicked,
		m_cc, &CanvasControl::clearBorder);

	//connect(m_tb->m_border_width, QOverload<int>::of(&QSpinBox::valueChanged),
	//	[this](int value) {
	//	m_cc->setBorderWidth(value);
	//});

	std::pair<QAbstractButton*, Qt::AlignmentFlag> align_map[] = {
		{ m_tb->m_halign_left, Qt::AlignLeft },
		{ m_tb->m_halign_center, Qt::AlignCenter },
		{ m_tb->m_halign_right, Qt::AlignRight },
		{ m_tb->m_halign_justify, Qt::AlignJustify }
	};
	for (auto pair : align_map) {
		connect(pair.first, &QAbstractButton::clicked, m_cc, [this, pair]() {
			m_cc->setHAlign(pair.second);
		});
	}

	std::pair<QAbstractButton*, Qt::AlignmentFlag> valign_map[] = {
		{ m_tb->m_valign_top, Qt::AlignTop },
		{ m_tb->m_valign_center, Qt::AlignVCenter },
		{ m_tb->m_valign_bottom, Qt::AlignBottom }
	};
	for (auto pair : valign_map) {
		connect(pair.first, &QAbstractButton::clicked, m_cc, [this, pair]() {
			m_cc->setVAlign(pair.second);
		});
	}

	// font begin end
	connect(m_tb, &CanvasToolBar::sFont, this, [this](const QFont &f) {
		m_cc->setFont(f.family());
	});
	connect(m_tb, &CanvasToolBar::sTextSize, this, [this](int size) {
		m_cc->setSize(size);
	});
	connect(m_tb, &CanvasToolBar::sLabelType, this, [this](LabelType type) {
		m_cc->setStyle(m_styles->getStyle(type));
	});

	// changes -> toolbar
	connect(m_cc, &CanvasControl::sAnyChange, this, [this]() {
		updateToolBar();
	});


	setStyles(m_default_styles.get()); // stuff needs to exist
}

void CanvasEditor::setScene(CanvasScene * scene)
{
	m_container->setScene(scene);
	m_cc->setScene(scene);
	if (scene == nullptr) {
		hide();
	}
	else {
		show();
		updateToolBar();
	}
}

void CanvasEditor::setStack(ICommandStack * stack)
{
	m_cc->setStack(stack);
}

void CanvasEditor::showToolBar(bool show)
{
	m_internal_window->setVisible(show);
}

void CanvasEditor::setStyles(LabelStyleGroup *styles)
{
	m_styles = styles;
	if (styles != m_default_styles.get()) m_default_styles.reset(nullptr);
	// apply to tool buttons
	applyStylesToButtons();
}

void CanvasEditor::applyStylesToButtons() {
	for (auto pair : m_button_type_map) {
		QAbstractButton *button = pair.first;
		LabelStyle *style = m_styles->getStyle(pair.second);
		style->applyToWidget(button, false);
	}
	FrameStyle *fs = m_styles->getImageStyle();
	fs->applyToWidget(m_tb->m_image);
}

void CanvasEditor::setEditable(bool editable)
{
	m_container->setEditable(editable);
	m_internal_window->setVisible(editable);
}

CanvasControl * CanvasEditor::control()
{
	return m_cc;
}

CanvasContainer * CanvasEditor::container()
{
	return m_container;
}

QMainWindow * CanvasEditor::internalWindow()
{
	return m_internal_window;
}

void CanvasEditor::debug()
{
	m_container->debug();
}

void CanvasEditor::updateToolBar()
{
	// font
	QFont f;
	QString ff = m_cc->allFont();
	f.setFamily(ff);
	m_tb->m_font->setCurrentFont(ff);

	// font size
	int size = m_cc->allFontSize();
	QString size_text = size < 0 ? "" : QString::number(size);
	m_tb->m_font_size->setCurrentText(size_text);

	// label type
	LabelType type = m_cc->allLabelType();
	QString type_text;
	if (type == LabelType::NONE) {
		type_text = "";
	} else {
		type_text = LabelTypeNames[type];
	}
	m_tb->m_style->setCurrentText(type_text);

	// border width
	int border = m_cc->allBorderSize();
	m_tb->m_border_width->setValue(border);
}
