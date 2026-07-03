/********************************************************************************
** Form generated from reading UI file 'mainw_dbvw.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QFileDialog>

QT_BEGIN_NAMESPACE

class qtabUtils: public QObject
{
	Q_OBJECT

public:
	qtabUtils() { };
	virtual ~qtabUtils() {};

public slots:
	virtual void clktbHeader(int) {};
	virtual void clkSelection(int, int) {};
	virtual void clkSelectionChanged() {};

protected:
	virtual bool eventFilter(QObject *, QEvent *);
	friend class myQMainWindow;
};

class myQFileDialog: public QFileDialog
{
	Q_OBJECT

public:
	myQFileDialog();
	virtual ~myQFileDialog();
public slots:
	void sig_dirEntered(const QString &dir);
	void sig_curChanged(const QString &path);
};

class Ui_MainWindow: public QObject
{
	Q_OBJECT

public:
    QAction *actQuit;
    QAction *actOpen;
    QAction *actZoomIn;
    QAction *actZoomOut;
    QAction *actExportDB;
    QAction *actNew;
    QAction *actUseFilter;
    QAction *actSetFilter;
    QAction *actSaveAs;
    QAction *actImportDB;
    QAction *actUpdateStatus;
    QAction *actCommitStatus;
    QAction *actRecover;
    QAction *actNew2;
    QWidget *centralWidget;
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout;
    QStackedWidget *stackedWidget2;
    QWidget *page_4;
    QGridLayout *gridLayout_23;
    QLabel *label_10;
    QWidget *page_3;
    QGridLayout *gridLayout_13;
    QTableWidget *qtw_sNC;
    QLabel *label_11;
    QWidget *page_5;
    QGridLayout *gridLayout_14;
    QLabel *label_12;
    QTableWidget *qtw_sNH;
    QWidget *page_6;
    QGridLayout *gridLayout_15;
    QLabel *label_13;
    QTableWidget *qtw_sNK;
    QWidget *page_7;
    QGridLayout *gridLayout_16;
    QLabel *label_14;
    QTableWidget *qtw_sSE;
    QWidget *page_8;
    QGridLayout *gridLayout_17;
    QLabel *label_15;
    QTableWidget *qtw_pNC;
    QWidget *page_9;
    QGridLayout *gridLayout_18;
    QLabel *label_16;
    QTableWidget *qtw_pAC;
    QWidget *page_10;
    QGridLayout *gridLayout_19;
    QLabel *label_17;
    QTableWidget *qtw_pNH;
    QWidget *page_11;
    QGridLayout *gridLayout_20;
    QLabel *label_18;
    QTableWidget *qtw_pNNT;
    QWidget *page_12;
    QGridLayout *gridLayout_21;
    QLabel *label_19;
    QTableWidget *qtw_pPNT;
    QWidget *page_13;
    QGridLayout *gridLayout_22;
    QLabel *label_20;
    QTableWidget *qtw_pRT;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout;
    QPushButton *pbtRemove;
    QPushButton *pbtNew;
    QPushButton *pbtMoveUp;
    QPushButton *pbtMoveDown;
    QTreeWidget *treeWidget2;
    QMenuBar *menuBar;
    QMenu *menuFiles;
    QMenu *menuPreferences;
    QMenu *menuTools;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1134, 728);
        actQuit = new QAction(MainWindow);
        actQuit->setObjectName(QString::fromUtf8("actQuit"));
        actOpen = new QAction(MainWindow);
        actOpen->setObjectName(QString::fromUtf8("actOpen"));
        actZoomIn = new QAction(MainWindow);
        actZoomIn->setObjectName(QString::fromUtf8("actZoomIn"));
        actZoomOut = new QAction(MainWindow);
        actZoomOut->setObjectName(QString::fromUtf8("actZoomOut"));
        actExportDB = new QAction(MainWindow);
        actExportDB->setObjectName(QString::fromUtf8("actExportDB"));
        actNew = new QAction(MainWindow);
        actNew->setObjectName(QString::fromUtf8("actNew"));
        actUseFilter = new QAction(MainWindow);
        actUseFilter->setObjectName(QString::fromUtf8("actUseFilter"));
        actUseFilter->setCheckable(true);
        actSetFilter = new QAction(MainWindow);
        actSetFilter->setObjectName(QString::fromUtf8("actSetFilter"));
        actSaveAs = new QAction(MainWindow);
        actSaveAs->setObjectName(QString::fromUtf8("actSaveAs"));
        actImportDB = new QAction(MainWindow);
        actImportDB->setObjectName(QString::fromUtf8("actImportDB"));
        actUpdateStatus = new QAction(MainWindow);
        actUpdateStatus->setObjectName(QString::fromUtf8("actUpdateStatus"));
        actCommitStatus = new QAction(MainWindow);
        actCommitStatus->setObjectName(QString::fromUtf8("actCommitStatus"));
        actRecover = new QAction(MainWindow);
        actRecover->setObjectName(QString::fromUtf8("actRecover"));
        actNew2 = new QAction(MainWindow);
        actNew2->setObjectName(QString::fromUtf8("actNew2"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout_4 = new QGridLayout(centralWidget);
        gridLayout_4->setSpacing(0);
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        stackedWidget2 = new QStackedWidget(centralWidget);
        stackedWidget2->setObjectName(QString::fromUtf8("stackedWidget2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(stackedWidget2->sizePolicy().hasHeightForWidth());
        stackedWidget2->setSizePolicy(sizePolicy);
        QFont font;
        font.setPointSize(8);
        stackedWidget2->setFont(font);
        stackedWidget2->setFrameShape(QFrame::Box);
        page_4 = new QWidget();
        page_4->setObjectName(QString::fromUtf8("page_4"));
        gridLayout_23 = new QGridLayout(page_4);
        gridLayout_23->setSpacing(6);
        gridLayout_23->setContentsMargins(11, 11, 11, 11);
        gridLayout_23->setObjectName(QString::fromUtf8("gridLayout_23"));
        label_10 = new QLabel(page_4);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        QFont font1;
        font1.setPointSize(20);
        label_10->setFont(font1);
        label_10->setLineWidth(0);
        label_10->setMidLineWidth(-2);
        label_10->setAlignment(Qt::AlignCenter);

        gridLayout_23->addWidget(label_10, 0, 0, 1, 1);

        stackedWidget2->addWidget(page_4);
        page_3 = new QWidget();
        page_3->setObjectName(QString::fromUtf8("page_3"));
        gridLayout_13 = new QGridLayout(page_3);
        gridLayout_13->setSpacing(6);
        gridLayout_13->setContentsMargins(11, 11, 11, 11);
        gridLayout_13->setObjectName(QString::fromUtf8("gridLayout_13"));
        qtw_sNC = new QTableWidget(page_3);
        if (qtw_sNC->columnCount() < 11)
            qtw_sNC->setColumnCount(11);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(9, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(10, __qtablewidgetitem10);
        qtw_sNC->setObjectName(QString::fromUtf8("qtw_sNC"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("DejaVu LGC Serif Condensed"));
        font2.setPointSize(9);
        qtw_sNC->setFont(font2);

        gridLayout_13->addWidget(qtw_sNC, 1, 0, 1, 1);

        label_11 = new QLabel(page_3);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setFont(font);

        gridLayout_13->addWidget(label_11, 0, 0, 1, 1);

        stackedWidget2->addWidget(page_3);
        page_5 = new QWidget();
        page_5->setObjectName(QString::fromUtf8("page_5"));
        gridLayout_14 = new QGridLayout(page_5);
        gridLayout_14->setSpacing(6);
        gridLayout_14->setContentsMargins(11, 11, 11, 11);
        gridLayout_14->setObjectName(QString::fromUtf8("gridLayout_14"));
        label_12 = new QLabel(page_5);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setFont(font);

        gridLayout_14->addWidget(label_12, 0, 0, 1, 1);

        qtw_sNH = new QTableWidget(page_5);
        if (qtw_sNH->columnCount() < 9)
            qtw_sNH->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(0, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(1, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(2, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(3, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(4, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(5, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(6, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(7, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(8, __qtablewidgetitem19);
        qtw_sNH->setObjectName(QString::fromUtf8("qtw_sNH"));
        qtw_sNH->setFont(font2);

        gridLayout_14->addWidget(qtw_sNH, 1, 0, 1, 1);

        stackedWidget2->addWidget(page_5);
        page_6 = new QWidget();
        page_6->setObjectName(QString::fromUtf8("page_6"));
        gridLayout_15 = new QGridLayout(page_6);
        gridLayout_15->setSpacing(6);
        gridLayout_15->setContentsMargins(11, 11, 11, 11);
        gridLayout_15->setObjectName(QString::fromUtf8("gridLayout_15"));
        label_13 = new QLabel(page_6);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setFont(font);

        gridLayout_15->addWidget(label_13, 0, 0, 1, 1);

        qtw_sNK = new QTableWidget(page_6);
        if (qtw_sNK->columnCount() < 11)
            qtw_sNK->setColumnCount(11);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(0, __qtablewidgetitem20);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(1, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(2, __qtablewidgetitem22);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(3, __qtablewidgetitem23);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(4, __qtablewidgetitem24);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(5, __qtablewidgetitem25);
        QTableWidgetItem *__qtablewidgetitem26 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(6, __qtablewidgetitem26);
        QTableWidgetItem *__qtablewidgetitem27 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(7, __qtablewidgetitem27);
        QTableWidgetItem *__qtablewidgetitem28 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(8, __qtablewidgetitem28);
        QTableWidgetItem *__qtablewidgetitem29 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(9, __qtablewidgetitem29);
        QTableWidgetItem *__qtablewidgetitem30 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(10, __qtablewidgetitem30);
        qtw_sNK->setObjectName(QString::fromUtf8("qtw_sNK"));
        qtw_sNK->setFont(font2);

        gridLayout_15->addWidget(qtw_sNK, 1, 0, 1, 1);

        stackedWidget2->addWidget(page_6);
        page_7 = new QWidget();
        page_7->setObjectName(QString::fromUtf8("page_7"));
        gridLayout_16 = new QGridLayout(page_7);
        gridLayout_16->setSpacing(6);
        gridLayout_16->setContentsMargins(11, 11, 11, 11);
        gridLayout_16->setObjectName(QString::fromUtf8("gridLayout_16"));
        label_14 = new QLabel(page_7);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setFont(font);

        gridLayout_16->addWidget(label_14, 0, 0, 1, 1);

        qtw_sSE = new QTableWidget(page_7);
        if (qtw_sSE->columnCount() < 3)
            qtw_sSE->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem31 = new QTableWidgetItem();
        qtw_sSE->setHorizontalHeaderItem(0, __qtablewidgetitem31);
        QTableWidgetItem *__qtablewidgetitem32 = new QTableWidgetItem();
        qtw_sSE->setHorizontalHeaderItem(1, __qtablewidgetitem32);
        QTableWidgetItem *__qtablewidgetitem33 = new QTableWidgetItem();
        qtw_sSE->setHorizontalHeaderItem(2, __qtablewidgetitem33);
        qtw_sSE->setObjectName(QString::fromUtf8("qtw_sSE"));
        qtw_sSE->setFont(font2);

        gridLayout_16->addWidget(qtw_sSE, 1, 0, 1, 1);

        stackedWidget2->addWidget(page_7);
        page_8 = new QWidget();
        page_8->setObjectName(QString::fromUtf8("page_8"));
        gridLayout_17 = new QGridLayout(page_8);
        gridLayout_17->setSpacing(6);
        gridLayout_17->setContentsMargins(11, 11, 11, 11);
        gridLayout_17->setObjectName(QString::fromUtf8("gridLayout_17"));
        label_15 = new QLabel(page_8);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setFont(font);

        gridLayout_17->addWidget(label_15, 0, 0, 1, 1);

        qtw_pNC = new QTableWidget(page_8);
        if (qtw_pNC->columnCount() < 11)
            qtw_pNC->setColumnCount(11);
        QTableWidgetItem *__qtablewidgetitem34 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(0, __qtablewidgetitem34);
        QTableWidgetItem *__qtablewidgetitem35 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(1, __qtablewidgetitem35);
        QTableWidgetItem *__qtablewidgetitem36 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(2, __qtablewidgetitem36);
        QTableWidgetItem *__qtablewidgetitem37 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(3, __qtablewidgetitem37);
        QTableWidgetItem *__qtablewidgetitem38 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(4, __qtablewidgetitem38);
        QTableWidgetItem *__qtablewidgetitem39 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(5, __qtablewidgetitem39);
        QTableWidgetItem *__qtablewidgetitem40 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(6, __qtablewidgetitem40);
        QTableWidgetItem *__qtablewidgetitem41 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(7, __qtablewidgetitem41);
        QTableWidgetItem *__qtablewidgetitem42 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(8, __qtablewidgetitem42);
        QTableWidgetItem *__qtablewidgetitem43 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(9, __qtablewidgetitem43);
        QTableWidgetItem *__qtablewidgetitem44 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(10, __qtablewidgetitem44);
        qtw_pNC->setObjectName(QString::fromUtf8("qtw_pNC"));
        qtw_pNC->setFont(font2);

        gridLayout_17->addWidget(qtw_pNC, 1, 0, 1, 1);

        stackedWidget2->addWidget(page_8);
        page_9 = new QWidget();
        page_9->setObjectName(QString::fromUtf8("page_9"));
        gridLayout_18 = new QGridLayout(page_9);
        gridLayout_18->setSpacing(6);
        gridLayout_18->setContentsMargins(11, 11, 11, 11);
        gridLayout_18->setObjectName(QString::fromUtf8("gridLayout_18"));
        label_16 = new QLabel(page_9);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setFont(font);

        gridLayout_18->addWidget(label_16, 0, 0, 1, 1);

        qtw_pAC = new QTableWidget(page_9);
        if (qtw_pAC->columnCount() < 11)
            qtw_pAC->setColumnCount(11);
        QTableWidgetItem *__qtablewidgetitem45 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(0, __qtablewidgetitem45);
        QTableWidgetItem *__qtablewidgetitem46 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(1, __qtablewidgetitem46);
        QTableWidgetItem *__qtablewidgetitem47 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(2, __qtablewidgetitem47);
        QTableWidgetItem *__qtablewidgetitem48 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(3, __qtablewidgetitem48);
        QTableWidgetItem *__qtablewidgetitem49 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(4, __qtablewidgetitem49);
        QTableWidgetItem *__qtablewidgetitem50 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(5, __qtablewidgetitem50);
        QTableWidgetItem *__qtablewidgetitem51 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(6, __qtablewidgetitem51);
        QTableWidgetItem *__qtablewidgetitem52 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(7, __qtablewidgetitem52);
        QTableWidgetItem *__qtablewidgetitem53 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(8, __qtablewidgetitem53);
        QTableWidgetItem *__qtablewidgetitem54 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(9, __qtablewidgetitem54);
        QTableWidgetItem *__qtablewidgetitem55 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(10, __qtablewidgetitem55);
        qtw_pAC->setObjectName(QString::fromUtf8("qtw_pAC"));
        qtw_pAC->setFont(font2);

        gridLayout_18->addWidget(qtw_pAC, 1, 0, 1, 1);

        stackedWidget2->addWidget(page_9);
        page_10 = new QWidget();
        page_10->setObjectName(QString::fromUtf8("page_10"));
        gridLayout_19 = new QGridLayout(page_10);
        gridLayout_19->setSpacing(6);
        gridLayout_19->setContentsMargins(11, 11, 11, 11);
        gridLayout_19->setObjectName(QString::fromUtf8("gridLayout_19"));
        label_17 = new QLabel(page_10);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setFont(font);

        gridLayout_19->addWidget(label_17, 0, 0, 1, 1);

        qtw_pNH = new QTableWidget(page_10);
        if (qtw_pNH->columnCount() < 12)
            qtw_pNH->setColumnCount(12);
        QTableWidgetItem *__qtablewidgetitem56 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(0, __qtablewidgetitem56);
        QTableWidgetItem *__qtablewidgetitem57 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(1, __qtablewidgetitem57);
        QTableWidgetItem *__qtablewidgetitem58 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(2, __qtablewidgetitem58);
        QTableWidgetItem *__qtablewidgetitem59 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(3, __qtablewidgetitem59);
        QTableWidgetItem *__qtablewidgetitem60 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(4, __qtablewidgetitem60);
        QTableWidgetItem *__qtablewidgetitem61 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(5, __qtablewidgetitem61);
        QTableWidgetItem *__qtablewidgetitem62 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(6, __qtablewidgetitem62);
        QTableWidgetItem *__qtablewidgetitem63 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(7, __qtablewidgetitem63);
        QTableWidgetItem *__qtablewidgetitem64 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(8, __qtablewidgetitem64);
        QTableWidgetItem *__qtablewidgetitem65 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(9, __qtablewidgetitem65);
        QTableWidgetItem *__qtablewidgetitem66 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(10, __qtablewidgetitem66);
        QTableWidgetItem *__qtablewidgetitem67 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(11, __qtablewidgetitem67);
        qtw_pNH->setObjectName(QString::fromUtf8("qtw_pNH"));
        qtw_pNH->setFont(font2);

        gridLayout_19->addWidget(qtw_pNH, 1, 0, 1, 1);

        stackedWidget2->addWidget(page_10);
        page_11 = new QWidget();
        page_11->setObjectName(QString::fromUtf8("page_11"));
        gridLayout_20 = new QGridLayout(page_11);
        gridLayout_20->setSpacing(6);
        gridLayout_20->setContentsMargins(11, 11, 11, 11);
        gridLayout_20->setObjectName(QString::fromUtf8("gridLayout_20"));
        label_18 = new QLabel(page_11);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        gridLayout_20->addWidget(label_18, 0, 0, 1, 1);

        qtw_pNNT = new QTableWidget(page_11);
        if (qtw_pNNT->columnCount() < 7)
            qtw_pNNT->setColumnCount(7);
        QTableWidgetItem *__qtablewidgetitem68 = new QTableWidgetItem();
        qtw_pNNT->setHorizontalHeaderItem(0, __qtablewidgetitem68);
        QTableWidgetItem *__qtablewidgetitem69 = new QTableWidgetItem();
        qtw_pNNT->setHorizontalHeaderItem(1, __qtablewidgetitem69);
        QTableWidgetItem *__qtablewidgetitem70 = new QTableWidgetItem();
        qtw_pNNT->setHorizontalHeaderItem(2, __qtablewidgetitem70);
        QTableWidgetItem *__qtablewidgetitem71 = new QTableWidgetItem();
        qtw_pNNT->setHorizontalHeaderItem(3, __qtablewidgetitem71);
        QTableWidgetItem *__qtablewidgetitem72 = new QTableWidgetItem();
        qtw_pNNT->setHorizontalHeaderItem(4, __qtablewidgetitem72);
        QTableWidgetItem *__qtablewidgetitem73 = new QTableWidgetItem();
        qtw_pNNT->setHorizontalHeaderItem(5, __qtablewidgetitem73);
        QTableWidgetItem *__qtablewidgetitem74 = new QTableWidgetItem();
        qtw_pNNT->setHorizontalHeaderItem(6, __qtablewidgetitem74);
        qtw_pNNT->setObjectName(QString::fromUtf8("qtw_pNNT"));
        qtw_pNNT->setFont(font2);

        gridLayout_20->addWidget(qtw_pNNT, 1, 0, 1, 1);

        stackedWidget2->addWidget(page_11);
        page_12 = new QWidget();
        page_12->setObjectName(QString::fromUtf8("page_12"));
        gridLayout_21 = new QGridLayout(page_12);
        gridLayout_21->setSpacing(6);
        gridLayout_21->setContentsMargins(11, 11, 11, 11);
        gridLayout_21->setObjectName(QString::fromUtf8("gridLayout_21"));
        label_19 = new QLabel(page_12);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        gridLayout_21->addWidget(label_19, 0, 0, 1, 1);

        qtw_pPNT = new QTableWidget(page_12);
        if (qtw_pPNT->columnCount() < 10)
            qtw_pPNT->setColumnCount(10);
        QTableWidgetItem *__qtablewidgetitem75 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(0, __qtablewidgetitem75);
        QTableWidgetItem *__qtablewidgetitem76 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(1, __qtablewidgetitem76);
        QTableWidgetItem *__qtablewidgetitem77 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(2, __qtablewidgetitem77);
        QTableWidgetItem *__qtablewidgetitem78 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(3, __qtablewidgetitem78);
        QTableWidgetItem *__qtablewidgetitem79 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(4, __qtablewidgetitem79);
        QTableWidgetItem *__qtablewidgetitem80 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(5, __qtablewidgetitem80);
        QTableWidgetItem *__qtablewidgetitem81 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(6, __qtablewidgetitem81);
        QTableWidgetItem *__qtablewidgetitem82 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(7, __qtablewidgetitem82);
        QTableWidgetItem *__qtablewidgetitem83 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(8, __qtablewidgetitem83);
        QTableWidgetItem *__qtablewidgetitem84 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(9, __qtablewidgetitem84);
        qtw_pPNT->setObjectName(QString::fromUtf8("qtw_pPNT"));
        qtw_pPNT->setFont(font2);

        gridLayout_21->addWidget(qtw_pPNT, 1, 0, 1, 1);

        stackedWidget2->addWidget(page_12);
        page_13 = new QWidget();
        page_13->setObjectName(QString::fromUtf8("page_13"));
        gridLayout_22 = new QGridLayout(page_13);
        gridLayout_22->setSpacing(6);
        gridLayout_22->setContentsMargins(11, 11, 11, 11);
        gridLayout_22->setObjectName(QString::fromUtf8("gridLayout_22"));
        label_20 = new QLabel(page_13);
        label_20->setObjectName(QString::fromUtf8("label_20"));

        gridLayout_22->addWidget(label_20, 0, 0, 1, 1);

        qtw_pRT = new QTableWidget(page_13);
        if (qtw_pRT->columnCount() < 17)
            qtw_pRT->setColumnCount(17);
        QTableWidgetItem *__qtablewidgetitem85 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(0, __qtablewidgetitem85);
        QTableWidgetItem *__qtablewidgetitem86 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(1, __qtablewidgetitem86);
        QTableWidgetItem *__qtablewidgetitem87 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(2, __qtablewidgetitem87);
        QTableWidgetItem *__qtablewidgetitem88 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(3, __qtablewidgetitem88);
        QTableWidgetItem *__qtablewidgetitem89 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(4, __qtablewidgetitem89);
        QTableWidgetItem *__qtablewidgetitem90 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(5, __qtablewidgetitem90);
        QTableWidgetItem *__qtablewidgetitem91 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(6, __qtablewidgetitem91);
        QTableWidgetItem *__qtablewidgetitem92 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(7, __qtablewidgetitem92);
        QTableWidgetItem *__qtablewidgetitem93 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(8, __qtablewidgetitem93);
        QTableWidgetItem *__qtablewidgetitem94 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(9, __qtablewidgetitem94);
        QTableWidgetItem *__qtablewidgetitem95 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(10, __qtablewidgetitem95);
        QTableWidgetItem *__qtablewidgetitem96 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(11, __qtablewidgetitem96);
        QTableWidgetItem *__qtablewidgetitem97 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(12, __qtablewidgetitem97);
        QTableWidgetItem *__qtablewidgetitem98 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(13, __qtablewidgetitem98);
        QTableWidgetItem *__qtablewidgetitem99 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(14, __qtablewidgetitem99);
        QTableWidgetItem *__qtablewidgetitem100 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(15, __qtablewidgetitem100);
        QTableWidgetItem *__qtablewidgetitem101 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(16, __qtablewidgetitem101);
        qtw_pRT->setObjectName(QString::fromUtf8("qtw_pRT"));
        qtw_pRT->setFont(font2);

        gridLayout_22->addWidget(qtw_pRT, 1, 0, 1, 1);

        stackedWidget2->addWidget(page_13);

        horizontalLayout->addWidget(stackedWidget2);

        widget_2 = new QWidget(centralWidget);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy1);
        widget_2->setFont(font);
        verticalLayout = new QVBoxLayout(widget_2);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        pbtRemove = new QPushButton(widget_2);
        pbtRemove->setObjectName(QString::fromUtf8("pbtRemove"));
        pbtRemove->setEnabled(false);

        verticalLayout->addWidget(pbtRemove);

        pbtNew = new QPushButton(widget_2);
        pbtNew->setObjectName(QString::fromUtf8("pbtNew"));
        pbtNew->setEnabled(false);

        verticalLayout->addWidget(pbtNew);

        pbtMoveUp = new QPushButton(widget_2);
        pbtMoveUp->setObjectName(QString::fromUtf8("pbtMoveUp"));
        pbtMoveUp->setEnabled(false);

        verticalLayout->addWidget(pbtMoveUp);

        pbtMoveDown = new QPushButton(widget_2);
        pbtMoveDown->setObjectName(QString::fromUtf8("pbtMoveDown"));
        pbtMoveDown->setEnabled(false);

        verticalLayout->addWidget(pbtMoveDown);


        horizontalLayout->addWidget(widget_2);


        gridLayout_4->addLayout(horizontalLayout, 0, 2, 2, 1);

        treeWidget2 = new QTreeWidget(centralWidget);
        treeWidget2->setObjectName(QString::fromUtf8("treeWidget2"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(treeWidget2->sizePolicy().hasHeightForWidth());
        treeWidget2->setSizePolicy(sizePolicy2);
        treeWidget2->setMaximumSize(QSize(200, 16777215));

        gridLayout_4->addWidget(treeWidget2, 0, 1, 2, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1134, 23));
        menuFiles = new QMenu(menuBar);
        menuFiles->setObjectName(QString::fromUtf8("menuFiles"));
        menuPreferences = new QMenu(menuBar);
        menuPreferences->setObjectName(QString::fromUtf8("menuPreferences"));
        menuTools = new QMenu(menuBar);
        menuTools->setObjectName(QString::fromUtf8("menuTools"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        statusBar->setFont(font2);
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFiles->menuAction());
        menuBar->addAction(menuPreferences->menuAction());
        menuBar->addAction(menuTools->menuAction());
        menuFiles->addAction(actOpen);
        menuFiles->addAction(actNew);
        menuFiles->addAction(actNew2);
        menuFiles->addAction(actRecover);
        menuFiles->addSeparator();
        menuFiles->addAction(actUpdateStatus);
        menuFiles->addAction(actCommitStatus);
        menuFiles->addSeparator();
        menuFiles->addAction(actQuit);
        menuPreferences->addAction(actZoomIn);
        menuPreferences->addAction(actZoomOut);
        menuPreferences->addSeparator();
        menuPreferences->addAction(actUseFilter);
        menuPreferences->addSeparator();
        menuTools->addAction(actExportDB);
        menuTools->addAction(actImportDB);
        menuTools->addSeparator();
        menuTools->addAction(actSetFilter);

        retranslateUi(MainWindow);

        stackedWidget2->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        actQuit->setText(QApplication::translate("MainWindow", "Quit                <alt>q", 0, QApplication::UnicodeUTF8));
        actOpen->setText(QApplication::translate("MainWindow", "Open", 0, QApplication::UnicodeUTF8));
        actOpen->setIconText(QApplication::translate("MainWindow", "Open DB file", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actOpen->setToolTip(QApplication::translate("MainWindow", "Open DB file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actZoomIn->setText(QApplication::translate("MainWindow", "Zoom In        <ctrl>+", 0, QApplication::UnicodeUTF8));
        actZoomIn->setIconText(QApplication::translate("MainWindow", "Zoom In", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actZoomIn->setToolTip(QApplication::translate("MainWindow", "Zoom In", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actZoomOut->setText(QApplication::translate("MainWindow", "Zoom Out     <ctrl>-", 0, QApplication::UnicodeUTF8));
        actZoomOut->setIconText(QApplication::translate("MainWindow", "Zoom Out", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actZoomOut->setToolTip(QApplication::translate("MainWindow", "Zoom Out", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actExportDB->setText(QApplication::translate("MainWindow", "Export DB", 0, QApplication::UnicodeUTF8));
        actNew->setText(QApplication::translate("MainWindow", "New", 0, QApplication::UnicodeUTF8));
        actNew->setIconText(QApplication::translate("MainWindow", "New DB file", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actNew->setToolTip(QApplication::translate("MainWindow", "New DB file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actUseFilter->setText(QApplication::translate("MainWindow", "Use filter", 0, QApplication::UnicodeUTF8));
        actUseFilter->setIconText(QApplication::translate("MainWindow", "UseFilter", 0, QApplication::UnicodeUTF8));
        actSetFilter->setText(QApplication::translate("MainWindow", "Set filter    <ctrl>f", 0, QApplication::UnicodeUTF8));
        actSetFilter->setIconText(QApplication::translate("MainWindow", "Set filter", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actSetFilter->setToolTip(QApplication::translate("MainWindow", "Set filter", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actSaveAs->setText(QApplication::translate("MainWindow", "Save as", 0, QApplication::UnicodeUTF8));
        actImportDB->setText(QApplication::translate("MainWindow", "Import DB", 0, QApplication::UnicodeUTF8));
        actUpdateStatus->setText(QApplication::translate("MainWindow", "Update table  <alt>u", 0, QApplication::UnicodeUTF8));
        actUpdateStatus->setIconText(QApplication::translate("MainWindow", "Update table from DB ", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actUpdateStatus->setToolTip(QApplication::translate("MainWindow", "Update table from DB ", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actCommitStatus->setText(QApplication::translate("MainWindow", "Commit table  <alt>c", 0, QApplication::UnicodeUTF8));
        actCommitStatus->setIconText(QApplication::translate("MainWindow", "Commit table to DB", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actCommitStatus->setToolTip(QApplication::translate("MainWindow", "Commit table to DB", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actRecover->setText(QApplication::translate("MainWindow", "Recover", 0, QApplication::UnicodeUTF8));
        actNew2->setText(QApplication::translate("MainWindow", "New v.2", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("MainWindow", "... select form to edit", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = qtw_sNC->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = qtw_sNC->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("MainWindow", "Uid", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = qtw_sNC->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("MainWindow", "Type", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = qtw_sNC->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("MainWindow", "MaxClients", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = qtw_sNC->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("MainWindow", "CPU", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem5 = qtw_sNC->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QApplication::translate("MainWindow", "Memory", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem6 = qtw_sNC->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QApplication::translate("MainWindow", "ScriptPath", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem7 = qtw_sNC->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QApplication::translate("MainWindow", "LogPath", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem8 = qtw_sNC->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QApplication::translate("MainWindow", "Submit", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem9 = qtw_sNC->horizontalHeaderItem(9);
        ___qtablewidgetitem9->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem10 = qtw_sNC->horizontalHeaderItem(10);
        ___qtablewidgetitem10->setText(QApplication::translate("MainWindow", "LoginTheEnd", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("MainWindow", "Server::NominalClient", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("MainWindow", "Server::NominalHost", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem11 = qtw_sNH->horizontalHeaderItem(0);
        ___qtablewidgetitem11->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem12 = qtw_sNH->horizontalHeaderItem(1);
        ___qtablewidgetitem12->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem13 = qtw_sNH->horizontalHeaderItem(2);
        ___qtablewidgetitem13->setText(QApplication::translate("MainWindow", "Interface", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem14 = qtw_sNH->horizontalHeaderItem(3);
        ___qtablewidgetitem14->setText(QApplication::translate("MainWindow", "OS", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem15 = qtw_sNH->horizontalHeaderItem(4);
        ___qtablewidgetitem15->setText(QApplication::translate("MainWindow", "CPUNumb", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem16 = qtw_sNH->horizontalHeaderItem(5);
        ___qtablewidgetitem16->setText(QApplication::translate("MainWindow", "Memory", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem17 = qtw_sNH->horizontalHeaderItem(6);
        ___qtablewidgetitem17->setText(QApplication::translate("MainWindow", "Clock", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem18 = qtw_sNH->horizontalHeaderItem(7);
        ___qtablewidgetitem18->setText(QApplication::translate("MainWindow", "ClientsAllowed", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem19 = qtw_sNH->horizontalHeaderItem(8);
        ___qtablewidgetitem19->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("MainWindow", "Server::NominalKiller", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem20 = qtw_sNK->horizontalHeaderItem(0);
        ___qtablewidgetitem20->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem21 = qtw_sNK->horizontalHeaderItem(1);
        ___qtablewidgetitem21->setText(QApplication::translate("MainWindow", "Uid", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem22 = qtw_sNK->horizontalHeaderItem(2);
        ___qtablewidgetitem22->setText(QApplication::translate("MainWindow", "Type", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem23 = qtw_sNK->horizontalHeaderItem(3);
        ___qtablewidgetitem23->setText(QApplication::translate("MainWindow", "MaxClients", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem24 = qtw_sNK->horizontalHeaderItem(4);
        ___qtablewidgetitem24->setText(QApplication::translate("MainWindow", "CPU", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem25 = qtw_sNK->horizontalHeaderItem(5);
        ___qtablewidgetitem25->setText(QApplication::translate("MainWindow", "Memory", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem26 = qtw_sNK->horizontalHeaderItem(6);
        ___qtablewidgetitem26->setText(QApplication::translate("MainWindow", "ScriptPath", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem27 = qtw_sNK->horizontalHeaderItem(7);
        ___qtablewidgetitem27->setText(QApplication::translate("MainWindow", "LogPath", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem28 = qtw_sNK->horizontalHeaderItem(8);
        ___qtablewidgetitem28->setText(QApplication::translate("MainWindow", "Submit", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem29 = qtw_sNK->horizontalHeaderItem(9);
        ___qtablewidgetitem29->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem30 = qtw_sNK->horizontalHeaderItem(10);
        ___qtablewidgetitem30->setText(QApplication::translate("MainWindow", "LoginTheEnd", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("MainWindow", "Server::SetEnvironment", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem31 = qtw_sSE->horizontalHeaderItem(0);
        ___qtablewidgetitem31->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem32 = qtw_sSE->horizontalHeaderItem(1);
        ___qtablewidgetitem32->setText(QApplication::translate("MainWindow", "EnvironmentVar", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem33 = qtw_sSE->horizontalHeaderItem(2);
        ___qtablewidgetitem33->setText(QApplication::translate("MainWindow", "Path", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("MainWindow", "Producer::NominalClient", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem34 = qtw_pNC->horizontalHeaderItem(0);
        ___qtablewidgetitem34->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem35 = qtw_pNC->horizontalHeaderItem(1);
        ___qtablewidgetitem35->setText(QApplication::translate("MainWindow", "Uid", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem36 = qtw_pNC->horizontalHeaderItem(2);
        ___qtablewidgetitem36->setText(QApplication::translate("MainWindow", "Type", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem37 = qtw_pNC->horizontalHeaderItem(3);
        ___qtablewidgetitem37->setText(QApplication::translate("MainWindow", "MaxClients", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem38 = qtw_pNC->horizontalHeaderItem(4);
        ___qtablewidgetitem38->setText(QApplication::translate("MainWindow", "CPU", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem39 = qtw_pNC->horizontalHeaderItem(5);
        ___qtablewidgetitem39->setText(QApplication::translate("MainWindow", "Memory", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem40 = qtw_pNC->horizontalHeaderItem(6);
        ___qtablewidgetitem40->setText(QApplication::translate("MainWindow", "ScriptPath", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem41 = qtw_pNC->horizontalHeaderItem(7);
        ___qtablewidgetitem41->setText(QApplication::translate("MainWindow", "LogPath", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem42 = qtw_pNC->horizontalHeaderItem(8);
        ___qtablewidgetitem42->setText(QApplication::translate("MainWindow", "Submit", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem43 = qtw_pNC->horizontalHeaderItem(9);
        ___qtablewidgetitem43->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem44 = qtw_pNC->horizontalHeaderItem(10);
        ___qtablewidgetitem44->setText(QApplication::translate("MainWindow", "LoginTheEnd", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("MainWindow", "Producer::ActiveClient", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem45 = qtw_pAC->horizontalHeaderItem(0);
        ___qtablewidgetitem45->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem46 = qtw_pAC->horizontalHeaderItem(1);
        ___qtablewidgetitem46->setText(QApplication::translate("MainWindow", "Uid", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem47 = qtw_pAC->horizontalHeaderItem(2);
        ___qtablewidgetitem47->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem48 = qtw_pAC->horizontalHeaderItem(3);
        ___qtablewidgetitem48->setText(QApplication::translate("MainWindow", "Pid", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem49 = qtw_pAC->horizontalHeaderItem(4);
        ___qtablewidgetitem49->setText(QApplication::translate("MainWindow", "Threads", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem50 = qtw_pAC->horizontalHeaderItem(5);
        ___qtablewidgetitem50->setText(QApplication::translate("MainWindow", "Mips", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem51 = qtw_pAC->horizontalHeaderItem(6);
        ___qtablewidgetitem51->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem52 = qtw_pAC->horizontalHeaderItem(7);
        ___qtablewidgetitem52->setText(QApplication::translate("MainWindow", "Type", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem53 = qtw_pAC->horizontalHeaderItem(8);
        ___qtablewidgetitem53->setText(QApplication::translate("MainWindow", "ExitStatus", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem54 = qtw_pAC->horizontalHeaderItem(9);
        ___qtablewidgetitem54->setText(QApplication::translate("MainWindow", "TimeOut", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem55 = qtw_pAC->horizontalHeaderItem(10);
        ___qtablewidgetitem55->setText(QApplication::translate("MainWindow", "LastUpd", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("MainWindow", "Producer::NominalHost", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem56 = qtw_pNH->horizontalHeaderItem(0);
        ___qtablewidgetitem56->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem57 = qtw_pNH->horizontalHeaderItem(1);
        ___qtablewidgetitem57->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem58 = qtw_pNH->horizontalHeaderItem(2);
        ___qtablewidgetitem58->setText(QApplication::translate("MainWindow", "Interface", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem59 = qtw_pNH->horizontalHeaderItem(3);
        ___qtablewidgetitem59->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem60 = qtw_pNH->horizontalHeaderItem(4);
        ___qtablewidgetitem60->setText(QApplication::translate("MainWindow", "CliRunning", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem61 = qtw_pNH->horizontalHeaderItem(5);
        ___qtablewidgetitem61->setText(QApplication::translate("MainWindow", "CliAllowed", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem62 = qtw_pNH->horizontalHeaderItem(6);
        ___qtablewidgetitem62->setText(QApplication::translate("MainWindow", "CliProcessed", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem63 = qtw_pNH->horizontalHeaderItem(7);
        ___qtablewidgetitem63->setText(QApplication::translate("MainWindow", "CliFailed", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem64 = qtw_pNH->horizontalHeaderItem(8);
        ___qtablewidgetitem64->setText(QApplication::translate("MainWindow", "CliKilled", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem65 = qtw_pNH->horizontalHeaderItem(9);
        ___qtablewidgetitem65->setText(QApplication::translate("MainWindow", "LastFailed", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem66 = qtw_pNH->horizontalHeaderItem(10);
        ___qtablewidgetitem66->setText(QApplication::translate("MainWindow", "LastUpdate", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem67 = qtw_pNH->horizontalHeaderItem(11);
        ___qtablewidgetitem67->setText(QApplication::translate("MainWindow", "Clock", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("MainWindow", "Producer::NominalNTuple", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem68 = qtw_pNNT->horizontalHeaderItem(0);
        ___qtablewidgetitem68->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem69 = qtw_pNNT->horizontalHeaderItem(1);
        ___qtablewidgetitem69->setText(QApplication::translate("MainWindow", "Uid", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem70 = qtw_pNNT->horizontalHeaderItem(2);
        ___qtablewidgetitem70->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem71 = qtw_pNNT->horizontalHeaderItem(3);
        ___qtablewidgetitem71->setText(QApplication::translate("MainWindow", "OutputDirPath", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem72 = qtw_pNNT->horizontalHeaderItem(4);
        ___qtablewidgetitem72->setText(QApplication::translate("MainWindow", "RunMode", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem73 = qtw_pNNT->horizontalHeaderItem(5);
        ___qtablewidgetitem73->setText(QApplication::translate("MainWindow", "UpdFreq", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem74 = qtw_pNNT->horizontalHeaderItem(6);
        ___qtablewidgetitem74->setText(QApplication::translate("MainWindow", "DstType", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("MainWindow", "Producer::ProducedNTuple", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem75 = qtw_pPNT->horizontalHeaderItem(0);
        ___qtablewidgetitem75->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem76 = qtw_pPNT->horizontalHeaderItem(1);
        ___qtablewidgetitem76->setText(QApplication::translate("MainWindow", "Run", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem77 = qtw_pPNT->horizontalHeaderItem(2);
        ___qtablewidgetitem77->setText(QApplication::translate("MainWindow", "Time", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem78 = qtw_pPNT->horizontalHeaderItem(3);
        ___qtablewidgetitem78->setText(QApplication::translate("MainWindow", "FirstEvent", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem79 = qtw_pPNT->horizontalHeaderItem(4);
        ___qtablewidgetitem79->setText(QApplication::translate("MainWindow", "LastEvent", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem80 = qtw_pPNT->horizontalHeaderItem(5);
        ___qtablewidgetitem80->setText(QApplication::translate("MainWindow", "Path", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem81 = qtw_pPNT->horizontalHeaderItem(6);
        ___qtablewidgetitem81->setText(QApplication::translate("MainWindow", "Version", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem82 = qtw_pPNT->horizontalHeaderItem(7);
        ___qtablewidgetitem82->setText(QApplication::translate("MainWindow", "Size", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem83 = qtw_pPNT->horizontalHeaderItem(8);
        ___qtablewidgetitem83->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem84 = qtw_pPNT->horizontalHeaderItem(9);
        ___qtablewidgetitem84->setText(QApplication::translate("MainWindow", "Type", 0, QApplication::UnicodeUTF8));
        label_20->setText(QApplication::translate("MainWindow", "Producer::RunTable", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem85 = qtw_pRT->horizontalHeaderItem(0);
        ___qtablewidgetitem85->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem86 = qtw_pRT->horizontalHeaderItem(1);
        ___qtablewidgetitem86->setText(QApplication::translate("MainWindow", "Run", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem87 = qtw_pRT->horizontalHeaderItem(2);
        ___qtablewidgetitem87->setText(QApplication::translate("MainWindow", "Uid", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem88 = qtw_pRT->horizontalHeaderItem(3);
        ___qtablewidgetitem88->setText(QApplication::translate("MainWindow", "CUid", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem89 = qtw_pRT->horizontalHeaderItem(4);
        ___qtablewidgetitem89->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem90 = qtw_pRT->horizontalHeaderItem(5);
        ___qtablewidgetitem90->setText(QApplication::translate("MainWindow", "FirstEvent", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem91 = qtw_pRT->horizontalHeaderItem(6);
        ___qtablewidgetitem91->setText(QApplication::translate("MainWindow", "LastEvent", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem92 = qtw_pRT->horizontalHeaderItem(7);
        ___qtablewidgetitem92->setText(QApplication::translate("MainWindow", "TEvent", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem93 = qtw_pRT->horizontalHeaderItem(8);
        ___qtablewidgetitem93->setText(QApplication::translate("MainWindow", "TLEvent", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem94 = qtw_pRT->horizontalHeaderItem(9);
        ___qtablewidgetitem94->setText(QApplication::translate("MainWindow", "Priority", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem95 = qtw_pRT->horizontalHeaderItem(10);
        ___qtablewidgetitem95->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem96 = qtw_pRT->horizontalHeaderItem(11);
        ___qtablewidgetitem96->setText(QApplication::translate("MainWindow", "History", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem97 = qtw_pRT->horizontalHeaderItem(12);
        ___qtablewidgetitem97->setText(QApplication::translate("MainWindow", "Fails", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem98 = qtw_pRT->horizontalHeaderItem(13);
        ___qtablewidgetitem98->setText(QApplication::translate("MainWindow", "DataMC", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem99 = qtw_pRT->horizontalHeaderItem(14);
        ___qtablewidgetitem99->setText(QApplication::translate("MainWindow", "FilePath", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem100 = qtw_pRT->horizontalHeaderItem(15);
        ___qtablewidgetitem100->setText(QApplication::translate("MainWindow", "SubmTime", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem101 = qtw_pRT->horizontalHeaderItem(16);
        ___qtablewidgetitem101->setText(QApplication::translate("MainWindow", "TimeSpent", 0, QApplication::UnicodeUTF8));
        pbtRemove->setText(QApplication::translate("MainWindow", "Rm <ctrl>r", 0, QApplication::UnicodeUTF8));
        pbtNew->setText(QApplication::translate("MainWindow", "New <ctrl>n", 0, QApplication::UnicodeUTF8));
        pbtMoveUp->setText(QApplication::translate("MainWindow", "MvUp", 0, QApplication::UnicodeUTF8));
        pbtMoveDown->setText(QApplication::translate("MainWindow", "MvDn", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget2->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("MainWindow", "Forms", 0, QApplication::UnicodeUTF8));
        menuFiles->setTitle(QApplication::translate("MainWindow", "DB", 0, QApplication::UnicodeUTF8));
        menuPreferences->setTitle(QApplication::translate("MainWindow", "Preferences", 0, QApplication::UnicodeUTF8));
        menuTools->setTitle(QApplication::translate("MainWindow", "Tools", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

public slots:
    virtual void mnuOpen() {};
    virtual void mnuNew() {};
    virtual void mnuNew2() {};
    virtual void mnuRecover() {};
    virtual void mnuUpdate() {};
    virtual void mnuCommit() {};
    virtual void mnuQuit() {};
    virtual void mnuZoomIn() {};
    virtual void mnuZoomOut() {};
    virtual void mnuUseFilter() {};
    virtual void mnuExportDB() {};
    virtual void mnuImportDB() {};

    virtual void clkTreeWidget2(QTreeWidgetItem *, int)=0;
	virtual void clkbtRemove(bool)=0;
	virtual void clkbtNew(bool)=0;
	virtual void clkbtMoveUp(bool)=0;
	virtual void clkbtMoveDown(bool)=0;
};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

