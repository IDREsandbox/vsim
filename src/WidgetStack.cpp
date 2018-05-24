#include "WidgetStack.h"

#include <QMouseEvent>
#include <QApplication>
#include <QDebug>

WidgetStack::WidgetStack(QWidget * parent)
	: QWidget(parent),
	m_last_event(nullptr),
	m_last_obj(nullptr),
	m_iterating(false),
	m_fell_through(false)
{
	m_layout = new QGridLayout(this);
}

void WidgetStack::push(QWidget * w)
{
	m_stack.push_back(w);
	w->setParent(this);
	w->raise();
	m_layout->addWidget(w, 0, 0);
	w->installEventFilter(new Thief(this));
}

bool WidgetStack::event(QEvent * e)
{
	QEvent::Type type = e->type();

	switch (type) {
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
	case QEvent::MouseButtonDblClick:
	case QEvent::Wheel:
		{
		if (m_iterating) {
			// getting here
			// means we should keep iterating down in that for loop
			m_fell_through = true;

			// don't pass simulated event to parent
			// otherwise we're multiplying event
			e->accept(); 
			return true;

		}
		auto *me = static_cast<QMouseEvent*>(e);


		qDebug() << "event from top" << m_last_obj;


		// indexof
		auto it = std::find(m_stack.begin(), m_stack.end(), m_last_obj);

		if (it == m_stack.end()) {
			return false;
		}
		int index = std::distance(m_stack.begin(), it);
		if (index == 0) {
			return false;
		}

		// send the event to everyone else, from top to bottom
		// to avoid multiplying events set m_iterating flag
		m_iterating = true;
		for (int i = index - 1; i >= 0; i--) {
			QWidget *wnext = m_stack[i];

			if (!wnext->isVisible()) continue;

			// recursive childAt to get the topmost visible child
			QWidget *current = this; // never null
			QWidget *next = wnext; // next candidate
			QPoint gpos = me->globalPos(); // easiest mapping
			while (next != nullptr && next->isVisible()) {
				current = next;
				next = current->childAt(current->mapFromGlobal(gpos));

				// if there is a next child
				// and child is visible
				// then we dig deeper
			}
			qDebug() << "sending to" << i << current;
			QMouseEvent me2(*me);
			me2.setLocalPos(current->mapFromGlobal(gpos));
			m_fell_through = false;
			bool result = QApplication::sendEvent(current, &me2);
			// result is always true, that's just how QWidget works

			// didn't fall through? -> we're done
			if (!m_fell_through) {
				qDebug() << "taken";
				// event was taken by current
				m_iterating = false;
				return true;
			}

			// did fall through -> keep iterating
			// continue;
		}
		m_iterating = false;

		// done
		return QWidget::event(e);
		}
	};
	return QWidget::event(e);
}

WidgetStack::Thief::Thief(WidgetStack *boss)
	: QObject(boss),
	m_boss(boss)
{
}

bool WidgetStack::Thief::eventFilter(QObject *obj, QEvent *e)
{
	m_boss->m_last_event = e;
	m_boss->m_last_obj = obj;
	return false;
}