#include "Canvas/CanvasControl.h"
#include "Canvas/CanvasScene.h"
#include "Core/ICommandStack.h"
#include "Core/SimpleCommandStack.h"
#include "Canvas/LabelStyle.h"

#include <QTextDocument>
#include <QTextCursor>
#include <QTextFrame>
#include <QDebug>
#include <numeric>
#include <QApplication>

CanvasControl::CanvasControl(QObject *parent)
	: QObject(parent),
	m_scene(nullptr),
	m_stack(nullptr),
	m_wrapping_text_commands(false),
	m_text_command_bundle(nullptr)
{
	m_default_stack = new SimpleCommandStack(this);
	m_stack = m_default_stack;
}

void CanvasControl::setScene(CanvasScene * scene)
{
	if (m_scene) disconnect(m_scene, 0, this, 0);
	m_scene = scene;
	if (scene == nullptr) return;
	connect(m_scene, &QObject::destroyed, this, [this]() {
		m_scene = nullptr;
	});

	connect(m_scene, &CanvasScene::sDocumentUndoCommandAdded, this,
		[this](CanvasLabel *label, QTextDocument *doc) {
		// if we're bunding commands then add to the bundle
		if (m_wrapping_text_commands) {
			new DocumentEditWrapperCommand(label->document(),
				m_text_command_bundle);
		}
		else {
			// TODO: does this need a selection wrapper?
			auto *cmd = m_stack->begin();
			cmd->setText("Edit Text");
			auto *sc = new CanvasSelectCommand(m_scene, { label }, Command::ON_BOTH, cmd);
			auto *dc = new DocumentEditWrapperCommand(label->document(), sc);
			m_stack->end();
		}
	});

	connect(m_scene, &CanvasScene::sRectsTransformed, this,
		[this](const std::map<CanvasItem*, QRectF> &old_rects,
			const std::map<CanvasItem*, QRectF> &new_rects) {
		std::set<CanvasItem*> items;
		for (const auto &pair : old_rects) {
			items.insert(pair.first);
		}
		auto *cmd = m_stack->begin();
		cmd->setText("Transform Items");
		auto *sc = new CanvasSelectCommand(m_scene, items, Command::ON_BOTH, cmd);
		auto *tc = new TransformCanvasItemsCommand(old_rects, new_rects, sc);
		m_stack->end();
	});

	connect(m_scene, &CanvasScene::changed, this,
		[this]() {
		emit sAnyChange();
	});
}

void CanvasControl::setStack(ICommandStack * stack)
{
	m_default_stack->stack()->clear();
	m_stack = stack;
}

std::shared_ptr<CanvasLabel> CanvasControl::createLabel(LabelStyle *style)
{
	auto item = std::make_shared<CanvasLabel>();
	item->setVAlign(Qt::AlignCenter);
	item->document()->setPlainText("New Label");
	item->setRect(QRectF(-.2, -.1, .4, .2));
	item->setBrush(QBrush(QColor(255, 0, 0, 100)));
	item->setEditable(true);
	item->setBorderAround(true);

	applyLabelStyle(item.get(), style);

	return item;
}

std::shared_ptr<CanvasLabel> CanvasControl::createLabelCommand(LabelStyle * style)
{
	auto item = createLabel(style);

	auto *cmd = m_stack->begin();
	cmd->setText("Create Label");
	auto *sc = new CanvasSelectCommand(m_scene, { item.get() }, Command::ON_REDO, cmd);
	auto *cc = new AddRemoveItemCommand(m_scene, item, true, sc);
	m_stack->end();

	return item;
}

std::shared_ptr<CanvasImage> CanvasControl::createImage(QPixmap pixmap, FrameStyle *style)
{
	auto item = std::make_shared<CanvasImage>();
	item->setPixmap(pixmap);
	item->setEditable(true);
	applyFrameStyle(item.get(), style);
	return item;
}

