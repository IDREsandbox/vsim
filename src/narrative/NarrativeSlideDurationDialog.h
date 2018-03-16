#ifndef NARRATIVESLIDEDURATIONDIALOG_H
#define NARRATIVESLIDEDURATIONDIALOG_H
#include <QWidget>
#include "ui_NarrativeSlideDurationDialog.h"

class NarrativeSlideDurationDialog : public QDialog {
	Q_OBJECT

public:
	// Creates and execs a dialog
	// returns 0 if wait for click
	// returns >0 for duration
	static float create(bool stay, float duration);

	NarrativeSlideDurationDialog(QWidget * parent = nullptr);

	void setDuration(bool stay, float duration);

	// returns >0 for the timer
	// returns 0 if transition-on-click is checked
	float getDuration();

private:
	void enableDuration(bool enable);
	Ui::NarrativeSlideDurationDialog ui;
};

#endif // NARRATIVESLIDEDURATIONDIALOG_HPP