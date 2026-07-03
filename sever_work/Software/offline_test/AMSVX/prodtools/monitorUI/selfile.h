/********************************************************************************
** Form generated from reading UI file 'selfile.ui'
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
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_selFileDialog: public QObject
{
	Q_OBJECT

public:
    QPushButton *pbtnOK;
    QComboBox *pcbStr;

    void setupUi(QDialog *selFileDialog)
    {
        if (selFileDialog->objectName().isEmpty())
            selFileDialog->setObjectName(QString::fromUtf8("selFileDialog"));
        selFileDialog->setWindowModality(Qt::ApplicationModal);
        selFileDialog->setEnabled(true);
        selFileDialog->resize(441, 70);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(selFileDialog->sizePolicy().hasHeightForWidth());
        selFileDialog->setSizePolicy(sizePolicy);
#ifndef QT_NO_STATUSTIP
        selFileDialog->setStatusTip(QString::fromUtf8(""));
#endif // QT_NO_STATUSTIP
        selFileDialog->setSizeGripEnabled(false);
        selFileDialog->setModal(true);
        pbtnOK = new QPushButton(selFileDialog);
        pbtnOK->setObjectName(QString::fromUtf8("pbtnOK"));
        pbtnOK->setGeometry(QRect(190, 38, 65, 27));
        QFont font;
        font.setFamily(QString::fromUtf8("DejaVu LGC Sans"));
        font.setPointSize(8);
        font.setItalic(false);
        pbtnOK->setFont(font);
        pbtnOK->setAutoFillBackground(false);
        pbtnOK->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        pbtnOK->setCheckable(false);
        pbtnOK->setFlat(false);
        pcbStr = new QComboBox(selFileDialog);
        pcbStr->setObjectName(QString::fromUtf8("pcbStr"));
        pcbStr->setEnabled(true);
        pcbStr->setGeometry(QRect(4, 8, 433, 25));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pcbStr->sizePolicy().hasHeightForWidth());
        pcbStr->setSizePolicy(sizePolicy1);
#ifndef QT_NO_TOOLTIP
        pcbStr->setToolTip(QString::fromUtf8(""));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        pcbStr->setStatusTip(QString::fromUtf8(""));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
        pcbStr->setWhatsThis(QString::fromUtf8(""));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_ACCESSIBILITY
        pcbStr->setAccessibleName(QString::fromUtf8(""));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        pcbStr->setAccessibleDescription(QString::fromUtf8(""));
#endif // QT_NO_ACCESSIBILITY
        pcbStr->setLayoutDirection(Qt::RightToLeft);
        pcbStr->setEditable(false);

        retranslateUi(selFileDialog);

        QMetaObject::connectSlotsByName(selFileDialog);
    } // setupUi

    void retranslateUi(QDialog *selFileDialog)
    {
        selFileDialog->setWindowTitle(QApplication::translate("selFileDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        pbtnOK->setText(QApplication::translate("selFileDialog", "Continue", 0, QApplication::UnicodeUTF8));
        pcbStr->clear();
    } // retranslateUi

public slots:
    	virtual void clkbtOK(bool)=0;
};

namespace Ui {
    class selFileDialog: public Ui_selFileDialog {};
} // namespace Ui

QT_END_NAMESPACE

