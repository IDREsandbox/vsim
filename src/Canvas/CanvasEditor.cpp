#include "CanvasEditor.h"
#include "CanvasEditor.h"
#include "CanvasEditor.h"

#include <QDebug>
#include <QResizeEvent>
#include <QtGlobal>
#include <QApplication>
#include <QLineEdit>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

#include "Canvas/CanvasContainer.h"
#include "Canvas/CanvasScene.h"
#include "Canvas/CanvasToolBar.h"
#include "Canvas/CanvasControl.h"
#include "Canvas/LabelStyleGroup.h"
#include "Canvas/LabelStyle.h"
#include "Canvas/ChildMaskFilter.h"
#include "Canvas/StyleSettingsDialog.h"

CanvasEditor::CanvasEditor(QWidget * parent)
	: QWidget(parent),
	m_styles(nullptr),
	m_scene(nullptr)
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

	auto *mask_filter = new ChildMaskFilter(this);
	m_internal_window = new CanvasWindow(this);
	layout->addWidget(m_internal_window, 0, 0);
	m_internal_window->show();
	m_internal_window->installEventFilter(mask_filter);

	m_tb = new CanvasToolBar(m_internal_window);
	m_internal_window->addToolBar(Qt::ToolBarArea::LeftToolBarArea, m_tb);

	m_cc = new CanvasControl(this);

	a_delete = new QAction(this);
	a_delete->setText("Delete");
	a_delete->setShortcut(QKeySequence(Qt::Key_Delete));
	m_tb->m_delete->setDefaultAction(a_delete);;

	// styling
	QFile file(QCoreApplication::applicationDirPath() + "/assets/darkstyle.qss");
	file.open(QFile::ReadOnly);
	QString dark_style = QLatin1String(file.readAll());
	internalWindow()->setStyleSheet(dark_style);

	// toolbar -> control

	m_button_type_map = {
		{ m_tb->m_header1, LabelType::HEADER1 },
		{ m_tb->m_header2, LabelType::HEADER2 },
		{ m_tb->m_body,    LabelType::BODY    },
		{ m_tb->m_label,   LabelType::LABEL   }
	};
	for (auto &pair : m_button_type_map) {
		connect(pair.first, &QAbstractButton::clicked, [this, pair]() {
			m_cc->createLabelCommand(m_styles->getStyle(pair.second));
		});
	}
	connect(m_tb->m_image, &QAbstractButton::clicked, [this]() {
		// open up a file dialog
		QString result = QFileDialog::getOpenFileName(nullptr,
			"Open Image", m_last_image_dir,
			"Image Files (*.png;*.jpg;*.bmp);;All Files (*)");
		if (result.isNull()) return;

		QFileInfo info(result);
		m_last_image_dir = info.absoluteDir().path();

		// try to read image
		QPixmap pm(result);
		if (pm.isNull()) {
			// error
			QMessageBox::critical(nullptr,
				"Image Load Error",
				"Image failed to load.",
				QMessageBox::Ok);
			return;
		}

		m_cc->createImageCommand(pm, m_styles->getImageStyle());
	});
	connect(a_delete, &QAction::triggered, [this]() {
		m_cc->removeItems();
	});

	connect(m_tb->m_bold, &QAbstractButton::clicked, m_cc, &CanvasControl::toggleBold);
	connect(m_tb->m_italicize, &QAbstractButton::clicked, m_cc, &CanvasControl::toggleItalic);
	connect(m_tb->m_strikeout, &QAbstractButton::clicked, m_cc, &CanvasControl::toggleStrikeOut);
	connect(m_tb->m_underline, &QAbstractButton::clicked, m_cc, &CanvasControl::toggleUnderline);

	connect(m_tb->m_link, &QAbstractButton::clicked, this, [this]() {
		// must be selecting a label
		if (!m_cc->canSetLink()) return;

		m_cc->selectLink();
		bool edit_link = !m_cc->currentLink().isNull();

		bool ok;
		QString result = QInputDialog::getText(this,
			edit_link ? "Edit Link" : "Insert Link",
			"url",
			QLineEdit::Normal,
			edit_link ? m_cc->currentLink() : "",
			&ok);

		if (!ok) return;
		m_cc->setLink(result);
	});
	connect(m_tb->m_link_off, &QAbstractButton::clicked, this, [this]() {
		// try to select full link, unless its the edge
		bool edge = m_cc->linkEdge();
		if (!edge) {
			m_cc->selectLink();
		}
		m_cc->setLink("");
	});

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

	connect(m_tb->m_done, &QAbstractButton::clicked, this, &CanvasEditor::sDone);

	// styles button
	connect(m_tb->m_edit_styles, &QAbstractButton::clicked, this, &CanvasEditor::editStyles);
	//connect(m_tb->m_edit_styles, &QAbstractButton::clicked, this, &CanvasEditor::sEditStyles);

	setStyles(m_default_styles.get()); // stuff needs to exist
}

