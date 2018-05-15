#ifndef CANVASCONTROL_H
#define CANVASCONTROL_H

#include <QObject>
#include <map>
#include <set>
#include <QRectF>
#include <QUndoStack>
#include <QTextCharFormat>
#include <QTextDocument>
#include <memory>
#include "narrative/LabelType.h"
#include "Command.h"

class ICommandStack;
class SimpleCommandStack;
class CanvasLabel;
class CanvasItem;
class CanvasScene;

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
	void createLabel(LabelType type);
	void createImage(QPixmap pixmap);
	void removeItems();

	// all
	void setBorderWidth(int width);
	void setBorderColor(const QColor &color);
	void setBackgroundColor(const QColor &color);

	// text only
	void setVAlign(Qt::Alignment al);
	void setFont(const QString &family);
	void setHAlign(Qt::Alignment al);
	void setStyle(LabelType type);
	void setSize(qreal size);

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
	std::set<CanvasLabel*> multiText() const;
	// selecting text within a single item
	CanvasLabel *subText() const;

	// is the selection allX?
	bool allBold() const;
	bool allUnderline() const;
	bool allStrikeOut() const;
	bool allItalic() const;
	qreal allSize() const; //

signals: // change signals
	//qreal allSizeChanged();

public: // misc, internal stuff
	void beginWrapTextCommands(); // delays edit commands for bundling
	void endWrapTextCommands(); // pushes command

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
		auto labels = multiText();
		for (auto *label : labels) {
			bool ok = fmt_test(label->textCursor().charFormat());
			if (!ok) return false;
		}
		return true;
	}

	// Performs a textCursor operation on relevant selected text
	// Wraps changes with wrapTextCommand
	// signature: void(*)(QTextCursor cursor)
	template <typename T>
	void mergeCursorOp(T func) {
		CanvasLabel *label = subText();
		if (label) {
			QTextCursor c = label->textCursor();
			func(c);
			return;
		}
		std::set<CanvasLabel*> labels = multiText();
		if (labels.size() > 0) {
			beginWrapTextCommands();
			for (CanvasLabel *label : labels) {
				QTextCursor c = label->textCursor();
				c.select(QTextCursor::SelectionType::Document);
				func(c);
			}
			endWrapTextCommands();
		}
	}

	// Pushes a multi command, saves selection
	// signature: void(*)(CanvasItem *item, QUndoCommand *parent)
	template <typename T>
	void multiEdit(T func) {
		auto sel = m_scene->getSelected();
		auto *sc = new CanvasSelectCommand(m_scene, sel);
		for (CanvasItem *item : sel) {
			func(item, sc);
		}
		m_stack->push(sc);
	}

public:
	QAction *a_delete;

	QAction *a_header1;
	QAction *a_header2;
	QAction *a_label;
	QAction *a_body;
	QAction *a_image;

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