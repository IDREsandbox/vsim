#ifndef WIDGETSTACK_H
#define WIDGETSTACK_H

#include <QObject>
#include <QWidget>
#include <QGridLayout>

// Normally events pass from child to parent, and not between siblings.
// If you want to pass between siblings you have to use a mask.
// However, it's difficult to have automatic nested masks (ChildMaskFilter).
// This widget container allows events to pass through 'layers'
// Any event that isn't accepted in the top layer goes to the second,
// then the third, etc. etc.

// Use case:
// The main goal of this was to allow clicks through the middle spacer to end
// up on either the canvas or the OSGWidget without the use of masks.
// (0) OSGWidget <= (1) Canvas <= (2) Splitter <- middleSpacer <- toolbar

// FIXME:
// The whole method is super hacky spaghetti
// m_iterating, m_fell_through, m_last_obj, m_last_e are state,
// so if any event recursion goes on then we're toast.
// ex. if someone calls drag.exec() I have no clue what will happen

// EDIT:
// This has other problems that I didn't think about, so... back to using masks
// - cursor doesn't change currectly

class WidgetStack : public QWidget {
	Q_OBJECT;
public:
	WidgetStack(QWidget *parent = nullptr);

	// adds a widget to the top of the stack (top has event priority)
	// takes ownership of the widget
	void push(QWidget *w);

protected:
	bool event(QEvent *e) override;

private:
	// approaches
	// 1. apply event filter on every child
	// 2. keep a dummy widget on top of the others
	// 3. override QApplication::notify?

	// event filter approach
	class Thief : public QObject {
	public:
		Thief(WidgetStack *boss);
		bool eventFilter(QObject *obj, QEvent *e) override;
	private:
		WidgetStack * m_boss;
	};

	// dummy widget approach
	//class Thief2 : public QWidget {
	//};

private:
	std::vector<QWidget*> m_stack;
	QGridLayout *m_layout;
	QEvent *m_last_event;
	QObject *m_last_obj;
	bool m_iterating;
	bool m_fell_through;
};

#endif