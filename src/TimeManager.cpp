#include "TimeManager.h"

#include "ModelGroup.h"
#include "Model.h"
#include "VSimApp.h"
#include "VSimRoot.h"

TimeManager::TimeManager(VSimApp *app, QObject *parent)
	: QObject(parent),
	m_app(app),
	m_models(nullptr)
{
	connect(m_app, &VSimApp::sAboutToReset, this, [this]() {
	});
	connect(m_app, &VSimApp::sReset, this, [this]() {
		m_models = m_app->getRoot()->models();

		// init
		if (m_keys.size() > 0) {
			setYear(*m_keys.begin());
		}
		else {
			setYear(1);
		}
		enableTime(true);
		setKeyYears(m_models->getKeyYears());

		// connect
		connect(m_models, &ModelGroup::sKeysChanged, this, [this]() {
			setKeyYears(m_models->getKeyYears());
		});
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
	if (m_models) m_models->setYear(year);
}

bool TimeManager::timeEnabled() const
{
	return m_enabled;
}

void TimeManager::enableTime(bool enable)
{
	m_enabled = enable;
	emit sTimeEnableChanged(enable);
	if (m_models) m_models->enableTime(enable);
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