std::shared_ptr<CanvasImage> CanvasControl::createImageCommand(QPixmap pixmap, FrameStyle * style)
{
	auto item = createImage(pixmap, style);

	auto *cmd = m_stack->begin();
	cmd->setText("Create Image");
	auto *sc = new CanvasSelectCommand(m_scene, { item.get() }, Command::ON_REDO, cmd);
	auto *cc = new AddRemoveItemCommand(m_scene, item, true, sc);
	m_stack->end();

	item->initSize();
	return item;
}

void CanvasControl::removeItems()
{
	multiEdit([scene = m_scene](CanvasItem *item, QUndoCommand *parent) {
		auto shared = scene->toShared(item);
		new AddRemoveItemCommand(scene, shared, false, parent);
	}, "Remove Items");
}

void CanvasControl::setBorderWidth(int width)
{
	multiEdit([width](CanvasItem *item, QUndoCommand *parent) {
		new CanvasItem::SetHasBorderCommand(item, true, parent);
		new CanvasItem::SetBorderWidthCommand(item, width, parent);
	}, "Set Border Thickness");
}

void CanvasControl::setBorderColor(const QColor & color)
{
	multiEdit([color](CanvasItem *item, QUndoCommand *parent) {
		new CanvasItem::SetHasBorderCommand(item, true, parent);
		new CanvasItem::SetBorderColorCommand(item, color, parent);
	}, "Set Border Color");
}

void CanvasControl::clearBorder()
{
	multiEdit([](CanvasItem *item, QUndoCommand *parent) {
		new CanvasItem::SetHasBorderCommand(item, false, parent);
	}, "Clear Border");
}

void CanvasControl::setBackgroundColor(const QColor & color)
{
	multiEdit([color](CanvasItem *item, QUndoCommand *parent) {
		new CanvasLabel::SetBackgroundCommand(item, color, parent);
	}, "Set Background");
}

void CanvasControl::clearBackground()
{
	multiEdit([](CanvasItem *item, QUndoCommand *parent) {
		new CanvasLabel::SetBackgroundCommand(item, QColor(0,0,0,0), parent);
	}, "Clear Background");
}

void CanvasControl::setVAlign(Qt::Alignment al)
{
	if (!anyText()) return;

	multiEdit([al](CanvasItem *item, QUndoCommand *parent) {
		auto *label = dynamic_cast<CanvasLabel*>(item);
		if (!label) return;
		new CanvasLabel::SetVAlignCommand(label, al, parent);
	}, "Set VAlign");
}

void CanvasControl::setStyle(LabelStyle *style)
{
	if (!anyText()) return;

	multiEdit([this, style](CanvasItem *item, QUndoCommand *parent) {
		auto *label = dynamic_cast<CanvasLabel*>(item);
		if (!label) return;
		createApplyLabelStyleCommand(label, style, parent);
	}, "Set Style");
}

void CanvasControl::setTextColor(QColor c)
{
	QTextCharFormat fmt;
	c.setAlpha(255); // no transparent text
	fmt.setForeground(c);
	mergeCharFormat(fmt);
}

