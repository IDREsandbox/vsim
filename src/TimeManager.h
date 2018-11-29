﻿#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <QObject>
#include <set>

class VSimApp;
class ModelGroup;

class TimeManager : public QObject {
    Q_OBJECT
public:
    TimeManager(VSimApp *app, QObject *parent = nullptr);

    int year() const;
    void setYear(int year);

    bool timeEnabled() const;
    void enableTime(bool enable);

    std::set<int> keyYears() const;
    void setKeyYears(const std::set<int> &keys);

	//void setUseStartingYear(bool use);
	//bool useStartingYear() const;
	//void setStartingYear(int year);
	//int startingYear() const;

signals:
	void sTimeEnableChanged(bool enabled);
	void sYearChanged(int year);
	void sKeysChanged();

private:
	void gatherSettings();
	void extractSettings();

private:
    VSimApp *m_app;
    ModelGroup *m_models;
    int m_year;
    bool m_enabled;
    std::set<int> m_keys;
	bool m_use_starting_year;
	int m_starting_year;
};

#endif