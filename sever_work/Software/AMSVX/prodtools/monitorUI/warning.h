/********************************************************************************
** Form generated from reading UI file 'warning.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Dialog: public QObject
{
	Q_OBJECT

public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *dlgWarning;
    QPushButton *pbtWarningOk;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(390, 86);
        verticalLayoutWidget = new QWidget(Dialog);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(7, 10, 335, 67));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        dlgWarning = new QLabel(verticalLayoutWidget);
        dlgWarning->setObjectName(QString::fromUtf8("dlgWarning"));
        dlgWarning->setFrameShape(QFrame::NoFrame);
        dlgWarning->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(dlgWarning);

        pbtWarningOk = new QPushButton(Dialog);
        pbtWarningOk->setObjectName(QString::fromUtf8("pbtWarningOk"));
        pbtWarningOk->setGeometry(QRect(348, 50, 37, 27));

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", 0, QApplication::UnicodeUTF8));
        dlgWarning->setText(QApplication::translate("Dialog", "Please select destination amsprodserver", 0, QApplication::UnicodeUTF8));
        pbtWarningOk->setText(QApplication::translate("Dialog", "OK", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

public slots:
    	virtual void ok(bool)=0;
};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

