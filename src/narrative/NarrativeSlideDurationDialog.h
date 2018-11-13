#ifndef NARRATIVESLIDEDURATIONDIALOG_H
#define NARRATIVESLIDEDURATIONDIALOG_H
#include <QWidget>
#include "ui_NarrativeSlideDurationDialog.h"

class NarrativeSlideDurationDialog : public QDialog {
	Q_OBJECT

public:
	// Creates and execs a dialog
	// returns <0 on cancel
	// returns true on accept, false on cancel
	// stores stay in *out_stay
	// stores duration in *out_duratio
	static bool create(bool init_stay, float init_duration, bool *out_stay, float *out_duration);

	NarrativeSlideDurationDialog(QWidget * parent = nullptr);

	void setDuration(bool stay, float duration);

	// returns >0 for the timer
	// returns 0 if transition-on-click is checked
	float getDuration() const;
	bool getStayOnNode() const;

private:
	void enableDuration(bool enable);
	Ui::NarrativeSlideDurationDialog ui;
};

#endif // NARRATIVESLIDEDURATIONDIALOG_HPP