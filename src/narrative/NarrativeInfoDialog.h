#pragma once
#ifndef NARRATIVEINFODIALOG_HPP
#define NARRATIVEINFODIALOG_HPP
#include <QDialog>
#include <QDialogButtonBox>

#include "ui_NarrativeInfoDialog.h"
#include "narrative/Narrative2.h"

// low budget version, TODO: replace with narrative?
struct NarrativeInfo {
	std::string m_title;
	std::string m_description;
	std::string m_contact;
};

class NarrativeInfoDialog : public QDialog {
	Q_OBJECT

public:
	NarrativeInfoDialog(QWidget *parent = nullptr);
	NarrativeInfoDialog(QWidget *parent, const NarrativeInfo&);
	~NarrativeInfoDialog();

	void setInfo(const Narrative2&);
	const NarrativeInfo& getInfo();
	void clear();

protected:
	void accept();

private:
	void setGui();
	Ui::dialog ui;
	NarrativeInfo m_info;
};

#endif // NARRATIVEINFODIALOG_HPP