#ifndef KEYTRACKER_H
#define KEYTRACKER_H

#include <QObject>
#include <QDebug>
#include <set>
#include <QKeyEvent>
#include <QMouseEvent>

// Qt filter object that keeps track of key event state
// this is basically a fancy std::set
class KeyTracker : public QObject {
public:
	// install this on a widget to track keys
	bool eventFilter(QObject *obj, QEvent *e);

	// check if a key is pressed
	bool keyPressed(int);

	// check if a mouse key is pressed
	Qt::MouseButtons mouseButtons();
	bool buttonPressed(Qt::MouseButton);

	void debug();

private:
	std::set<int> m_keymap;
	Qt::MouseButtons m_mouse_buttons;
};
#endif