void CanvasControl::setLink(QString url)
{
	CanvasLabel *label = subText();
	if (!label) return;

	// full behavior spec

	// cursor inside a link
	// -> edit the link (call selectLink() first)
	// cursor inside no link
	// -> insert the link as text
	// selecting no link
	// -> set link for selection
	// selecting yes link
	// -> edit the link

	bool has_selection = !selectionEmpty();
	bool url_ok = !url.isEmpty();

	bool insert = !has_selection && url_ok;
	bool remove = has_selection && !url_ok;
	bool set = has_selection && url_ok;
	bool remove_simple = !has_selection && !url_ok;

	QTextCursor c = label->textCursor();

	if (remove_simple) {
		QTextCharFormat fmt = c.charFormat();
		fmt.setAnchor(false);
		fmt.setAnchorHref("");
		fmt.clearForeground();
		fmt.clearProperty(QTextFormat::TextUnderlineStyle);
		c.setCharFormat(fmt);
		label->setTextCursor(c);
		return;
	}

	auto *cmd = m_stack->begin();
	if (insert) cmd->setText("Insert Link");
	if (remove) cmd->setText("Remove Link");
	if (set) cmd->setText("Set Link");

	beginWrapTextCommands(cmd);

	if (insert || set) {
		QTextCharFormat new_fmt;
		new_fmt.setAnchor(true);
		new_fmt.setAnchorHref(url);
		new_fmt.setForeground(QApplication::palette().color(QPalette::Link));
		new_fmt.setFontUnderline(true);
		c.mergeCharFormat(new_fmt);
	}

	if (insert) {
		// insert new link
		c.insertText(url);
	}

	if (remove) {
		// clear old link
		// try to clear foreground, underline
		QTextCharFormat fmt = c.charFormat();
		fmt.setAnchor(false);
		fmt.setAnchorHref("");
		fmt.clearForeground();
		fmt.clearProperty(QTextFormat::TextUnderlineStyle);

		c.setCharFormat(fmt);
	}

	label->setTextCursor(c); // is this necessary?
	endWrapTextCommands();
	m_stack->end();

	return;
}

void CanvasControl::setFont(const QString & family)
{
	QTextCharFormat fmt;
	fmt.setFontFamily(family);
	mergeCharFormat(fmt);
}

void CanvasControl::setHAlign(Qt::Alignment al)
{
	QTextBlockFormat fmt;
	fmt.setAlignment(al);
	mergeBlockFormat(fmt);
}

void CanvasControl::setSize(qreal size)
{
	QTextCharFormat fmt;
	fmt.setFontPointSize(size);
	mergeCharFormat(fmt);
}

void CanvasControl::toggleBold()
{
	QTextCharFormat fmt;
	fmt.setFontWeight(allBold() ? QFont::Normal : QFont::Bold);
	mergeCharFormat(fmt);
}

void CanvasControl::toggleUnderline()
{
	QTextCharFormat fmt;
	fmt.setFontUnderline(!allUnderline());
	mergeCharFormat(fmt);
}

void CanvasControl::toggleStrikeOut()
{
	QTextCharFormat fmt;
	fmt.setFontStrikeOut(!allStrikeOut());
	mergeCharFormat(fmt);
}

void CanvasControl::toggleItalic()
{
	QTextCharFormat fmt;
	fmt.setFontItalic(!allItalic());
	mergeCharFormat(fmt);
}

void CanvasControl::listBullet()
{
	auto *label = subText();
	if (!label) return;
	insertList(label->textCursor(), QTextListFormat::Style::ListDisc);
}

void CanvasControl::listOrdered()
{
	auto *label = subText();
	if (!label) return;
	insertList(label->textCursor(), QTextListFormat::Style::ListDecimal);
}

void CanvasControl::applyFrameStyle(CanvasItem * item, FrameStyle * fs)
{
	if (fs == nullptr) return;
	QColor m_bg_color;
	item->setBrush(fs->m_bg_color);

	QPen p = item->pen();
	p.setBrush(fs->m_frame_color);
	p.setStyle(fs->m_has_frame ? Qt::SolidLine : Qt::NoPen);
	p.setWidthF(fs->m_frame_width / item->baseHeight());
	item->setPen(p);
}

void CanvasControl::applyLabelStyle(CanvasLabel * label, LabelStyle *style)
{
	if (style == nullptr) return;
	applyFrameStyle(label, style->frameStyle());
	label->setStyleType(style->getType());
	label->setVAlign(style->m_align);

	style->applyToDocument(label->document());
}

