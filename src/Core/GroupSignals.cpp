#include "GroupSignals.h"

GroupSignals::GroupSignals(QObject *parent)
	: QObject(parent),
	m_restricted(false)
{
}

bool GroupSignals::restrictedToCurrent() const
{
	return m_restricted;
}

void GroupSignals::setRestrictToCurrent(bool restrict)
{
	m_restricted = restrict;
	emit sRestrictToCurrentChanged(restrict);
}