#include "TimeManager.h"

#include "ModelGroup.h"
#include "Model.h"
#include "VSimApp.h"
#include "VSimRoot.h"

TimeManager::TimeManager(VSimApp *app, QObject *parent)
	: QObject(parent),
	m_app(app)
{
	connect(m_app, &VSimApp::sAboutToReset, this, [this]() {

	});
	connect(m_app, &VSimApp::sReset, this, [this]() {
		ModelGroup *models = m_app->getRoot()->models();
	});
}

int TimeManager::year() const
{
	return m_year;
}

void TimeManager::setYear(int year)
{
	m_year = year;
	emit sYearChanged(year);
}

bool TimeManager::timeEnabled() const
{
	return m_enabled;
}

void TimeManager::enableTime(bool enable)
{
	m_enabled = enable;
	emit sTimeEnableChanged(enable);
}

std::set<int> TimeManager::keyYears() const
{
	return m_keys;
}

void TimeManager::setKeyYears(const std::set<int> &keys)
{
	m_keys = keys;
	emit sKeysChanged();
}