QUndoCommand *CanvasControl::createApplyLabelStyleCommand(CanvasLabel *label,
	LabelStyle *s, QUndoCommand * parent)
{
	QUndoCommand *cmd = new QUndoCommand(parent);

	// NONE type -> style is nullptr -> return here after setting type
	if (s == nullptr) {
		new CanvasLabel::SetStyleTypeCommand(label, LabelType::NONE, cmd);
		return cmd;
	}

	new CanvasLabel::SetStyleTypeCommand(label, s->getType(), cmd);

	auto *frame = s->frameStyle();

	// frame
	new CanvasItem::SetBackgroundCommand(label, frame->m_bg_color, cmd);
	new CanvasItem::SetBorderWidthCommand(label, frame->m_frame_width, cmd);
	new CanvasItem::SetBorderColorCommand(label, frame->m_frame_color, cmd);
	new CanvasItem::SetHasBorderCommand(label, frame->m_has_frame, cmd);

	// label
	new CanvasLabel::SetVAlignCommand(label, s->valign(), cmd);

	// text
	beginWrapTextCommands(cmd);
	s->applyToDocument(label->document());
	auto *textcmd = endWrapTextCommands();

	return cmd;
}

void CanvasControl::mergeCharFormat(const QTextCharFormat &fmt) {
	mergeCursorOp([&](QTextCursor &cursor) {
		cursor.mergeCharFormat(fmt);
		//cursor.mergeBlockCharFormat(fmt); //
	});
}

void CanvasControl::mergeBlockFormat(const QTextBlockFormat & fmt)
{
	mergeCursorOp([&](QTextCursor cursor) {
		cursor.mergeBlockFormat(fmt);
	});
}

void CanvasControl::insertList(QTextCursor cursor, QTextListFormat::Style style)
{
	cursor.beginEditBlock();

	QTextListFormat listFmt;
	listFmt.setStyle(style);
	cursor.createList(listFmt);

	cursor.endEditBlock();
}

// can perform text operations
bool CanvasControl::anyText() const
{
	return (selectedLabels().size() > 0)
		|| (subText() != nullptr);
}

// selecting 1+ text items
std::set<CanvasLabel*> CanvasControl::selectedLabels() const
{
	auto sel = m_scene->getSelected();
	std::set<CanvasLabel*> out;
	for (CanvasItem *item : sel) {
		auto *label = dynamic_cast<CanvasLabel*>(item);
		//if (!label) return {}; // reject if selecting something else?
		if (label) out.insert(label);
	}
	return out;
}

// selecting text within a single item
CanvasLabel *CanvasControl::subText() const
{
	auto sel = m_scene->getSelected();
	if (sel.size() != 1) {
		return nullptr;
	}
	auto *label = dynamic_cast<CanvasLabel*>(*sel.begin());
	if (!label) return nullptr;
	if (!label->hasFocus()) return nullptr;
	return label;
}

bool CanvasControl::selectionEmpty() const
{
	CanvasLabel *label = subText();
	if (!label) return true;
	return label->textCursor().selectedText().isEmpty();
}

bool CanvasControl::canSetLink() const
{
	return (subText() != nullptr);
}

QString CanvasControl::currentLink() const
{
	CanvasLabel *label = subText();
	if (!label) return QString();

	auto fmt = label->textCursor().charFormat();
	return fmt.anchorHref();
}

QTextCursor CanvasControl::currentCursor() const
{
	CanvasLabel *label = subText();
	if (!label) return QTextCursor();

	return label->textCursor();
}

void CanvasControl::selectLink() const
{
	CanvasLabel *label = subText();
	if (!label) return;

	QTextCursor c = label->textCursor();
	cursorSelectLink(&c);
	label->setTextCursor(c);
}

bool CanvasControl::linkEdge() const
{
	CanvasLabel *label = subText();
	if (!label) return true;

	QTextCursor c = label->textCursor();

	// if beginning or end the true
	if (c.atStart() || c.atEnd()) return true;

	auto fmt = c.charFormat();

	// check left edge:
	if (!fmt.isAnchor()) return true;

	// check right edge:
	//  move right one
	c.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::MoveAnchor, 1);
	fmt = c.charFormat();
	if (!fmt.isAnchor()) return true;

	return false;
}

