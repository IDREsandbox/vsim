/********************************************************************************
** Form generated from reading UI file 'editButtons.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITBUTTONS_H
#define UI_EDITBUTTONS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_editButtons
{
public:
    QVBoxLayout *verticalLayout;
    QPushButton *head1;
    QPushButton *head2;
    QPushButton *body;
    QPushButton *label;
    QPushButton *image;
    QSpacerItem *verticalSpacer;
    QPushButton *edit;
    QPushButton *delete_2;
    QSpacerItem *verticalSpacer_2;
    QPushButton *done;

    void setupUi(QWidget *editButtons)
    {
        if (editButtons->objectName().isEmpty())
            editButtons->setObjectName(QStringLiteral("editButtons"));
        editButtons->resize(93, 305);
        editButtons->setMinimumSize(QSize(93, 305));
        editButtons->setMaximumSize(QSize(93, 300));
        verticalLayout = new QVBoxLayout(editButtons);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(3, 3, 3, 10);
        head1 = new QPushButton(editButtons);
        head1->setObjectName(QStringLiteral("head1"));
        head1->setMinimumSize(QSize(75, 30));

        verticalLayout->addWidget(head1);

        head2 = new QPushButton(editButtons);
        head2->setObjectName(QStringLiteral("head2"));
        head2->setMinimumSize(QSize(75, 30));

        verticalLayout->addWidget(head2);

        body = new QPushButton(editButtons);
        body->setObjectName(QStringLiteral("body"));
        body->setMinimumSize(QSize(75, 30));

        verticalLayout->addWidget(body);

        label = new QPushButton(editButtons);
        label->setObjectName(QStringLiteral("label"));
        label->setMinimumSize(QSize(75, 30));

        verticalLayout->addWidget(label);

        image = new QPushButton(editButtons);
        image->setObjectName(QStringLiteral("image"));
        image->setMinimumSize(QSize(75, 30));

        verticalLayout->addWidget(image);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer);

        edit = new QPushButton(editButtons);
        edit->setObjectName(QStringLiteral("edit"));
        edit->setMinimumSize(QSize(75, 30));

        verticalLayout->addWidget(edit);

        delete_2 = new QPushButton(editButtons);
        delete_2->setObjectName(QStringLiteral("delete_2"));
        delete_2->setMinimumSize(QSize(75, 30));

        verticalLayout->addWidget(delete_2);

        verticalSpacer_2 = new QSpacerItem(20, 47, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer_2);

        done = new QPushButton(editButtons);
        done->setObjectName(QStringLiteral("done"));
        done->setMinimumSize(QSize(75, 30));

        verticalLayout->addWidget(done);


        retranslateUi(editButtons);

        QMetaObject::connectSlotsByName(editButtons);
    } // setupUi

    void retranslateUi(QWidget *editButtons)
    {
        editButtons->setWindowTitle(QApplication::translate("editButtons", "Form", Q_NULLPTR));
        head1->setText(QApplication::translate("editButtons", "Head1", Q_NULLPTR));
        head2->setText(QApplication::translate("editButtons", "Head2", Q_NULLPTR));
        body->setText(QApplication::translate("editButtons", "Body", Q_NULLPTR));
        label->setText(QApplication::translate("editButtons", "Label", Q_NULLPTR));
        image->setText(QApplication::translate("editButtons", "Image", Q_NULLPTR));
        edit->setText(QApplication::translate("editButtons", "Edit", Q_NULLPTR));
        delete_2->setText(QApplication::translate("editButtons", "Delete", Q_NULLPTR));
        done->setText(QApplication::translate("editButtons", "Done", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class editButtons: public Ui_editButtons {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITBUTTONS_H
