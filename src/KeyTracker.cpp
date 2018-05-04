#include "KeyTracker.h"

KeyTracker::KeyTracker(QObject * parent)
	: QObject(parent)
{
}

bool KeyTracker::eventFilter(QObject * obj, QEvent * e)
{
	QEvent::Type type = e->type();
	QKeyEvent *ke;
	QMouseEvent *me;

	bool steal = false;

	switch (e->type()) {
	case QEvent::KeyPress:
		ke = (QKeyEvent*)e;
		m_keymap.insert(ke->key());
		break;
	case QEvent::KeyRelease:
		ke = (QKeyEvent*)e;
		m_keymap.erase(ke->key());
		break;
	case QEvent::MouseButtonPress:
		me = (QMouseEvent*)e;
		m_mouse_buttons = me->buttons();
	case QEvent::MouseButtonRelease:
		me = (QMouseEvent*)e;
		m_mouse_buttons = me->buttons();
		break;
	case QEvent::ShortcutOverride:
		ke = (QKeyEvent*)e;
		if (m_key_steal.find(ke->key()) != m_key_steal.end()) {
			// found in steal map
			ke->accept();
			return true;
		}
		break;
	default:
		break;
	}

	return false; // always forward the event
}

bool KeyTracker::keyPressed(int key)
{
	return (m_keymap.find(key) != m_keymap.end());
}

void KeyTracker::debug() {
	qInfo() << "Debugging key tracker";
	for (auto i : m_keymap) {
		qInfo() << "key" << QKeySequence(i);
	}
	qInfo() << "mouse" << m_mouse_buttons;
}

Qt::MouseButtons KeyTracker::mouseButtons()
{
	return m_mouse_buttons;
}

bool KeyTracker::mouseButton(Qt::MouseButton button)
{
	return m_mouse_buttons & button;
}

void KeyTracker::stealKeys(const QList<int>& list)
{
	m_key_steal.clear();
	for (auto key : list) {
		m_key_steal.insert(key);
	}
}
