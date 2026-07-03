/********************************************************************************
** Form generated from reading UI file 'editcolumn.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_editColumnDialog: public QObject
{
	Q_OBJECT

public:
    QPushButton *pbtnOK;
    QPushButton *pbtnCancel;
    QLineEdit *pcField;

    void setupUi(QDialog *editColumnDialog)
    {
        if (editColumnDialog->objectName().isEmpty())
            editColumnDialog->setObjectName(QString::fromUtf8("editColumnDialog"));
        editColumnDialog->setWindowModality(Qt::ApplicationModal);
        editColumnDialog->resize(171, 21);
#ifndef QT_NO_STATUSTIP
        editColumnDialog->setStatusTip(QString::fromUtf8(""));
#endif // QT_NO_STATUSTIP
        editColumnDialog->setSizeGripEnabled(false);
        editColumnDialog->setModal(true);
        pbtnOK = new QPushButton(editColumnDialog);
        pbtnOK->setObjectName(QString::fromUtf8("pbtnOK"));
        pbtnOK->setGeometry(QRect(144, 0, 27, 21));
        QFont font;
        font.setFamily(QString::fromUtf8("DejaVu LGC Sans"));
        font.setPointSize(8);
        font.setItalic(false);
        pbtnOK->setFont(font);
        pbtnOK->setAutoFillBackground(false);
        pbtnOK->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        pbtnOK->setFlat(false);
        pbtnCancel = new QPushButton(editColumnDialog);
        pbtnCancel->setObjectName(QString::fromUtf8("pbtnCancel"));
        pbtnCancel->setGeometry(QRect(0, 0, 41, 21));
        pbtnCancel->setFont(font);
        pbtnCancel->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        pbtnCancel->setFlat(false);
        pcField = new QLineEdit(editColumnDialog);
        pcField->setObjectName(QString::fromUtf8("pcField"));
        pcField->setGeometry(QRect(40, 0, 105, 21));
        QFont font1;
        font1.setFamily(QString::fromUtf8("DejaVu LGC Sans"));
        font1.setPointSize(9);
        font1.setItalic(false);
        pcField->setFont(font1);
        pcField->setFrame(true);

        retranslateUi(editColumnDialog);

        QMetaObject::connectSlotsByName(editColumnDialog);
    } // setupUi

    void retranslateUi(QDialog *editColumnDialog)
    {
        editColumnDialog->setWindowTitle(QApplication::translate("editColumnDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        pbtnOK->setText(QApplication::translate("editColumnDialog", "OK", 0, QApplication::UnicodeUTF8));
        pbtnCancel->setText(QApplication::translate("editColumnDialog", "Cancel", 0, QApplication::UnicodeUTF8));
        pcField->setText(QString());
    } // retranslateUi

public slots:
    	virtual void clkbtOK(bool)=0;
    	virtual void clkbtCancel(bool)=0;
};

namespace Ui {
    class editColumnDialog: public Ui_editColumnDialog {};
} // namespace Ui

QT_END_NAMESPACE

