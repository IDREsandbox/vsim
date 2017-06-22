#pragma once
#ifndef NARRATIVEINFODIALOG_HPP
#define NARRATIVEINFODIALOG_HPP
#include <QDialog>
#include "ui_NarrativeInfoDialog.h"
#include <QDialogButtonBox>
#include <QAbstractButton>

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

	const NarrativeInfo& getInfo();

//signals:
	// QDialog - accepted, finished, rejected
	//void sOK(const NarrativeInfo&);
	//void sCancel();
protected:
	void accept();

private:
	Ui::dialog ui;
	NarrativeInfo m_result;
};

#endif // NARRATIVEINFODIALOG_HPP