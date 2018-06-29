#ifndef CANVASCONTROL_H
#define CANVASCONTROL_H

#include <QObject>
#include <map>
#include <set>
#include <QRectF>
#include <QUndoStack>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QTextCursor>
#include <memory>

#include "Canvas/LabelType.h"
#include "Core/Command.h"

class ICommandStack;
class SimpleCommandStack;
class CanvasLabel;
class CanvasItem;
class CanvasImage;
class CanvasScene;
class LabelStyle;
class FrameStyle;

//namespace Canvas {
//	void setBorderWidth(const std::vector<RectItem*> &items,
//		int width, ICommandStack *stack);
//	
//	void setFont(const std::vector<TextRect*> &items,
//		const QString &family, ICommandStack *stack);
//}

class CanvasControl : public QObject {
	Q_OBJECT;
public:
	CanvasControl(QObject *parent = nullptr);

	void setScene(CanvasScene *scene);
	void setStack(ICommandStack *stack);

public: // command wrappers
	//
	std::shared_ptr<CanvasLabel> createLabel(LabelStyle *);
	std::shared_ptr<CanvasLabel> createLabelCommand(LabelStyle *styles);
	std::shared_ptr<CanvasImage> createImage(QPixmap pixmap, FrameStyle *style = nullptr);
	std::shared_ptr<CanvasImage> createImageCommand(QPixmap pixmap, FrameStyle *style = nullptr);
	void removeItems();

	// all
	void setBorderWidth(int width);
	void setBorderColor(const QColor &color);
	void clearBorder();
	void setBackgroundColor(const QColor &color);
	void clearBackground();

	// text only
	void setFont(const QString &family);
	void setSize(qreal size);
	void setVAlign(Qt::Alignment al);
	void setHAlign(Qt::Alignment al);
	void setStyle(LabelStyle *style);
	void setTextColor(QColor c);

	// possible operations
	// 1. insert link (url, no selection)
	// 2. remove link (no url, selection)
	// 3. set link (url, selection)
	// 4. remove link no command (no url, no selection)
	void setLink(QString url);

	void toggleBold();
	void toggleUnderline();
	void toggleStrikeOut();
	void toggleItalic();

	// single text only
	void listBullet();
	void listOrdered();

public: // queries
	// can perform text operations
	bool anyText() const;
	// selecting 1+ text items
	std::set<CanvasLabel*> selectedLabels() const;
	// selecting text within a single item
	CanvasLabel *subText() const;

	bool selectionEmpty() const;
	QTextCursor currentCursor() const;

	bool canSetLink() const;
	QString currentLink() const;

	// if text cursor has a link, expands the cursor's
	// selection across the whole link
	void selectLink() const;
	static void cursorSelectLink(QTextCursor *c);
	// if single select cursor is on the edge, beginning or end
	bool linkEdge() const;

	//
	QColor allBackgroundColor() const;
	QColor allBorderColor() const;
	int allBorderSize() const; // < 0 if different
	QColor allTextColor() const;

	// is the selection allX?
	bool allBold() const;
	bool allUnderline() const;
	bool allStrikeOut() const;
	bool allItalic() const;
	int allFontSize() const; // < 0 if different
	QString allFont() const; // "" if different
	LabelType allLabelType() const; // NONE if different

signals: // change signals
	//qreal allSizeChanged();
	void sAnyChange();

public: // misc, internal stuff
	void applyFrameStyle(CanvasItem *item, FrameStyle *style);
	void applyLabelStyle(CanvasLabel *label, LabelStyle *style);
	QUndoCommand *createApplyLabelStyleCommand(CanvasLabel *label, LabelStyle *style,
		QUndoCommand *parent = nullptr);

	// delays edit commands and bundles them together
	// returns the allocated command
	void beginWrapTextCommands(QUndoCommand *parent = nullptr);
	QUndoCommand *endWrapTextCommands();

	//void onTextCommandAdded(CanvasLabel *label);

