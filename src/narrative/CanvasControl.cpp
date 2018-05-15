#include "narrative/CanvasControl.h"
#include "narrative/CanvasScene.h"
#include "narrative/TextEdit.h"
#include "ICommandStack.h"
#include "SimpleCommandStack.h"

#include <QTextDocument>
#include <QDebug>

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
			auto *sc = new CanvasSelectCommand(m_scene, { label });
			auto *dc = new DocumentEditWrapperCommand(label->document(), sc);
			m_stack->push(sc);
		}
	});

	connect(m_scene, &CanvasScene::sRectsTransformed, this,
		[this](const std::map<CanvasItem*, QRectF> &old_rects,
			const std::map<CanvasItem*, QRectF> &new_rects) {
		std::set<CanvasItem*> items;
		for (const auto &pair : old_rects) {
			items.insert(pair.first);
		}
		auto *sc = new CanvasSelectCommand(m_scene, items);
		auto *tc = new TransformCanvasItemsCommand(old_rects, new_rects, sc);
		m_stack->push(sc);
	});

}

void CanvasControl::setStack(ICommandStack * stack)
{
	m_default_stack->stack()->clear();
	m_stack = stack;
}

void CanvasControl::createLabel(LabelType type)
{
	auto item = std::make_shared<CanvasLabel>();
	item->setVAlign(Qt::AlignCenter);
	item->document()->setPlainText("helloooo\nWORLd");
	item->setRect(QRectF(0, 0, .2, .2));
	item->setBrush(QBrush(QColor(255, 0, 0, 100)));
	item->setEditable(true);

	auto *sc = new CanvasSelectCommand(m_scene, { item.get() }, Command::ON_REDO);
	auto *cc = new AddRemoveItemCommand(m_scene, item, true, sc);
	m_stack->push(sc);
}

void CanvasControl::createImage(QPixmap pixmap)
{
	auto item = std::make_shared<CanvasImage>();
	item->setPixmap(pixmap);
	item->setEditable(true);

	auto *sc = new CanvasSelectCommand(m_scene, { item.get() }, Command::ON_REDO);
	auto *cc = new AddRemoveItemCommand(m_scene, item, true, sc);
	m_stack->push(sc);

	item->initSize();
}

void CanvasControl::removeItems()
{
	multiEdit([scene = m_scene](CanvasItem *item, QUndoCommand *parent) {
		auto shared = scene->toShared(item);
		new AddRemoveItemCommand(scene, shared, false, parent);
	});
}

void CanvasControl::setBorderWidth(int width)
{
	multiEdit([width](CanvasItem *item, QUndoCommand *parent) {
		new CanvasLabel::SetBorderWidthCommand(item, width, parent);
	});
}

void CanvasControl::setBorderColor(const QColor & color)
{
	multiEdit([color](CanvasItem *item, QUndoCommand *parent) {
		new CanvasLabel::SetBorderColorCommand(item, color, parent);
	});
}

void CanvasControl::setBackgroundColor(const QColor & color)
{
	multiEdit([color](CanvasItem *item, QUndoCommand *parent) {
		new CanvasLabel::SetBackgroundCommand(item, color, parent);
	});
}

void CanvasControl::setVAlign(Qt::Alignment al)
{
	if (!anyText()) return;

	multiEdit([al](CanvasItem *item, QUndoCommand *parent) {
		auto *label = dynamic_cast<CanvasLabel*>(item);
		if (!label) return;
		new CanvasLabel::SetVAlignCommand(label, al, parent);
	});
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

void CanvasControl::setStyle(LabelType type)
{
	// hmm
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

void CanvasControl::mergeCharFormat(const QTextCharFormat &fmt) {
	mergeCursorOp([&](QTextCursor cursor) {
		cursor.mergeCharFormat(fmt);
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
	return (multiText().size() > 0)
		|| (subText() != nullptr);
}

// selecting 1+ text items
std::set<CanvasLabel*> CanvasControl::multiText() const
{
	auto sel = m_scene->getSelected();
	std::set<CanvasLabel*> out;
	for (CanvasItem *item : sel) {
		auto *label = dynamic_cast<CanvasLabel*>(item);
		if (!label) return {};
		out.insert(label);
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
qreal CanvasControl::allSize() const
{
	return 0.0;
}
//bool CanvasControl::allBold() const
//{
//	return formatTest([](const QTextCharFormat &fmt)->bool {
//		return fmt.fontWeight() >= QFont::Bold;
//	});
//}
//bool CanvasControl::allBold() const
//{
//	return formatTest([](const QTextCharFormat &fmt)->bool {
//		return fmt.fontWeight() >= QFont::Bold;
//	});
//}

void CanvasControl::beginWrapTextCommands()
{
	m_text_command_bundle = new QUndoCommand();
	m_wrapping_text_commands = true;
}

void CanvasControl::endWrapTextCommands()
{
	m_stack->push(m_text_command_bundle);
	m_wrapping_text_commands = false;
	m_text_command_bundle = nullptr;
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
	m_doc->redo();
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