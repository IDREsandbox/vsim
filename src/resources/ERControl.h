#ifndef ERCONTROL_H
#define ERCONTROL_H

#include <QObject>
// manages which ER is active
// slots for creating a new ER, editing, etc

class ERControl : public QObject 
{
	ERControl() {}

public:
	void newER();
	void deleteER();
	void editER();

signals:
	
};

#endif /* ERCONTROL_H */
