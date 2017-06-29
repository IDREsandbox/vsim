#ifndef NARRATIVESLIDEDURATIONDIALOG_H
#define NARRATIVESLIDEDURATIONDIALOG_H
#include <QWidget>
#include "ui_NarrativeSlideDurationDialog.h"

class NarrativeSlideDurationDialog : public QDialog {
	Q_OBJECT

public:
	NarrativeSlideDurationDialog(QWidget * parent = nullptr);

	// 0 for transition-on-click
	// >0 for duration value
	void setDuration(bool checked, float duration);

	// returns >0 for the timer
	// returns 0 if transition-on-click is checked
	float getDuration();

private:
	void enableDisableDuration(int checkbox_state);
	Ui::NarrativeSlideDurationDialog ui;
};

#endif // NARRATIVESLIDEDURATIONDIALOG_HPP