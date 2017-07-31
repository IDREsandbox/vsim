#pragma once
#ifndef DRAGLABELINPUT_HPP
#define DRAGLABELINPUT_HPP
#include <QDialog>
#include "ui_dragLabelInput.h"
#include <QDialogButtonBox>
#include <QAbstractButton>

class dragLabelInput : public QDialog {
	Q_OBJECT

public:
	dragLabelInput(QWidget *parent = nullptr);
	dragLabelInput(QWidget *parent, const QString&);
	~dragLabelInput();

	const QString& getInfo();

	//signals:
	// QDialog - accepted, finished, rejected
	//void sOK(const NarrativeInfo&);
	//void sCancel();
protected:
	void accept();

private:
	Ui::editLabel ui;
	QString m_result;
};

#endif // DRAGLABELINPUT_HPP