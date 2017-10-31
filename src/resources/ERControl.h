#ifndef ERCONTROL_H
#define ERCONTROL_H

#include <QObject>
#include <osg/Node>

class EResourceGroup;
class MainWindow;
class ECategoryGroup;
class ERDialog;

// manages which ER is active
// slots for creating a new ER, editing, etc
class ERControl : public QObject 
{
	Q_OBJECT;
public:
	ERControl(QObject *parent, MainWindow *window, EResourceGroup *ers, ECategoryGroup *categories);

	void load(EResourceGroup *ers, ECategoryGroup *categories);

	void newER();
	void deleteER();
	void editERInfo();
	void openResource();

	void newERCat(const std::string &name, QColor color);

signals:

private:
	MainWindow *m_window;


	osg::ref_ptr<EResourceGroup> m_ers;
	osg::ref_ptr<ECategoryGroup> m_categories;

	ERDialog *m_display;
};

#endif /* ERCONTROL_H */
