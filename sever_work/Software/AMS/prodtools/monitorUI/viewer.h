/********************************************************************************
** Form generated from reading UI file 'viewer.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form: public QObject
{
	Q_OBJECT
	
public:
    QGridLayout *gridLayout;
    QPlainTextEdit *plainTextEdit;
    QFrame *frame;
    QPushButton *pbtUp;
    QPushButton *pbtDown;
    QLineEdit *lnePattern;
    QCheckBox *chkCaseSens;
    QPushButton *pbtUpd;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QString::fromUtf8("Form"));
        Form->resize(691, 630);
        gridLayout = new QGridLayout(Form);
        gridLayout->setContentsMargins(2, 2, 2, 2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        plainTextEdit = new QPlainTextEdit(Form);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        QFont font;
        font.setFamily(QString::fromUtf8("DejaVu LGC Serif"));
        plainTextEdit->setFont(font);
        plainTextEdit->setFrameShadow(QFrame::Plain);

        gridLayout->addWidget(plainTextEdit, 0, 0, 1, 1);

        frame = new QFrame(Form);
        frame->setObjectName(QString::fromUtf8("frame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        frame->setMinimumSize(QSize(0, 24));
        frame->setFrameShape(QFrame::NoFrame);
        frame->setFrameShadow(QFrame::Plain);
        pbtUp = new QPushButton(frame);
        pbtUp->setObjectName(QString::fromUtf8("pbtUp"));
        pbtUp->setGeometry(QRect(80, 0, 27, 24));
        pbtUp->setMinimumSize(QSize(0, 20));
        QFont font1;
        font1.setFamily(QString::fromUtf8("DejaVu LGC Sans"));
        font1.setPointSize(8);
        font1.setUnderline(false);
        pbtUp->setFont(font1);
        pbtDown = new QPushButton(frame);
        pbtDown->setObjectName(QString::fromUtf8("pbtDown"));
        pbtDown->setGeometry(QRect(108, 0, 27, 24));
        pbtDown->setMinimumSize(QSize(0, 20));
        QFont font2;
        font2.setFamily(QString::fromUtf8("DejaVu LGC Sans"));
        font2.setPointSize(8);
        pbtDown->setFont(font2);
        lnePattern = new QLineEdit(frame);
        lnePattern->setObjectName(QString::fromUtf8("lnePattern"));
        lnePattern->setGeometry(QRect(136, -2, 227, 27));
        lnePattern->setFont(font2);
        lnePattern->setFrame(false);
        lnePattern->setAlignment(Qt::AlignCenter);
        chkCaseSens = new QCheckBox(frame);
        chkCaseSens->setObjectName(QString::fromUtf8("chkCaseSens"));
        chkCaseSens->setGeometry(QRect(0, 2, 75, 20));
        chkCaseSens->setFont(font2);
        chkCaseSens->setChecked(false);
        pbtUpd = new QPushButton(frame);
        pbtUpd->setObjectName(QString::fromUtf8("pbtUpd"));
        pbtUpd->setEnabled(false);
        pbtUpd->setGeometry(QRect(364, 0, 49, 24));
        pbtUpd->setMinimumSize(QSize(0, 20));
        pbtUpd->setFont(font2);
        pbtUpd->setCheckable(false);
        pbtUpd->setChecked(false);

        gridLayout->addWidget(frame, 1, 0, 1, 1);


        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0, QApplication::UnicodeUTF8));
        pbtUp->setText(QApplication::translate("Form", "<", 0, QApplication::UnicodeUTF8));
        pbtDown->setText(QApplication::translate("Form", ">", 0, QApplication::UnicodeUTF8));
        chkCaseSens->setText(QApplication::translate("Form", "CaseSens", 0, QApplication::UnicodeUTF8));
        pbtUpd->setText(QApplication::translate("Form", "Update", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

public slots:
	virtual void clkbtUp(bool checked)=0;
	virtual void clkbtDown(bool checked)=0;
	virtual void clkbtUpd(bool checked)=0;
};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