bool CanvasControl::allBold() const
{
	return formatTest([](const QTextCharFormat &fmt)->bool {
		return fmt.fontWeight() >= QFont::Bold;
	});
}
bool CanvasControl::allUnderline() const
{
	return formatTest([](const QTextCharFormat &fmt)->bool {
		return fmt.fontUnderline();
	});
}
bool CanvasControl::allStrikeOut() const
{
	return formatTest([](const QTextCharFormat &fmt)->bool {
		return fmt.fontStrikeOut();
	});
}
bool CanvasControl::allItalic() const
{
	return formatTest([](const QTextCharFormat &fmt)->bool {
		return fmt.fontItalic();
	});
}
int CanvasControl::allFontSize() const
{
	auto labels = selectedLabels();
	return std::accumulate(labels.begin(), labels.end(), 0,
		[](int size, CanvasLabel *label) -> int {
		QTextCharFormat fmt = label->textCursor().charFormat();
		int x = std::lround(fmt.fontPointSize());
		// fmt.fontPointSize() might be 0 != font.pointSize()
		if (x == 0) {
			x = std::lround(fmt.font().pointSizeF());
		}

		if (size == 0) {
			return x;
		}
		if (x == size) {
			return size;
		}
		else {
			return -1;
		}
	});
}

QString CanvasControl::allFont() const
{
	auto labels = selectedLabels();
	bool first = true;
	QString s = std::accumulate(labels.begin(), labels.end(), QString(""),
		[&first](QString acc,  CanvasLabel *label) -> QString {
		QTextCharFormat fmt = label->textCursor().charFormat();
		QString ff = fmt.fontFamily();
		// apparently fmt.fontFamily() != font.family()
		// so use font.family() if ""
		if (ff.isEmpty()) {
			ff = fmt.font().family();
		}

		if (first) {
			return ff;
		}
		if (ff == acc) {
			return acc;
		}
		else {
			return "";
		}
	});
	return s;
}

void CanvasControl::cursorSelectLink(QTextCursor *cursor)
{
	if (cursor->isNull()) return;
	QTextCursor c = *cursor;

	int start = c.selectionStart();
	int end = c.selectionEnd();

	// if already selecting range then quit
	if (start != end) return;

	auto fmt = c.charFormat();
	bool is_anchor = fmt.isAnchor();
	QString href = fmt.anchorHref();

	// not a link, so nothing to select
	if (!is_anchor) return;
	if (href.isNull()) return;

	// does this character match links?
	auto acceptChar = [&](int pos) -> bool {
		QTextCursor tester(c.document());
		tester.setPosition(pos + 1);
		auto tfmt = tester.charFormat();
		if (!tfmt.isAnchor()) return false;
		if (tfmt.anchorHref() != href) return false;
		return true;
	};

	QTextDocument *doc = c.document();

	// scan left then right, accept if anchor and same href

	// (left - 1) tests the character to the left
	int left;
	for (left = start; left > 0; left--) {
		if (!acceptChar(left - 1)) break;
	}

	// QTextDocument::characterCount() is +1 for some reason
	// ex: "abc" -> 4, a text cursor at 4 is the end
	int right;
	for (right = end; right < doc->characterCount() - 1; right++) {
		if (!acceptChar(right)) break;
	}

	// select the range
	c.setPosition(left);
	c.setPosition(right, QTextCursor::MoveMode::KeepAnchor);
	*cursor = c;
}

QColor CanvasControl::allBackgroundColor() const
{
	// just returns the first one
	auto items = m_scene->getSelected();
	if (items.size() == 0) {
		return QColor(0, 0, 0);
	}
	return (*items.begin())->background();
}

QColor CanvasControl::allBorderColor() const
{
	// just returns the first one
	auto items = m_scene->getSelected();
	if (items.size() == 0) {
		return QColor(0, 0, 0);
	}
	return (*items.begin())->borderColor();
}

