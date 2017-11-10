#ifndef ERCONTROL_H
#define ERCONTROL_H

#include <QObject>
#include <osg/Node>
#include <QUndoStack>

class EResourceGroup;
class MainWindow;
class ECategoryGroup;
class ERDialog;
class ERScrollBox;

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

	// opens new cat dialog
	void newERCat();

private:
	MainWindow *m_window;

	osg::ref_ptr<EResourceGroup> m_ers;
	osg::ref_ptr<ECategoryGroup> m_categories;

	ERDialog *m_display;

	ERScrollBox *m_box;

	QUndoStack *m_undo_stack;
};

#endif /* ERCONTROL_H */
