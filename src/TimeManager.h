#ifndef TIMEMANAGER_H
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

signals:
	void sTimeEnableChanged(bool enabled);
	void sYearChanged(int year);
	void sKeysChanged();

private:
    VSimApp *m_app;
    ModelGroup *m_models;
    int m_year;
    bool m_enabled;
    std::set<int> m_keys;
};

#endif