int CanvasControl::allBorderSize() const
{
	auto items = m_scene->getSelected();
	// different: returns the first one
	bool first = true;
	return std::accumulate(items.begin(), items.end(), -1,
		[&first](int acc, CanvasItem *item) -> int {
		int border = item->borderWidthPixels();
		if (first) { // initial
			first = false;
			return border;
		}
		else if (acc == border) {
			return acc;
		}
		else {
			return -1;
		}
	});
}

QColor CanvasControl::allTextColor() const
{
	// just returns the first one
	auto labels = selectedLabels();
	if (labels.size() == 0) {
		return QColor(0, 0, 0);
	}
	auto *label = (*labels.begin());
	auto fmt = label->textCursor().charFormat();
	
	return fmt.foreground().color();
}

LabelType CanvasControl::allLabelType() const
{
	auto labels = selectedLabels();

	bool first = true;
	LabelType t = std::accumulate(labels.begin(), labels.end(), LabelType::NONE,
		[&first](LabelType acc, CanvasLabel *label) -> LabelType {
		LabelType type = label->styleType();
		if (first) {
			first = false;
			return type;
		}
		else if (type == acc) {
			return acc;
		}
		else {
			return LabelType::NONE;
		}
	});
	return t;
}

void CanvasControl::beginWrapTextCommands(QUndoCommand *parent)
{
	m_text_command_bundle = new QUndoCommand(parent);
	m_wrapping_text_commands = true;
}

QUndoCommand *CanvasControl::endWrapTextCommands()
{
	auto *bundle = m_text_command_bundle;
	m_wrapping_text_commands = false;
	m_text_command_bundle = nullptr;
	return bundle;
}

TransformCanvasItemsCommand::TransformCanvasItemsCommand(
	const std::map<CanvasItem*, QRectF>& old_rects,
	const std::map<CanvasItem*, QRectF>& new_rects,
	QUndoCommand *parent)
	: QUndoCommand(parent),
	m_old(old_rects),
	m_new(new_rects)
{
}

void TransformCanvasItemsCommand::undo()
{
	for (const auto &pair : m_old) {
		auto *item = pair.first;
		QRectF rect = pair.second;
		item->setRect(rect);
	}
}

void TransformCanvasItemsCommand::redo()
{
	for (const auto &pair : m_new) {
		auto *item = pair.first;
		QRectF rect = pair.second;
		item->setRect(rect);
	}
}

DocumentEditWrapperCommand::DocumentEditWrapperCommand(QTextDocument *doc,
	QUndoCommand *parent)
	: QUndoCommand(parent),
	m_doc(doc)
{
}
void DocumentEditWrapperCommand::undo()
{
	m_doc->undo();
}
void DocumentEditWrapperCommand::redo()
{
	if (m_doc->isRedoAvailable()) {
		m_doc->redo();
	}
}

CanvasSelectCommand::CanvasSelectCommand(CanvasScene * scene,
	const std::set<CanvasItem*> selection,
	Command::When when,
	QUndoCommand * parent)
	: QUndoCommand(parent),
	m_scene(scene),
	m_selection(selection),
	m_when(when)
{
}

void CanvasSelectCommand::undo()
{
	QUndoCommand::undo();
	if (m_when & Command::ON_UNDO) {
		m_scene->setSelected(m_selection);
	}
}

void CanvasSelectCommand::redo()
{
	QUndoCommand::redo();
	if (m_when & Command::ON_REDO) {
		m_scene->setSelected(m_selection);
	}
}

AddRemoveItemCommand::AddRemoveItemCommand(CanvasScene * scene,
	std::shared_ptr<CanvasItem> item,
	bool add,
	QUndoCommand *parent)
	: QUndoCommand(parent),
	m_scene(scene),
	m_item(item),
	m_add(add)
{
}

void AddRemoveItemCommand::undo()
{
	act(!m_add);
}

void AddRemoveItemCommand::redo()
{
	act(m_add);
}

void AddRemoveItemCommand::act(bool add)
{
	if (add) {
		m_scene->addItem(m_item);
	}
	else {
		m_scene->removeItem(m_item.get());
	}
}