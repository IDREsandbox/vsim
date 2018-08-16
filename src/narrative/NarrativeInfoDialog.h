#ifndef NARRATIVEINFODIALOG_H
#define NARRATIVEINFODIALOG_H
#include <QDialog>
#include <QDialogButtonBox>

#include "ui_NarrativeInfoDialog.h"
#include "narrative/Narrative.h"

class NarrativeInfoDialog : public QDialog {
	Q_OBJECT
public:
	NarrativeInfoDialog(QWidget *parent = nullptr);
	NarrativeInfoDialog(const Narrative *, QWidget *parent = nullptr);
	~NarrativeInfoDialog();

	std::string getTitle() const;
	std::string getDescription() const;
	std::string getAuthor() const;

	void setReadOnly(bool read_only);

private:
	Ui::dialog ui;
};

#endif // NARRATIVEINFODIALOG_H