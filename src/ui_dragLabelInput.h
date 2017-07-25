/********************************************************************************
** Form generated from reading UI file 'dragLabelInputp11628.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef DRAGLABELINPUTP11628_H
#define DRAGLABELINPUTP11628_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QVBoxLayout>
#include "mrichtextedit.h"

QT_BEGIN_NAMESPACE

class Ui_editLabel
{
public:
    QVBoxLayout *verticalLayout;
    MRichTextEdit *widget;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *editLabel)
    {
        if (editLabel->objectName().isEmpty())
            editLabel->setObjectName(QStringLiteral("editLabel"));
        editLabel->resize(600, 300);
        editLabel->setMinimumSize(QSize(600, 300));
        editLabel->setMaximumSize(QSize(600, 300));
        verticalLayout = new QVBoxLayout(editLabel);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        widget = new MRichTextEdit(editLabel);
        widget->setObjectName(QStringLiteral("widget"));

        verticalLayout->addWidget(widget);

        buttonBox = new QDialogButtonBox(editLabel);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(editLabel);
        QObject::connect(buttonBox, SIGNAL(accepted()), editLabel, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), editLabel, SLOT(reject()));

        QMetaObject::connectSlotsByName(editLabel);
    } // setupUi

    void retranslateUi(QDialog *editLabel)
    {
        editLabel->setWindowTitle(QApplication::translate("editLabel", "Edit Label", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class editLabel: public Ui_editLabel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // DRAGLABELINPUTP11628_H
