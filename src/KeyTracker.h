#ifndef KEYTRACKER_H
#define KEYTRACKER_H

#include <QObject>
#include <QDebug>
#include <set>
#include <QKeyEvent>

// Qt filter object that keeps track of key event state
// this is basically a fancy std::set
class KeyTracker : public QObject {
public:
	// install this on a widget to track keys
	bool eventFilter(QObject *obj, QEvent *e);

	// check if a key is pressed
	bool keyPressed(int);
	void debug();

private:
	std::set<int> m_keymap;
};
#endif