	// performs small or big merge
	void mergeCharFormat(const QTextCharFormat &fmt);
	void mergeBlockFormat(const QTextBlockFormat &fmt);

	static void insertList(QTextCursor cursor, QTextListFormat::Style style);

	// Performs a generic test on selection
	// Operates on both text subselection and multiple labels
	// Like a map-reduce when selecting multiple
	template <typename T>
	bool formatTest(T fmt_test) const {
		CanvasLabel *label = subText();
		if (label) {
			return fmt_test(label->textCursor().charFormat());
		}
		auto labels = selectedLabels();
		for (auto *label : labels) {
			bool ok = fmt_test(label->textCursor().charFormat());
			if (!ok) return false;
		}
		return true;
	}

	// Performs a textCursor operation on relevant selected text
	// Wraps changes with wrapTextCommand
	// signature: void(*)(QTextCursor &cursor)
	template <typename T>
	void mergeCursorOp(T func) {
		CanvasLabel *label = subText();
		if (label) {
			QTextCursor c = label->textCursor();
			func(c);
			label->setTextCursor(c);
			return;
		}
		std::set<CanvasLabel*> labels = selectedLabels();
		if (labels.size() > 0) {
			auto *cmd = m_stack->begin();
			cmd->setText("Edit Text");
			beginWrapTextCommands(cmd);
			for (CanvasLabel *label : labels) {
				QTextCursor c = label->textCursor();
				c.select(QTextCursor::SelectionType::Document);
				func(c);
			}
			endWrapTextCommands();
			m_stack->end();
		}
	}

	// Pushes a multi command, saves selection
	// signature: void(*)(CanvasItem *item, QUndoCommand *parent)
	template <typename T>
	void multiEdit(T func, const QString &name = "") {
		auto sel = m_scene->getSelected();
		auto *cmd = m_stack->begin();
		cmd->setText(name);
		auto *sc = new CanvasSelectCommand(m_scene, sel, Command::ON_BOTH, cmd);
		for (CanvasItem *item : sel) {
			func(item, cmd);
		}
		m_stack->end();
	}

private:
	CanvasScene *m_scene;
	SimpleCommandStack *m_default_stack;
	ICommandStack *m_stack;
	bool m_wrapping_text_commands;
	QUndoCommand *m_text_command_bundle;
};


class TransformCanvasItemsCommand : public QUndoCommand {
public:
	TransformCanvasItemsCommand(
		const std::map<CanvasItem*, QRectF> &old_rects,
		const std::map<CanvasItem*, QRectF> &new_rects,
		QUndoCommand *parent = nullptr
	);
	void undo();
	void redo();
private:
	std::map<CanvasItem*, QRectF> m_old;
	std::map<CanvasItem*, QRectF> m_new;
};

class DocumentEditWrapperCommand : public QUndoCommand {
public:
	DocumentEditWrapperCommand(QTextDocument *doc,
		QUndoCommand *parent = nullptr);
	void undo();
	void redo();
private:
	QTextDocument * m_doc;
};

class CanvasSelectCommand : public QUndoCommand
{
public:
	CanvasSelectCommand(CanvasScene *scene,
		const std::set<CanvasItem*> selection,
		Command::When when = Command::ON_BOTH,
		QUndoCommand *parent = nullptr);
	void undo();
	void redo();
private:
	CanvasScene * m_scene;
	std::set<CanvasItem*> m_selection;
	Command::When m_when;
};

// Command thats adds or removes items from a scene
// The two implementations are so similar I just stuck them together
class AddRemoveItemCommand : public QUndoCommand {
public:
	AddRemoveItemCommand(CanvasScene *scene,
		std::shared_ptr<CanvasItem> item,
		bool add,
		QUndoCommand *parent = nullptr);
	void undo();
	void redo();
	void act(bool add);
private:
	CanvasScene *m_scene;
	std::shared_ptr<CanvasItem> m_item;
	bool m_add;
};

#endif