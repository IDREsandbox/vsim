﻿#pragma once
#ifndef MAINWINDOWTOPBAR_HPP
#define MAINWINDOWTOPBAR_HPP
#include <QFrame>
#include "ui_mainwindowtopbar.h"

class MainWindowTopBar : public QFrame {
	Q_OBJECT

public:
	MainWindowTopBar(QWidget * parent = nullptr);
	~MainWindowTopBar();

	void showNarratives();
	void showSlides();

	void setSlidesHeader(std::string);

	// TODO
	//void enableEdting(bool);

public:
	Ui::MainWindowTopBar ui;
};

#endif // MAINWINDOWTOPBAR_HPP