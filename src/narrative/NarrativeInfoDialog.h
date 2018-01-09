#ifndef NARRATIVEINFODIALOG_HPP
#define NARRATIVEINFODIALOG_HPP
#include <QDialog>
#include <QDialogButtonBox>

#include "ui_NarrativeInfoDialog.h"
#include "narrative/Narrative2.h"

class NarrativeInfoDialog : public QDialog {
	Q_OBJECT
public:
	NarrativeInfoDialog(QWidget *parent = nullptr);
	NarrativeInfoDialog(const Narrative2 *, QWidget *parent = nullptr);
	~NarrativeInfoDialog();

	std::string getTitle() const;
	std::string getDescription() const;
	std::string getAuthor() const;

private:
	void setGui();
	Ui::dialog ui;
};

#endif // NARRATIVEINFODIALOG_HPP