CanvasScene *CanvasEditor::scene() const
{
	return m_container->scene();
}

void CanvasEditor::setScene(CanvasScene * scene)
{
	m_scene = scene;
	m_container->setScene(scene);
	m_cc->setScene(scene);
	if (scene == nullptr) {
		hide();
	}
	else {
		m_scene->setEditable(m_editing_enabled);
		show();
		updateToolBar();
	}
}

void CanvasEditor::setStack(ICommandStack * stack)
{
	m_cc->setStack(stack);
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
	m_editing_enabled = editable;
	if (m_scene) {
		m_scene->setEditable(editable);
	}
	m_container->setEditable(editable);
	m_internal_window->setVisible(editable);
	updateToolBar();
}

bool CanvasEditor::isEditable() const
{
	return m_editing_enabled;
}

void CanvasEditor::editStyles()
{
	StyleSettingsDialog dlg;
	dlg.setStyles(m_styles);
	connect(&dlg, &StyleSettingsDialog::sApplied, this,
		&CanvasEditor::onStylesChanged);
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}
}

CanvasControl * CanvasEditor::control()
{
	return m_cc;
}

CanvasContainer * CanvasEditor::container()
{
	return m_container;
}

CanvasWindow *CanvasEditor::internalWindow()
{
	return m_internal_window;
}

void CanvasEditor::debug()
{
	m_container->debug();
}

void CanvasEditor::hideEvent(QHideEvent *e) {
	// hide toolbar
	//toolbar
	updateToolBar();
}

void CanvasEditor::showEvent(QShowEvent *e) {
	//m_tb->show();
	updateToolBar();
}

void CanvasEditor::updateToolBar()
{
	// if we're hidden or not editable then hide
	if (!isVisible() || !isEditable()) {
		m_tb->hide();
		return;
	}
	m_tb->show();
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
	m_tb->m_style->setCurrentText(LabelTypeNames[type]);

	// border width
	int border = m_cc->allBorderSize();
	m_tb->m_border_width->setValue(border);
}

void CanvasEditor::onStylesChanged()
{
	applyStylesToButtons();
}

CanvasWindow::CanvasWindow(QWidget *parent)
	: QMainWindow(parent)
{
	setWindowFlags(0);
	setAttribute(Qt::WA_NoBackground);
	setContextMenuPolicy(
		Qt::NoContextMenu
	);
	setObjectName("transparent");
	setStyleSheet("#transparent{background: transparent;}");
	// this style gets overriden by darkstyle
	// #transparent is also defined in darkstyle

	// manual filtering?
	//auto *filter = new ChildMaskFilter(this);
	//installEventFilter(filter);
}

void CanvasWindow::calcMask() {
	auto cr = childrenRegion();
	if (cr.isNull()) {
		setAttribute(Qt::WA_TransparentForMouseEvents);
		//hide();
	}
	else {
		setAttribute(Qt::WA_TransparentForMouseEvents, false);
		setMask(cr);
		//show();
	}
	//emit sMaskChanged(cr);
}


bool CanvasWindow::event(QEvent *e) {
	QEvent::Type type = e->type();
	switch (type) {
	case QEvent::LayoutRequest:
	case QEvent::Show:
	case QEvent::Hide:
		calcMask();
	}
	return QMainWindow::event(e);
}

void CanvasWindow::resizeEvent(QResizeEvent *e) {
	QMainWindow::resizeEvent(e);
	calcMask();
}
