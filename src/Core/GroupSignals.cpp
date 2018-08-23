#include "GroupSignals.h"

#include <QDebug>

GroupSignals::GroupSignals(QObject *parent)
	: QObject(parent),
	m_restricted(false)
{
}

GroupSignals & GroupSignals::operator=(const GroupSignals & other)
{
	setRestrictToCurrent(other.m_restricted);
	return *this;
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