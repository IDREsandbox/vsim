#ifndef KEYTRACKER_H
#define KEYTRACKER_H

#include <QObject>
#include <QDebug>
#include <set>
#include <QKeyEvent>
#include <QMouseEvent>
#include <unordered_set>

// Qt filter object that keeps track of key event state
// this is basically a fancy std::set
class KeyTracker : public QObject {
public:
	KeyTracker(QObject *parent = nullptr);

	// install this on a widget to track keys
	bool eventFilter(QObject *obj, QEvent *e);

	// check if a key is pressed
	bool keyPressed(int key);

	// check if a mouse key is pressed
	Qt::MouseButtons mouseButtons();
	bool mouseButton(Qt::MouseButton);

	// release all keys/mouse buttons
	// when a widget loses focus it doesn't necessarily get a release event,
	// allowing you to hang
	void releaseAll();

	void stealKeys(const QList<int> &list);

	void debug();

private:
	std::set<int> m_keymap;
	Qt::MouseButtons m_mouse_buttons;
	std::unordered_set<int> m_key_steal;
};
#endif