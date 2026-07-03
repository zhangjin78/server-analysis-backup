/********************************************************************************
** Form generated from reading UI file 'filter.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_FilterDialog: public QObject
{
	Q_OBJECT

public:
    QPushButton *pbReset;
    QPushButton *pbApply;
    QPushButton *pbCancel;
    QFrame *frame;
    QLabel *label_6;
    QLabel *label;
    QLabel *label_4;
    QLineEdit *pcHostName;
    QComboBox *pcServer;
    QFrame *frame_2;
    QLabel *label_3;
    QLabel *label_7;
    QComboBox *pcRunStatus;
    QLabel *label_8;
    QLineEdit *pcRunId;
    QLineEdit *pcRunDataSet;
    QLabel *label_9;
    QLabel *label_10;
    QLineEdit *pcUid;
    QLabel *label_11;
    QLineEdit *pcPriority;
    QLineEdit *pcDataMC;
    QLabel *label_12;
    QFrame *frame_3;
    QLabel *label_2;
    QLabel *label_5;
    QComboBox *pcClientStatus;
    QLineEdit *pcThreads;
    QLabel *label_13;
    QLineEdit *pcProcessID;
    QLabel *label_14;

    void setupUi(QDialog *FilterDialog)
    {
        if (FilterDialog->objectName().isEmpty())
            FilterDialog->setObjectName(QString::fromUtf8("FilterDialog"));
        FilterDialog->setWindowModality(Qt::ApplicationModal);
        FilterDialog->resize(432, 388);
        QFont font;
        font.setFamily(QString::fromUtf8("DejaVu LGC Sans"));
        font.setBold(false);
        font.setWeight(50);
        FilterDialog->setFont(font);
        FilterDialog->setModal(true);
        pbReset = new QPushButton(FilterDialog);
        pbReset->setObjectName(QString::fromUtf8("pbReset"));
        pbReset->setGeometry(QRect(220, 326, 85, 27));
        pbApply = new QPushButton(FilterDialog);
        pbApply->setObjectName(QString::fromUtf8("pbApply"));
        pbApply->setGeometry(QRect(134, 326, 85, 27));
        pbCancel = new QPushButton(FilterDialog);
        pbCancel->setObjectName(QString::fromUtf8("pbCancel"));
        pbCancel->setGeometry(QRect(176, 354, 85, 27));
        frame = new QFrame(FilterDialog);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(4, 8, 143, 309));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label_6 = new QLabel(frame);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(4, 78, 137, 16));
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(4, 26, 135, 23));
        label_4 = new QLabel(frame);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(2, 2, 139, 16));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        label_4->setFont(font1);
        label_4->setAlignment(Qt::AlignCenter);
        pcHostName = new QLineEdit(frame);
        pcHostName->setObjectName(QString::fromUtf8("pcHostName"));
        pcHostName->setGeometry(QRect(6, 92, 131, 23));
        pcServer = new QComboBox(frame);
        pcServer->setObjectName(QString::fromUtf8("pcServer"));
        pcServer->setGeometry(QRect(6, 44, 95, 25));
        frame_2 = new QFrame(FilterDialog);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setGeometry(QRect(152, 8, 135, 309));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        label_3 = new QLabel(frame_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(0, 2, 133, 16));
        label_3->setFont(font1);
        label_3->setAlignment(Qt::AlignCenter);
        label_7 = new QLabel(frame_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(4, 30, 127, 16));
        pcRunStatus = new QComboBox(frame_2);
        pcRunStatus->setObjectName(QString::fromUtf8("pcRunStatus"));
        pcRunStatus->setGeometry(QRect(6, 44, 93, 25));
        label_8 = new QLabel(frame_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(3, 78, 129, 16));
        pcRunId = new QLineEdit(frame_2);
        pcRunId->setObjectName(QString::fromUtf8("pcRunId"));
        pcRunId->setGeometry(QRect(6, 92, 123, 23));
        pcRunDataSet = new QLineEdit(frame_2);
        pcRunDataSet->setObjectName(QString::fromUtf8("pcRunDataSet"));
        pcRunDataSet->setGeometry(QRect(6, 184, 123, 23));
        label_9 = new QLabel(frame_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(3, 170, 129, 16));
        label_10 = new QLabel(frame_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(4, 124, 127, 16));
        pcUid = new QLineEdit(frame_2);
        pcUid->setObjectName(QString::fromUtf8("pcUid"));
        pcUid->setGeometry(QRect(6, 138, 123, 23));
        label_11 = new QLabel(frame_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(4, 216, 129, 16));
        pcPriority = new QLineEdit(frame_2);
        pcPriority->setObjectName(QString::fromUtf8("pcPriority"));
        pcPriority->setGeometry(QRect(6, 232, 123, 23));
        pcDataMC = new QLineEdit(frame_2);
        pcDataMC->setObjectName(QString::fromUtf8("pcDataMC"));
        pcDataMC->setGeometry(QRect(6, 280, 123, 23));
        label_12 = new QLabel(frame_2);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(4, 264, 129, 16));
        frame_3 = new QFrame(FilterDialog);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setGeometry(QRect(292, 8, 135, 309));
        frame_3->setAutoFillBackground(false);
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        label_2 = new QLabel(frame_3);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(2, 0, 131, 17));
        label_2->setFont(font1);
        label_2->setAlignment(Qt::AlignCenter);
        label_5 = new QLabel(frame_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(2, 30, 127, 16));
        pcClientStatus = new QComboBox(frame_3);
        pcClientStatus->setObjectName(QString::fromUtf8("pcClientStatus"));
        pcClientStatus->setGeometry(QRect(6, 44, 93, 25));
        pcThreads = new QLineEdit(frame_3);
        pcThreads->setObjectName(QString::fromUtf8("pcThreads"));
        pcThreads->setGeometry(QRect(6, 92, 123, 23));
        label_13 = new QLabel(frame_3);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(3, 78, 129, 16));
        pcProcessID = new QLineEdit(frame_3);
        pcProcessID->setObjectName(QString::fromUtf8("pcProcessID"));
        pcProcessID->setGeometry(QRect(6, 138, 123, 23));
        label_14 = new QLabel(frame_3);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(3, 124, 129, 16));

        retranslateUi(FilterDialog);

        QMetaObject::connectSlotsByName(FilterDialog);
    } // setupUi

    void retranslateUi(QDialog *FilterDialog)
    {
        FilterDialog->setWindowTitle(QApplication::translate("FilterDialog", "Filter settings", 0, QApplication::UnicodeUTF8));
        pbReset->setText(QApplication::translate("FilterDialog", "Reset", 0, QApplication::UnicodeUTF8));
        pbApply->setText(QApplication::translate("FilterDialog", "Apply", 0, QApplication::UnicodeUTF8));
        pbCancel->setText(QApplication::translate("FilterDialog", "Exit", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("FilterDialog", "HostName", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("FilterDialog", "Server ID", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("FilterDialog", "Common settings", 0, QApplication::UnicodeUTF8));
        pcServer->clear();
        pcServer->insertItems(0, QStringList()
         << QString()
        );
        label_3->setText(QApplication::translate("FilterDialog", "Runs", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("FilterDialog", "Status", 0, QApplication::UnicodeUTF8));
        pcRunStatus->clear();
        pcRunStatus->insertItems(0, QStringList()
         << QString()
         << QApplication::translate("FilterDialog", "ToBeRerun", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("FilterDialog", "Failed", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("FilterDialog", "Processing", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("FilterDialog", "Finished", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("FilterDialog", "Unknown", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("FilterDialog", "Allocated", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("FilterDialog", "Foreign", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("FilterDialog", "Canceled", 0, QApplication::UnicodeUTF8)
        );
        label_8->setText(QApplication::translate("FilterDialog", "Run", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("FilterDialog", "DataSet", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("FilterDialog", "Uid", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("FilterDialog", "Priority", 0, QApplication::UnicodeUTF8));
        pcPriority->setText(QString());
        pcDataMC->setText(QString());
        label_12->setText(QApplication::translate("FilterDialog", "DataMC", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("FilterDialog", "ActiveClients", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("FilterDialog", "Status", 0, QApplication::UnicodeUTF8));
        pcClientStatus->clear();
        pcClientStatus->insertItems(0, QStringList()
         << QString()
         << QApplication::translate("FilterDialog", "Lost", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("FilterDialog", "Unknown", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("FilterDialog", "Submitted", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("FilterDialog", "Registered", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("FilterDialog", "Active", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("FilterDialog", "TimeOut", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("FilterDialog", "Killed", 0, QApplication::UnicodeUTF8)
        );
        label_13->setText(QApplication::translate("FilterDialog", "Threads", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("FilterDialog", "ProcessID", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

public slots:
   	virtual void clkbtApply(bool)=0;
   	virtual void clkbtReset(bool)=0;
   	virtual void clkbtCancel(bool)=0;
};

namespace Ui {
    class FilterDialog: public Ui_FilterDialog {};
} // namespace Ui

QT_END_NAMESPACE

