#include "KeyTracker.h"

bool KeyTracker::eventFilter(QObject * obj, QEvent * e)
{
	if (e->type() == QEvent::KeyPress) {
		QKeyEvent *ke = (QKeyEvent*)e;
		m_keymap.insert(ke->key());
	}
	else if (e->type() == QEvent::KeyRelease) {
		QKeyEvent *ke = (QKeyEvent*)e;
		m_keymap.erase(ke->key());
	}
	return false; // always forward the event
}

bool KeyTracker::keyPressed(int key)
{
	return (m_keymap.find(key) != m_keymap.end());
}

void KeyTracker::debug() {
	qDebug() << "Debugging key tracker";
	for (auto i : m_keymap) {
		qDebug() << "key" << QKeySequence(i);
	}
}