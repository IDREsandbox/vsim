#include "KeyTracker.h"

bool KeyTracker::eventFilter(QObject * obj, QEvent * e)
{
	QEvent::Type type = e->type();
	QKeyEvent *ke;
	QMouseEvent *me;

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
		qDebug() << "mouse buttons" << m_mouse_buttons;
	case QEvent::MouseButtonRelease:
		me = (QMouseEvent*)e;
		m_mouse_buttons = me->buttons();
		qDebug() << "mouse buttons" << m_mouse_buttons;
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
	qDebug() << "Debugging key tracker";
	for (auto i : m_keymap) {
		qDebug() << "key" << QKeySequence(i);
	}
	qDebug() << "mouse" << m_mouse_buttons;
}

Qt::MouseButtons KeyTracker::mouseButtons()
{
	return m_mouse_buttons;
}

bool KeyTracker::buttonPressed(Qt::MouseButton button)
{
	return m_mouse_buttons | button;
}
