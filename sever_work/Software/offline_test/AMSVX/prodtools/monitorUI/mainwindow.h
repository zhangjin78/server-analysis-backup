/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
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
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QToolBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class qtabUtils: public QObject
{
	Q_OBJECT

public:
	qtabUtils() { };
	virtual ~qtabUtils() {};

public slots:
	virtual void clktbHeader(int) {};
	virtual void clkcSelection(int, int) {};
	virtual void clkSelection(int, int) {};
	virtual void dblclkSelection(int, int) {};

protected:
	virtual bool eventFilter(QObject *, QEvent *);
	friend class myQMainWindow;
};

class Ui_MainWindow: public QObject
{
	Q_OBJECT

public:
    QAction *actActiveHosts;
    QAction *actUpdateStatus;
    QAction *actQuit;
    QAction *actResetHosts;
    QAction *actDeleteValidatedDST;
    QAction *actDeleteFailedDST;
    QAction *actZoomIn;
    QAction *actZoomOut;
    QAction *actDeleteRuns;
    QAction *actFinishFailedRuns;
    QAction *actResetHistory;
    QAction *actLogToCout;
    QAction *actClearMyLogging;
    QAction *actLockUpdate;
    QAction *actExportDB;
    QAction *actExportDBMerged;
    QAction *actResetFailedHosts;
    QAction *actUpdateAFSToken;
    QAction *actUseFilter;
    QAction *actSetFilter;
    QAction *actLockFreeHost;
    QAction *actDeleteActiveClients;
    QAction *actOnlyReset;
    QAction *actResetRemoveDST;
    QAction *actImportDB;
    QWidget *centralWidget;
    QGridLayout *gridLayout_4;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QGridLayout *gridLayout_5;
    QTreeWidget *treeWidget;
    QWidget *page_2;
    QGridLayout *gridLayout_2;
    QTreeWidget *treeWidget2;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout_3;
    QLabel *label;
    QTableWidget *qtwActiveHosts;
    QLabel *label_2;
    QTableWidget *qtwActiveClients;
    QWidget *tab_2;
    QGridLayout *gridLayout_7;
    QLabel *label_3;
    QTableWidget *qtwProducerRuns;
    QTableWidget *qtwProducerNTuples;
    QLabel *label_4;
    QWidget *tab_4;
    QGridLayout *gridLayout_12;
    QGridLayout *gridLayout_11;
    QTableWidget *qtwDiskUsage;
    QLabel *label_7;
    QTableWidget *qtwDBServActClients;
    QTableWidget *qtwServActClients;
    QLabel *label_6;
    QLabel *label_5;
    QWidget *tab_3;
    QGridLayout *gridLayout;
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
    QPushButton *pbtReplace;
    QPushButton *pbtAdd;
    QPushButton *pbtRemove;
    QPushButton *pbtReplaceAll;
    QPushButton *pbtDummy2;
    QPushButton *pbtNew;
    QPushButton *pbtMoveUp;
    QPushButton *pbtMoveDown;
    QWidget *tab_5;
    QGridLayout *gridLayout_8;
    QLabel *label_8;
    QTableWidget *qtwVrdLog;
    QLabel *label_21;
    QPlainTextEdit *qtwMyLogging;
    QLabel *label_9;
    QTableWidget *qtwOtherLog;
    QMenuBar *menuBar;
    QMenu *menuFiles;
    QMenu *menuResetFailedRuns;
    QMenu *menuPreferences;
    QMenu *menuTools;
    QStatusBar *statusBar;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1134, 728);
        actActiveHosts = new QAction(MainWindow);
        actActiveHosts->setObjectName(QString::fromUtf8("actActiveHosts"));
        actActiveHosts->setCheckable(true);
        actActiveHosts->setChecked(true);
        actUpdateStatus = new QAction(MainWindow);
        actUpdateStatus->setObjectName(QString::fromUtf8("actUpdateStatus"));
        actQuit = new QAction(MainWindow);
        actQuit->setObjectName(QString::fromUtf8("actQuit"));
        actResetHosts = new QAction(MainWindow);
        actResetHosts->setObjectName(QString::fromUtf8("actResetHosts"));
        actDeleteValidatedDST = new QAction(MainWindow);
        actDeleteValidatedDST->setObjectName(QString::fromUtf8("actDeleteValidatedDST"));
        actDeleteFailedDST = new QAction(MainWindow);
        actDeleteFailedDST->setObjectName(QString::fromUtf8("actDeleteFailedDST"));
        actZoomIn = new QAction(MainWindow);
        actZoomIn->setObjectName(QString::fromUtf8("actZoomIn"));
        actZoomOut = new QAction(MainWindow);
        actZoomOut->setObjectName(QString::fromUtf8("actZoomOut"));
        actDeleteRuns = new QAction(MainWindow);
        actDeleteRuns->setObjectName(QString::fromUtf8("actDeleteRuns"));
        actFinishFailedRuns = new QAction(MainWindow);
        actFinishFailedRuns->setObjectName(QString::fromUtf8("actFinishFailedRuns"));
        actResetHistory = new QAction(MainWindow);
        actResetHistory->setObjectName(QString::fromUtf8("actResetHistory"));
        actLogToCout = new QAction(MainWindow);
        actLogToCout->setObjectName(QString::fromUtf8("actLogToCout"));
        actLogToCout->setCheckable(true);
        actLogToCout->setChecked(true);
        actClearMyLogging = new QAction(MainWindow);
        actClearMyLogging->setObjectName(QString::fromUtf8("actClearMyLogging"));
        actLockUpdate = new QAction(MainWindow);
        actLockUpdate->setObjectName(QString::fromUtf8("actLockUpdate"));
        actLockUpdate->setCheckable(true);
        actExportDB = new QAction(MainWindow);
        actExportDB->setObjectName(QString::fromUtf8("actExportDB"));
        actExportDBMerged = new QAction(MainWindow);
        actExportDBMerged->setObjectName(QString::fromUtf8("actExportDBMerged"));
        actResetFailedHosts = new QAction(MainWindow);
        actResetFailedHosts->setObjectName(QString::fromUtf8("actResetFailedHosts"));
        actUpdateAFSToken = new QAction(MainWindow);
        actUpdateAFSToken->setObjectName(QString::fromUtf8("actUpdateAFSToken"));
        actUseFilter = new QAction(MainWindow);
        actUseFilter->setObjectName(QString::fromUtf8("actUseFilter"));
        actUseFilter->setCheckable(true);
        actSetFilter = new QAction(MainWindow);
        actSetFilter->setObjectName(QString::fromUtf8("actSetFilter"));
        actLockFreeHost = new QAction(MainWindow);
        actLockFreeHost->setObjectName(QString::fromUtf8("actLockFreeHost"));
        actLockFreeHost->setCheckable(true);
        actDeleteActiveClients = new QAction(MainWindow);
        actDeleteActiveClients->setObjectName(QString::fromUtf8("actDeleteActiveClients"));
        actOnlyReset = new QAction(MainWindow);
        actOnlyReset->setObjectName(QString::fromUtf8("actOnlyReset"));
        actResetRemoveDST = new QAction(MainWindow);
        actResetRemoveDST->setObjectName(QString::fromUtf8("actResetRemoveDST"));
        actImportDB = new QAction(MainWindow);
        actImportDB->setObjectName(QString::fromUtf8("actImportDB"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout_4 = new QGridLayout(centralWidget);
        gridLayout_4->setSpacing(0);
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        stackedWidget = new QStackedWidget(centralWidget);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(stackedWidget->sizePolicy().hasHeightForWidth());
        stackedWidget->setSizePolicy(sizePolicy);
        stackedWidget->setMinimumSize(QSize(200, 0));
        stackedWidget->setMaximumSize(QSize(200, 16777215));
        QFont font;
        font.setFamily(QString::fromUtf8("DejaVu LGC Serif Condensed"));
        stackedWidget->setFont(font);
        stackedWidget->setFrameShape(QFrame::Box);
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        gridLayout_5 = new QGridLayout(page);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        treeWidget = new QTreeWidget(page);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("DejaVu LGC Serif Condensed"));
        font1.setPointSize(9);
        treeWidget->setFont(font1);
        treeWidget->setTextElideMode(Qt::ElideMiddle);

        gridLayout_5->addWidget(treeWidget, 0, 0, 1, 1);

        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName(QString::fromUtf8("page_2"));
        gridLayout_2 = new QGridLayout(page_2);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        treeWidget2 = new QTreeWidget(page_2);
        treeWidget2->setObjectName(QString::fromUtf8("treeWidget2"));

        gridLayout_2->addWidget(treeWidget2, 0, 0, 1, 1);

        stackedWidget->addWidget(page_2);

        gridLayout_4->addWidget(stackedWidget, 0, 0, 1, 1);

        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("DejaVu LGC Sans"));
        font2.setPointSize(9);
        tabWidget->setFont(font2);
        tabWidget->setAutoFillBackground(false);
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setElideMode(Qt::ElideNone);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout_3 = new QGridLayout(tab);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label = new QLabel(tab);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);
        label->setMinimumSize(QSize(0, 12));
        label->setMaximumSize(QSize(16777215, 12));
        QFont font3;
        font3.setFamily(QString::fromUtf8("DejaVu LGC Sans"));
        font3.setPointSize(8);
        label->setFont(font3);

        gridLayout_3->addWidget(label, 0, 0, 1, 1);

        qtwActiveHosts = new QTableWidget(tab);
        if (qtwActiveHosts->columnCount() < 17)
            qtwActiveHosts->setColumnCount(17);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        qtwActiveHosts->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        qtwActiveHosts->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        qtwActiveHosts->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        qtwActiveHosts->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        qtwActiveHosts->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        qtwActiveHosts->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        qtwActiveHosts->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        qtwActiveHosts->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        qtwActiveHosts->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        qtwActiveHosts->setHorizontalHeaderItem(9, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        qtwActiveHosts->setHorizontalHeaderItem(10, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        qtwActiveHosts->setHorizontalHeaderItem(11, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        qtwActiveHosts->setHorizontalHeaderItem(12, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        qtwActiveHosts->setHorizontalHeaderItem(13, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        qtwActiveHosts->setHorizontalHeaderItem(14, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        qtwActiveHosts->setHorizontalHeaderItem(15, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        qtwActiveHosts->setHorizontalHeaderItem(16, __qtablewidgetitem16);
        qtwActiveHosts->setObjectName(QString::fromUtf8("qtwActiveHosts"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(qtwActiveHosts->sizePolicy().hasHeightForWidth());
        qtwActiveHosts->setSizePolicy(sizePolicy2);
        qtwActiveHosts->setMinimumSize(QSize(0, 0));
        qtwActiveHosts->setMaximumSize(QSize(16777215, 280));
        qtwActiveHosts->setFont(font1);
        qtwActiveHosts->setFrameShadow(QFrame::Raised);
        qtwActiveHosts->setShowGrid(false);
        qtwActiveHosts->setGridStyle(Qt::SolidLine);

        gridLayout_3->addWidget(qtwActiveHosts, 1, 0, 1, 1);

        label_2 = new QLabel(tab);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy1);
        label_2->setMinimumSize(QSize(0, 20));
        label_2->setMaximumSize(QSize(16777215, 20));
        label_2->setFont(font3);

        gridLayout_3->addWidget(label_2, 2, 0, 1, 1);

        qtwActiveClients = new QTableWidget(tab);
        if (qtwActiveClients->columnCount() < 14)
            qtwActiveClients->setColumnCount(14);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        qtwActiveClients->setHorizontalHeaderItem(0, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        qtwActiveClients->setHorizontalHeaderItem(1, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        qtwActiveClients->setHorizontalHeaderItem(2, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        qtwActiveClients->setHorizontalHeaderItem(3, __qtablewidgetitem20);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        qtwActiveClients->setHorizontalHeaderItem(4, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        qtwActiveClients->setHorizontalHeaderItem(5, __qtablewidgetitem22);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        qtwActiveClients->setHorizontalHeaderItem(6, __qtablewidgetitem23);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        qtwActiveClients->setHorizontalHeaderItem(7, __qtablewidgetitem24);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        qtwActiveClients->setHorizontalHeaderItem(8, __qtablewidgetitem25);
        QTableWidgetItem *__qtablewidgetitem26 = new QTableWidgetItem();
        qtwActiveClients->setHorizontalHeaderItem(9, __qtablewidgetitem26);
        QTableWidgetItem *__qtablewidgetitem27 = new QTableWidgetItem();
        qtwActiveClients->setHorizontalHeaderItem(10, __qtablewidgetitem27);
        QTableWidgetItem *__qtablewidgetitem28 = new QTableWidgetItem();
        qtwActiveClients->setHorizontalHeaderItem(11, __qtablewidgetitem28);
        QTableWidgetItem *__qtablewidgetitem29 = new QTableWidgetItem();
        qtwActiveClients->setHorizontalHeaderItem(12, __qtablewidgetitem29);
        QTableWidgetItem *__qtablewidgetitem30 = new QTableWidgetItem();
        qtwActiveClients->setHorizontalHeaderItem(13, __qtablewidgetitem30);
        qtwActiveClients->setObjectName(QString::fromUtf8("qtwActiveClients"));
        sizePolicy2.setHeightForWidth(qtwActiveClients->sizePolicy().hasHeightForWidth());
        qtwActiveClients->setSizePolicy(sizePolicy2);
        qtwActiveClients->setMinimumSize(QSize(0, 320));
        qtwActiveClients->setFont(font1);
        qtwActiveClients->setFrameShadow(QFrame::Plain);
        qtwActiveClients->setShowGrid(false);
        qtwActiveClients->setGridStyle(Qt::SolidLine);

        gridLayout_3->addWidget(qtwActiveClients, 3, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        gridLayout_7 = new QGridLayout(tab_2);
        gridLayout_7->setSpacing(6);
        gridLayout_7->setContentsMargins(11, 11, 11, 11);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        label_3 = new QLabel(tab_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);
        label_3->setMinimumSize(QSize(0, 12));
        label_3->setMaximumSize(QSize(16777215, 12));
        label_3->setFont(font3);

        gridLayout_7->addWidget(label_3, 0, 0, 1, 1);

        qtwProducerRuns = new QTableWidget(tab_2);
        if (qtwProducerRuns->columnCount() < 12)
            qtwProducerRuns->setColumnCount(12);
        QTableWidgetItem *__qtablewidgetitem31 = new QTableWidgetItem();
        qtwProducerRuns->setHorizontalHeaderItem(0, __qtablewidgetitem31);
        QTableWidgetItem *__qtablewidgetitem32 = new QTableWidgetItem();
        qtwProducerRuns->setHorizontalHeaderItem(1, __qtablewidgetitem32);
        QTableWidgetItem *__qtablewidgetitem33 = new QTableWidgetItem();
        qtwProducerRuns->setHorizontalHeaderItem(2, __qtablewidgetitem33);
        QTableWidgetItem *__qtablewidgetitem34 = new QTableWidgetItem();
        qtwProducerRuns->setHorizontalHeaderItem(3, __qtablewidgetitem34);
        QTableWidgetItem *__qtablewidgetitem35 = new QTableWidgetItem();
        qtwProducerRuns->setHorizontalHeaderItem(4, __qtablewidgetitem35);
        QTableWidgetItem *__qtablewidgetitem36 = new QTableWidgetItem();
        qtwProducerRuns->setHorizontalHeaderItem(5, __qtablewidgetitem36);
        QTableWidgetItem *__qtablewidgetitem37 = new QTableWidgetItem();
        qtwProducerRuns->setHorizontalHeaderItem(6, __qtablewidgetitem37);
        QTableWidgetItem *__qtablewidgetitem38 = new QTableWidgetItem();
        qtwProducerRuns->setHorizontalHeaderItem(7, __qtablewidgetitem38);
        QTableWidgetItem *__qtablewidgetitem39 = new QTableWidgetItem();
        qtwProducerRuns->setHorizontalHeaderItem(8, __qtablewidgetitem39);
        QTableWidgetItem *__qtablewidgetitem40 = new QTableWidgetItem();
        qtwProducerRuns->setHorizontalHeaderItem(9, __qtablewidgetitem40);
        QTableWidgetItem *__qtablewidgetitem41 = new QTableWidgetItem();
        qtwProducerRuns->setHorizontalHeaderItem(10, __qtablewidgetitem41);
        QTableWidgetItem *__qtablewidgetitem42 = new QTableWidgetItem();
        qtwProducerRuns->setHorizontalHeaderItem(11, __qtablewidgetitem42);
        qtwProducerRuns->setObjectName(QString::fromUtf8("qtwProducerRuns"));
        sizePolicy2.setHeightForWidth(qtwProducerRuns->sizePolicy().hasHeightForWidth());
        qtwProducerRuns->setSizePolicy(sizePolicy2);
        qtwProducerRuns->setMinimumSize(QSize(0, 0));
        qtwProducerRuns->setMaximumSize(QSize(16777215, 320));
        qtwProducerRuns->setSizeIncrement(QSize(0, 0));
        qtwProducerRuns->setFont(font1);
        qtwProducerRuns->setShowGrid(false);

        gridLayout_7->addWidget(qtwProducerRuns, 1, 0, 1, 1);

        qtwProducerNTuples = new QTableWidget(tab_2);
        if (qtwProducerNTuples->columnCount() < 9)
            qtwProducerNTuples->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem43 = new QTableWidgetItem();
        qtwProducerNTuples->setHorizontalHeaderItem(0, __qtablewidgetitem43);
        QTableWidgetItem *__qtablewidgetitem44 = new QTableWidgetItem();
        qtwProducerNTuples->setHorizontalHeaderItem(1, __qtablewidgetitem44);
        QTableWidgetItem *__qtablewidgetitem45 = new QTableWidgetItem();
        qtwProducerNTuples->setHorizontalHeaderItem(2, __qtablewidgetitem45);
        QTableWidgetItem *__qtablewidgetitem46 = new QTableWidgetItem();
        qtwProducerNTuples->setHorizontalHeaderItem(3, __qtablewidgetitem46);
        QTableWidgetItem *__qtablewidgetitem47 = new QTableWidgetItem();
        qtwProducerNTuples->setHorizontalHeaderItem(4, __qtablewidgetitem47);
        QTableWidgetItem *__qtablewidgetitem48 = new QTableWidgetItem();
        qtwProducerNTuples->setHorizontalHeaderItem(5, __qtablewidgetitem48);
        QTableWidgetItem *__qtablewidgetitem49 = new QTableWidgetItem();
        qtwProducerNTuples->setHorizontalHeaderItem(6, __qtablewidgetitem49);
        QTableWidgetItem *__qtablewidgetitem50 = new QTableWidgetItem();
        qtwProducerNTuples->setHorizontalHeaderItem(7, __qtablewidgetitem50);
        QTableWidgetItem *__qtablewidgetitem51 = new QTableWidgetItem();
        qtwProducerNTuples->setHorizontalHeaderItem(8, __qtablewidgetitem51);
        qtwProducerNTuples->setObjectName(QString::fromUtf8("qtwProducerNTuples"));
        sizePolicy2.setHeightForWidth(qtwProducerNTuples->sizePolicy().hasHeightForWidth());
        qtwProducerNTuples->setSizePolicy(sizePolicy2);
        qtwProducerNTuples->setMinimumSize(QSize(0, 300));
        qtwProducerNTuples->setMaximumSize(QSize(16777215, 16777215));
        qtwProducerNTuples->setSizeIncrement(QSize(0, 0));
        qtwProducerNTuples->setFont(font1);
        qtwProducerNTuples->setShowGrid(false);

        gridLayout_7->addWidget(qtwProducerNTuples, 3, 0, 1, 1);

        label_4 = new QLabel(tab_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        sizePolicy1.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy1);
        label_4->setMinimumSize(QSize(0, 12));
        label_4->setMaximumSize(QSize(16777215, 12));
        label_4->setFont(font3);

        gridLayout_7->addWidget(label_4, 2, 0, 1, 1);

        tabWidget->addTab(tab_2, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        gridLayout_12 = new QGridLayout(tab_4);
        gridLayout_12->setSpacing(6);
        gridLayout_12->setContentsMargins(11, 11, 11, 11);
        gridLayout_12->setObjectName(QString::fromUtf8("gridLayout_12"));
        gridLayout_11 = new QGridLayout();
        gridLayout_11->setSpacing(0);
        gridLayout_11->setObjectName(QString::fromUtf8("gridLayout_11"));
        qtwDiskUsage = new QTableWidget(tab_4);
        if (qtwDiskUsage->columnCount() < 5)
            qtwDiskUsage->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem52 = new QTableWidgetItem();
        qtwDiskUsage->setHorizontalHeaderItem(0, __qtablewidgetitem52);
        QTableWidgetItem *__qtablewidgetitem53 = new QTableWidgetItem();
        qtwDiskUsage->setHorizontalHeaderItem(1, __qtablewidgetitem53);
        QTableWidgetItem *__qtablewidgetitem54 = new QTableWidgetItem();
        qtwDiskUsage->setHorizontalHeaderItem(2, __qtablewidgetitem54);
        QTableWidgetItem *__qtablewidgetitem55 = new QTableWidgetItem();
        qtwDiskUsage->setHorizontalHeaderItem(3, __qtablewidgetitem55);
        QTableWidgetItem *__qtablewidgetitem56 = new QTableWidgetItem();
        qtwDiskUsage->setHorizontalHeaderItem(4, __qtablewidgetitem56);
        qtwDiskUsage->setObjectName(QString::fromUtf8("qtwDiskUsage"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(qtwDiskUsage->sizePolicy().hasHeightForWidth());
        qtwDiskUsage->setSizePolicy(sizePolicy3);
        qtwDiskUsage->setMinimumSize(QSize(0, 300));
        qtwDiskUsage->setMaximumSize(QSize(16777215, 16777215));
        qtwDiskUsage->setFont(font1);
        qtwDiskUsage->setShowGrid(false);

        gridLayout_11->addWidget(qtwDiskUsage, 5, 0, 1, 1);

        label_7 = new QLabel(tab_4);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        sizePolicy1.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy1);
        label_7->setMinimumSize(QSize(0, 20));
        label_7->setMaximumSize(QSize(16777215, 20));
        label_7->setFont(font3);

        gridLayout_11->addWidget(label_7, 4, 0, 1, 1);

        qtwDBServActClients = new QTableWidget(tab_4);
        if (qtwDBServActClients->columnCount() < 9)
            qtwDBServActClients->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem57 = new QTableWidgetItem();
        qtwDBServActClients->setHorizontalHeaderItem(0, __qtablewidgetitem57);
        QTableWidgetItem *__qtablewidgetitem58 = new QTableWidgetItem();
        qtwDBServActClients->setHorizontalHeaderItem(1, __qtablewidgetitem58);
        QTableWidgetItem *__qtablewidgetitem59 = new QTableWidgetItem();
        qtwDBServActClients->setHorizontalHeaderItem(2, __qtablewidgetitem59);
        QTableWidgetItem *__qtablewidgetitem60 = new QTableWidgetItem();
        qtwDBServActClients->setHorizontalHeaderItem(3, __qtablewidgetitem60);
        QTableWidgetItem *__qtablewidgetitem61 = new QTableWidgetItem();
        qtwDBServActClients->setHorizontalHeaderItem(4, __qtablewidgetitem61);
        QTableWidgetItem *__qtablewidgetitem62 = new QTableWidgetItem();
        qtwDBServActClients->setHorizontalHeaderItem(5, __qtablewidgetitem62);
        QTableWidgetItem *__qtablewidgetitem63 = new QTableWidgetItem();
        qtwDBServActClients->setHorizontalHeaderItem(6, __qtablewidgetitem63);
        QTableWidgetItem *__qtablewidgetitem64 = new QTableWidgetItem();
        qtwDBServActClients->setHorizontalHeaderItem(7, __qtablewidgetitem64);
        QTableWidgetItem *__qtablewidgetitem65 = new QTableWidgetItem();
        qtwDBServActClients->setHorizontalHeaderItem(8, __qtablewidgetitem65);
        qtwDBServActClients->setObjectName(QString::fromUtf8("qtwDBServActClients"));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(qtwDBServActClients->sizePolicy().hasHeightForWidth());
        qtwDBServActClients->setSizePolicy(sizePolicy4);
        qtwDBServActClients->setMinimumSize(QSize(0, 100));
        qtwDBServActClients->setMaximumSize(QSize(16777215, 100));
        qtwDBServActClients->setFont(font1);
        qtwDBServActClients->setShowGrid(false);

        gridLayout_11->addWidget(qtwDBServActClients, 1, 0, 1, 1);

        qtwServActClients = new QTableWidget(tab_4);
        if (qtwServActClients->columnCount() < 9)
            qtwServActClients->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem66 = new QTableWidgetItem();
        qtwServActClients->setHorizontalHeaderItem(0, __qtablewidgetitem66);
        QTableWidgetItem *__qtablewidgetitem67 = new QTableWidgetItem();
        qtwServActClients->setHorizontalHeaderItem(1, __qtablewidgetitem67);
        QTableWidgetItem *__qtablewidgetitem68 = new QTableWidgetItem();
        qtwServActClients->setHorizontalHeaderItem(2, __qtablewidgetitem68);
        QTableWidgetItem *__qtablewidgetitem69 = new QTableWidgetItem();
        qtwServActClients->setHorizontalHeaderItem(3, __qtablewidgetitem69);
        QTableWidgetItem *__qtablewidgetitem70 = new QTableWidgetItem();
        qtwServActClients->setHorizontalHeaderItem(4, __qtablewidgetitem70);
        QTableWidgetItem *__qtablewidgetitem71 = new QTableWidgetItem();
        qtwServActClients->setHorizontalHeaderItem(5, __qtablewidgetitem71);
        QTableWidgetItem *__qtablewidgetitem72 = new QTableWidgetItem();
        qtwServActClients->setHorizontalHeaderItem(6, __qtablewidgetitem72);
        QTableWidgetItem *__qtablewidgetitem73 = new QTableWidgetItem();
        qtwServActClients->setHorizontalHeaderItem(7, __qtablewidgetitem73);
        QTableWidgetItem *__qtablewidgetitem74 = new QTableWidgetItem();
        qtwServActClients->setHorizontalHeaderItem(8, __qtablewidgetitem74);
        qtwServActClients->setObjectName(QString::fromUtf8("qtwServActClients"));
        sizePolicy4.setHeightForWidth(qtwServActClients->sizePolicy().hasHeightForWidth());
        qtwServActClients->setSizePolicy(sizePolicy4);
        qtwServActClients->setMinimumSize(QSize(0, 100));
        qtwServActClients->setMaximumSize(QSize(16777215, 100));
        qtwServActClients->setFont(font1);
        qtwServActClients->setShowGrid(false);

        gridLayout_11->addWidget(qtwServActClients, 3, 0, 1, 1);

        label_6 = new QLabel(tab_4);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        sizePolicy1.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy1);
        label_6->setMinimumSize(QSize(0, 20));
        label_6->setMaximumSize(QSize(16777215, 20));
        label_6->setFont(font3);

        gridLayout_11->addWidget(label_6, 2, 0, 1, 1);

        label_5 = new QLabel(tab_4);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        sizePolicy1.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy1);
        label_5->setMinimumSize(QSize(0, 20));
        label_5->setMaximumSize(QSize(16777215, 20));
        label_5->setFont(font3);

        gridLayout_11->addWidget(label_5, 0, 0, 1, 1);


        gridLayout_12->addLayout(gridLayout_11, 4, 0, 1, 1);

        tabWidget->addTab(tab_4, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        gridLayout = new QGridLayout(tab_3);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        stackedWidget2 = new QStackedWidget(tab_3);
        stackedWidget2->setObjectName(QString::fromUtf8("stackedWidget2"));
        QFont font4;
        font4.setPointSize(8);
        stackedWidget2->setFont(font4);
        stackedWidget2->setFrameShape(QFrame::Box);
        page_4 = new QWidget();
        page_4->setObjectName(QString::fromUtf8("page_4"));
        gridLayout_23 = new QGridLayout(page_4);
        gridLayout_23->setSpacing(6);
        gridLayout_23->setContentsMargins(11, 11, 11, 11);
        gridLayout_23->setObjectName(QString::fromUtf8("gridLayout_23"));
        label_10 = new QLabel(page_4);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        QFont font5;
        font5.setPointSize(20);
        label_10->setFont(font5);
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
        QTableWidgetItem *__qtablewidgetitem75 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(0, __qtablewidgetitem75);
        QTableWidgetItem *__qtablewidgetitem76 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(1, __qtablewidgetitem76);
        QTableWidgetItem *__qtablewidgetitem77 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(2, __qtablewidgetitem77);
        QTableWidgetItem *__qtablewidgetitem78 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(3, __qtablewidgetitem78);
        QTableWidgetItem *__qtablewidgetitem79 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(4, __qtablewidgetitem79);
        QTableWidgetItem *__qtablewidgetitem80 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(5, __qtablewidgetitem80);
        QTableWidgetItem *__qtablewidgetitem81 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(6, __qtablewidgetitem81);
        QTableWidgetItem *__qtablewidgetitem82 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(7, __qtablewidgetitem82);
        QTableWidgetItem *__qtablewidgetitem83 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(8, __qtablewidgetitem83);
        QTableWidgetItem *__qtablewidgetitem84 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(9, __qtablewidgetitem84);
        QTableWidgetItem *__qtablewidgetitem85 = new QTableWidgetItem();
        qtw_sNC->setHorizontalHeaderItem(10, __qtablewidgetitem85);
        qtw_sNC->setObjectName(QString::fromUtf8("qtw_sNC"));
        qtw_sNC->setFont(font1);

        gridLayout_13->addWidget(qtw_sNC, 1, 0, 1, 1);

        label_11 = new QLabel(page_3);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setFont(font4);

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
        label_12->setFont(font4);

        gridLayout_14->addWidget(label_12, 0, 0, 1, 1);

        qtw_sNH = new QTableWidget(page_5);
        if (qtw_sNH->columnCount() < 9)
            qtw_sNH->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem86 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(0, __qtablewidgetitem86);
        QTableWidgetItem *__qtablewidgetitem87 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(1, __qtablewidgetitem87);
        QTableWidgetItem *__qtablewidgetitem88 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(2, __qtablewidgetitem88);
        QTableWidgetItem *__qtablewidgetitem89 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(3, __qtablewidgetitem89);
        QTableWidgetItem *__qtablewidgetitem90 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(4, __qtablewidgetitem90);
        QTableWidgetItem *__qtablewidgetitem91 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(5, __qtablewidgetitem91);
        QTableWidgetItem *__qtablewidgetitem92 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(6, __qtablewidgetitem92);
        QTableWidgetItem *__qtablewidgetitem93 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(7, __qtablewidgetitem93);
        QTableWidgetItem *__qtablewidgetitem94 = new QTableWidgetItem();
        qtw_sNH->setHorizontalHeaderItem(8, __qtablewidgetitem94);
        qtw_sNH->setObjectName(QString::fromUtf8("qtw_sNH"));
        qtw_sNH->setFont(font1);

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
        label_13->setFont(font4);

        gridLayout_15->addWidget(label_13, 0, 0, 1, 1);

        qtw_sNK = new QTableWidget(page_6);
        if (qtw_sNK->columnCount() < 11)
            qtw_sNK->setColumnCount(11);
        QTableWidgetItem *__qtablewidgetitem95 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(0, __qtablewidgetitem95);
        QTableWidgetItem *__qtablewidgetitem96 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(1, __qtablewidgetitem96);
        QTableWidgetItem *__qtablewidgetitem97 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(2, __qtablewidgetitem97);
        QTableWidgetItem *__qtablewidgetitem98 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(3, __qtablewidgetitem98);
        QTableWidgetItem *__qtablewidgetitem99 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(4, __qtablewidgetitem99);
        QTableWidgetItem *__qtablewidgetitem100 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(5, __qtablewidgetitem100);
        QTableWidgetItem *__qtablewidgetitem101 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(6, __qtablewidgetitem101);
        QTableWidgetItem *__qtablewidgetitem102 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(7, __qtablewidgetitem102);
        QTableWidgetItem *__qtablewidgetitem103 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(8, __qtablewidgetitem103);
        QTableWidgetItem *__qtablewidgetitem104 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(9, __qtablewidgetitem104);
        QTableWidgetItem *__qtablewidgetitem105 = new QTableWidgetItem();
        qtw_sNK->setHorizontalHeaderItem(10, __qtablewidgetitem105);
        qtw_sNK->setObjectName(QString::fromUtf8("qtw_sNK"));
        qtw_sNK->setFont(font1);

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
        label_14->setFont(font4);

        gridLayout_16->addWidget(label_14, 0, 0, 1, 1);

        qtw_sSE = new QTableWidget(page_7);
        if (qtw_sSE->columnCount() < 3)
            qtw_sSE->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem106 = new QTableWidgetItem();
        qtw_sSE->setHorizontalHeaderItem(0, __qtablewidgetitem106);
        QTableWidgetItem *__qtablewidgetitem107 = new QTableWidgetItem();
        qtw_sSE->setHorizontalHeaderItem(1, __qtablewidgetitem107);
        QTableWidgetItem *__qtablewidgetitem108 = new QTableWidgetItem();
        qtw_sSE->setHorizontalHeaderItem(2, __qtablewidgetitem108);
        qtw_sSE->setObjectName(QString::fromUtf8("qtw_sSE"));
        qtw_sSE->setFont(font1);

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
        label_15->setFont(font4);

        gridLayout_17->addWidget(label_15, 0, 0, 1, 1);

        qtw_pNC = new QTableWidget(page_8);
        if (qtw_pNC->columnCount() < 11)
            qtw_pNC->setColumnCount(11);
        QTableWidgetItem *__qtablewidgetitem109 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(0, __qtablewidgetitem109);
        QTableWidgetItem *__qtablewidgetitem110 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(1, __qtablewidgetitem110);
        QTableWidgetItem *__qtablewidgetitem111 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(2, __qtablewidgetitem111);
        QTableWidgetItem *__qtablewidgetitem112 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(3, __qtablewidgetitem112);
        QTableWidgetItem *__qtablewidgetitem113 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(4, __qtablewidgetitem113);
        QTableWidgetItem *__qtablewidgetitem114 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(5, __qtablewidgetitem114);
        QTableWidgetItem *__qtablewidgetitem115 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(6, __qtablewidgetitem115);
        QTableWidgetItem *__qtablewidgetitem116 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(7, __qtablewidgetitem116);
        QTableWidgetItem *__qtablewidgetitem117 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(8, __qtablewidgetitem117);
        QTableWidgetItem *__qtablewidgetitem118 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(9, __qtablewidgetitem118);
        QTableWidgetItem *__qtablewidgetitem119 = new QTableWidgetItem();
        qtw_pNC->setHorizontalHeaderItem(10, __qtablewidgetitem119);
        qtw_pNC->setObjectName(QString::fromUtf8("qtw_pNC"));
        qtw_pNC->setFont(font1);

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
        label_16->setFont(font4);

        gridLayout_18->addWidget(label_16, 0, 0, 1, 1);

        qtw_pAC = new QTableWidget(page_9);
        if (qtw_pAC->columnCount() < 9)
            qtw_pAC->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem120 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(0, __qtablewidgetitem120);
        QTableWidgetItem *__qtablewidgetitem121 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(1, __qtablewidgetitem121);
        QTableWidgetItem *__qtablewidgetitem122 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(2, __qtablewidgetitem122);
        QTableWidgetItem *__qtablewidgetitem123 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(3, __qtablewidgetitem123);
        QTableWidgetItem *__qtablewidgetitem124 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(4, __qtablewidgetitem124);
        QTableWidgetItem *__qtablewidgetitem125 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(5, __qtablewidgetitem125);
        QTableWidgetItem *__qtablewidgetitem126 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(6, __qtablewidgetitem126);
        QTableWidgetItem *__qtablewidgetitem127 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(7, __qtablewidgetitem127);
        QTableWidgetItem *__qtablewidgetitem128 = new QTableWidgetItem();
        qtw_pAC->setHorizontalHeaderItem(8, __qtablewidgetitem128);
        qtw_pAC->setObjectName(QString::fromUtf8("qtw_pAC"));
        qtw_pAC->setFont(font1);

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
        label_17->setFont(font4);

        gridLayout_19->addWidget(label_17, 0, 0, 1, 1);

        qtw_pNH = new QTableWidget(page_10);
        if (qtw_pNH->columnCount() < 11)
            qtw_pNH->setColumnCount(11);
        QTableWidgetItem *__qtablewidgetitem129 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(0, __qtablewidgetitem129);
        QTableWidgetItem *__qtablewidgetitem130 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(1, __qtablewidgetitem130);
        QTableWidgetItem *__qtablewidgetitem131 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(2, __qtablewidgetitem131);
        QTableWidgetItem *__qtablewidgetitem132 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(3, __qtablewidgetitem132);
        QTableWidgetItem *__qtablewidgetitem133 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(4, __qtablewidgetitem133);
        QTableWidgetItem *__qtablewidgetitem134 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(5, __qtablewidgetitem134);
        QTableWidgetItem *__qtablewidgetitem135 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(6, __qtablewidgetitem135);
        QTableWidgetItem *__qtablewidgetitem136 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(7, __qtablewidgetitem136);
        QTableWidgetItem *__qtablewidgetitem137 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(8, __qtablewidgetitem137);
        QTableWidgetItem *__qtablewidgetitem138 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(9, __qtablewidgetitem138);
        QTableWidgetItem *__qtablewidgetitem139 = new QTableWidgetItem();
        qtw_pNH->setHorizontalHeaderItem(10, __qtablewidgetitem139);
        qtw_pNH->setObjectName(QString::fromUtf8("qtw_pNH"));
        qtw_pNH->setFont(font1);

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
        QTableWidgetItem *__qtablewidgetitem140 = new QTableWidgetItem();
        qtw_pNNT->setHorizontalHeaderItem(0, __qtablewidgetitem140);
        QTableWidgetItem *__qtablewidgetitem141 = new QTableWidgetItem();
        qtw_pNNT->setHorizontalHeaderItem(1, __qtablewidgetitem141);
        QTableWidgetItem *__qtablewidgetitem142 = new QTableWidgetItem();
        qtw_pNNT->setHorizontalHeaderItem(2, __qtablewidgetitem142);
        QTableWidgetItem *__qtablewidgetitem143 = new QTableWidgetItem();
        qtw_pNNT->setHorizontalHeaderItem(3, __qtablewidgetitem143);
        QTableWidgetItem *__qtablewidgetitem144 = new QTableWidgetItem();
        qtw_pNNT->setHorizontalHeaderItem(4, __qtablewidgetitem144);
        QTableWidgetItem *__qtablewidgetitem145 = new QTableWidgetItem();
        qtw_pNNT->setHorizontalHeaderItem(5, __qtablewidgetitem145);
        QTableWidgetItem *__qtablewidgetitem146 = new QTableWidgetItem();
        qtw_pNNT->setHorizontalHeaderItem(6, __qtablewidgetitem146);
        qtw_pNNT->setObjectName(QString::fromUtf8("qtw_pNNT"));
        qtw_pNNT->setFont(font1);

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
        QTableWidgetItem *__qtablewidgetitem147 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(0, __qtablewidgetitem147);
        QTableWidgetItem *__qtablewidgetitem148 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(1, __qtablewidgetitem148);
        QTableWidgetItem *__qtablewidgetitem149 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(2, __qtablewidgetitem149);
        QTableWidgetItem *__qtablewidgetitem150 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(3, __qtablewidgetitem150);
        QTableWidgetItem *__qtablewidgetitem151 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(4, __qtablewidgetitem151);
        QTableWidgetItem *__qtablewidgetitem152 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(5, __qtablewidgetitem152);
        QTableWidgetItem *__qtablewidgetitem153 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(6, __qtablewidgetitem153);
        QTableWidgetItem *__qtablewidgetitem154 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(7, __qtablewidgetitem154);
        QTableWidgetItem *__qtablewidgetitem155 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(8, __qtablewidgetitem155);
        QTableWidgetItem *__qtablewidgetitem156 = new QTableWidgetItem();
        qtw_pPNT->setHorizontalHeaderItem(9, __qtablewidgetitem156);
        qtw_pPNT->setObjectName(QString::fromUtf8("qtw_pPNT"));
        qtw_pPNT->setFont(font1);

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
        if (qtw_pRT->columnCount() < 16)
            qtw_pRT->setColumnCount(16);
        QTableWidgetItem *__qtablewidgetitem157 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(0, __qtablewidgetitem157);
        QTableWidgetItem *__qtablewidgetitem158 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(1, __qtablewidgetitem158);
        QTableWidgetItem *__qtablewidgetitem159 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(2, __qtablewidgetitem159);
        QTableWidgetItem *__qtablewidgetitem160 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(3, __qtablewidgetitem160);
        QTableWidgetItem *__qtablewidgetitem161 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(4, __qtablewidgetitem161);
        QTableWidgetItem *__qtablewidgetitem162 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(5, __qtablewidgetitem162);
        QTableWidgetItem *__qtablewidgetitem163 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(6, __qtablewidgetitem163);
        QTableWidgetItem *__qtablewidgetitem164 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(7, __qtablewidgetitem164);
        QTableWidgetItem *__qtablewidgetitem165 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(8, __qtablewidgetitem165);
        QTableWidgetItem *__qtablewidgetitem166 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(9, __qtablewidgetitem166);
        QTableWidgetItem *__qtablewidgetitem167 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(10, __qtablewidgetitem167);
        QTableWidgetItem *__qtablewidgetitem168 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(11, __qtablewidgetitem168);
        QTableWidgetItem *__qtablewidgetitem169 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(12, __qtablewidgetitem169);
        QTableWidgetItem *__qtablewidgetitem170 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(13, __qtablewidgetitem170);
        QTableWidgetItem *__qtablewidgetitem171 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(14, __qtablewidgetitem171);
        QTableWidgetItem *__qtablewidgetitem172 = new QTableWidgetItem();
        qtw_pRT->setHorizontalHeaderItem(15, __qtablewidgetitem172);
        qtw_pRT->setObjectName(QString::fromUtf8("qtw_pRT"));
        qtw_pRT->setFont(font1);

        gridLayout_22->addWidget(qtw_pRT, 1, 0, 1, 1);

        stackedWidget2->addWidget(page_13);

        gridLayout->addWidget(stackedWidget2, 0, 0, 1, 1);

        widget_2 = new QWidget(tab_3);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        sizePolicy1.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy1);
        widget_2->setFont(font4);
        verticalLayout = new QVBoxLayout(widget_2);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        pbtReplace = new QPushButton(widget_2);
        pbtReplace->setObjectName(QString::fromUtf8("pbtReplace"));
        pbtReplace->setEnabled(false);

        verticalLayout->addWidget(pbtReplace);

        pbtAdd = new QPushButton(widget_2);
        pbtAdd->setObjectName(QString::fromUtf8("pbtAdd"));
        pbtAdd->setEnabled(false);

        verticalLayout->addWidget(pbtAdd);

        pbtRemove = new QPushButton(widget_2);
        pbtRemove->setObjectName(QString::fromUtf8("pbtRemove"));
        pbtRemove->setEnabled(false);

        verticalLayout->addWidget(pbtRemove);

        pbtReplaceAll = new QPushButton(widget_2);
        pbtReplaceAll->setObjectName(QString::fromUtf8("pbtReplaceAll"));
        pbtReplaceAll->setEnabled(false);

        verticalLayout->addWidget(pbtReplaceAll);

        pbtDummy2 = new QPushButton(widget_2);
        pbtDummy2->setObjectName(QString::fromUtf8("pbtDummy2"));
        pbtDummy2->setEnabled(false);
        pbtDummy2->setFlat(true);

        verticalLayout->addWidget(pbtDummy2);

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


        gridLayout->addWidget(widget_2, 0, 1, 1, 1);

        tabWidget->addTab(tab_3, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QString::fromUtf8("tab_5"));
        gridLayout_8 = new QGridLayout(tab_5);
        gridLayout_8->setSpacing(6);
        gridLayout_8->setContentsMargins(11, 11, 11, 11);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        label_8 = new QLabel(tab_5);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        QSizePolicy sizePolicy5(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy5);
        label_8->setMinimumSize(QSize(0, 12));
        label_8->setMaximumSize(QSize(16777215, 12));
        label_8->setFont(font3);

        gridLayout_8->addWidget(label_8, 0, 0, 1, 1);

        qtwVrdLog = new QTableWidget(tab_5);
        if (qtwVrdLog->columnCount() < 4)
            qtwVrdLog->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem173 = new QTableWidgetItem();
        qtwVrdLog->setHorizontalHeaderItem(0, __qtablewidgetitem173);
        QTableWidgetItem *__qtablewidgetitem174 = new QTableWidgetItem();
        qtwVrdLog->setHorizontalHeaderItem(1, __qtablewidgetitem174);
        QTableWidgetItem *__qtablewidgetitem175 = new QTableWidgetItem();
        qtwVrdLog->setHorizontalHeaderItem(2, __qtablewidgetitem175);
        QTableWidgetItem *__qtablewidgetitem176 = new QTableWidgetItem();
        qtwVrdLog->setHorizontalHeaderItem(3, __qtablewidgetitem176);
        qtwVrdLog->setObjectName(QString::fromUtf8("qtwVrdLog"));
        sizePolicy4.setHeightForWidth(qtwVrdLog->sizePolicy().hasHeightForWidth());
        qtwVrdLog->setSizePolicy(sizePolicy4);
        qtwVrdLog->setMinimumSize(QSize(0, 92));
        qtwVrdLog->setMaximumSize(QSize(16777215, 92));
        qtwVrdLog->setFont(font1);
        qtwVrdLog->setShowGrid(false);

        gridLayout_8->addWidget(qtwVrdLog, 1, 0, 1, 1);

        label_21 = new QLabel(tab_5);
        label_21->setObjectName(QString::fromUtf8("label_21"));
        sizePolicy5.setHeightForWidth(label_21->sizePolicy().hasHeightForWidth());
        label_21->setSizePolicy(sizePolicy5);
        label_21->setMinimumSize(QSize(0, 12));
        label_21->setFont(font4);

        gridLayout_8->addWidget(label_21, 4, 0, 1, 1);

        qtwMyLogging = new QPlainTextEdit(tab_5);
        qtwMyLogging->setObjectName(QString::fromUtf8("qtwMyLogging"));
        QFont font6;
        font6.setFamily(QString::fromUtf8("DejaVu LGC Sans Mono"));
        qtwMyLogging->setFont(font6);
        qtwMyLogging->setReadOnly(true);

        gridLayout_8->addWidget(qtwMyLogging, 7, 0, 1, 1);

        label_9 = new QLabel(tab_5);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setFont(font3);

        gridLayout_8->addWidget(label_9, 6, 0, 1, 1);

        qtwOtherLog = new QTableWidget(tab_5);
        if (qtwOtherLog->columnCount() < 4)
            qtwOtherLog->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem177 = new QTableWidgetItem();
        qtwOtherLog->setHorizontalHeaderItem(0, __qtablewidgetitem177);
        QTableWidgetItem *__qtablewidgetitem178 = new QTableWidgetItem();
        qtwOtherLog->setHorizontalHeaderItem(1, __qtablewidgetitem178);
        QTableWidgetItem *__qtablewidgetitem179 = new QTableWidgetItem();
        qtwOtherLog->setHorizontalHeaderItem(2, __qtablewidgetitem179);
        QTableWidgetItem *__qtablewidgetitem180 = new QTableWidgetItem();
        qtwOtherLog->setHorizontalHeaderItem(3, __qtablewidgetitem180);
        qtwOtherLog->setObjectName(QString::fromUtf8("qtwOtherLog"));
        sizePolicy4.setHeightForWidth(qtwOtherLog->sizePolicy().hasHeightForWidth());
        qtwOtherLog->setSizePolicy(sizePolicy4);
        qtwOtherLog->setMinimumSize(QSize(0, 92));
        qtwOtherLog->setMaximumSize(QSize(16777215, 113));
        qtwOtherLog->setFont(font);
        qtwOtherLog->setShowGrid(false);

        gridLayout_8->addWidget(qtwOtherLog, 5, 0, 1, 1);

        tabWidget->addTab(tab_5, QString());

        gridLayout_4->addWidget(tabWidget, 0, 1, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1134, 23));
        menuFiles = new QMenu(menuBar);
        menuFiles->setObjectName(QString::fromUtf8("menuFiles"));
        menuResetFailedRuns = new QMenu(menuFiles);
        menuResetFailedRuns->setObjectName(QString::fromUtf8("menuResetFailedRuns"));
        menuPreferences = new QMenu(menuBar);
        menuPreferences->setObjectName(QString::fromUtf8("menuPreferences"));
        menuTools = new QMenu(menuBar);
        menuTools->setObjectName(QString::fromUtf8("menuTools"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        statusBar->setFont(font1);
        MainWindow->setStatusBar(statusBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        mainToolBar->setFont(font3);
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);

        menuBar->addAction(menuFiles->menuAction());
        menuBar->addAction(menuPreferences->menuAction());
        menuBar->addAction(menuTools->menuAction());
        menuFiles->addAction(actUpdateStatus);
        menuFiles->addSeparator();
        menuFiles->addAction(menuResetFailedRuns->menuAction());
        menuFiles->addAction(actResetHosts);
        menuFiles->addAction(actResetFailedHosts);
        menuFiles->addAction(actDeleteActiveClients);
        menuFiles->addAction(actDeleteRuns);
        menuFiles->addAction(actDeleteFailedDST);
        menuFiles->addAction(actDeleteValidatedDST);
        menuFiles->addAction(actFinishFailedRuns);
        menuFiles->addAction(actUpdateAFSToken);
        menuFiles->addAction(actResetHistory);
        menuFiles->addSeparator();
        menuFiles->addAction(actQuit);
        menuResetFailedRuns->addAction(actOnlyReset);
        menuResetFailedRuns->addAction(actResetRemoveDST);
        menuPreferences->addAction(actActiveHosts);
        menuPreferences->addAction(actLogToCout);
        menuPreferences->addSeparator();
        menuPreferences->addAction(actZoomIn);
        menuPreferences->addAction(actZoomOut);
        menuPreferences->addAction(actClearMyLogging);
        menuPreferences->addSeparator();
        menuPreferences->addAction(actLockUpdate);
        menuPreferences->addAction(actUseFilter);
        menuPreferences->addSeparator();
        menuTools->addAction(actExportDB);
        menuTools->addAction(actExportDBMerged);
        menuTools->addAction(actImportDB);
        menuTools->addSeparator();
        menuTools->addAction(actSetFilter);
        mainToolBar->addAction(actUpdateStatus);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actActiveHosts);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actLockUpdate);
        mainToolBar->addAction(actUseFilter);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(0);
        tabWidget->setCurrentIndex(0);
        stackedWidget2->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        actActiveHosts->setText(QApplication::translate("MainWindow", "Show only active hosts", 0, QApplication::UnicodeUTF8));
        actActiveHosts->setIconText(QApplication::translate("MainWindow", "OnlyActHosts", 0, QApplication::UnicodeUTF8));
        actUpdateStatus->setText(QApplication::translate("MainWindow", "Update Status            <alt>u", 0, QApplication::UnicodeUTF8));
        actUpdateStatus->setIconText(QApplication::translate("MainWindow", "UpdStatus", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actUpdateStatus->setToolTip(QApplication::translate("MainWindow", "Update Status", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actQuit->setText(QApplication::translate("MainWindow", "Quit                           <alt>q", 0, QApplication::UnicodeUTF8));
        actResetHosts->setText(QApplication::translate("MainWindow", "Reset Hosts               <alt>h", 0, QApplication::UnicodeUTF8));
        actResetHosts->setIconText(QApplication::translate("MainWindow", "Reset Hosts", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actResetHosts->setToolTip(QApplication::translate("MainWindow", "Reset Hosts", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actDeleteValidatedDST->setText(QApplication::translate("MainWindow", "Delete Validated DST <alt>v", 0, QApplication::UnicodeUTF8));
        actDeleteValidatedDST->setIconText(QApplication::translate("MainWindow", "Delete Validated DST", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actDeleteValidatedDST->setToolTip(QApplication::translate("MainWindow", "Delete Validated DST", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actDeleteFailedDST->setText(QApplication::translate("MainWindow", "Delete Failed DST", 0, QApplication::UnicodeUTF8));
        actZoomIn->setText(QApplication::translate("MainWindow", "Zoom In               <ctrl>+", 0, QApplication::UnicodeUTF8));
        actZoomIn->setIconText(QApplication::translate("MainWindow", "Zoom In", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actZoomIn->setToolTip(QApplication::translate("MainWindow", "Zoom In", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actZoomOut->setText(QApplication::translate("MainWindow", "Zoom Out            <ctrl>-", 0, QApplication::UnicodeUTF8));
        actZoomOut->setIconText(QApplication::translate("MainWindow", "Zoom Out", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actZoomOut->setToolTip(QApplication::translate("MainWindow", "Zoom Out", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actDeleteRuns->setText(QApplication::translate("MainWindow", "Delete Runs", 0, QApplication::UnicodeUTF8));
        actFinishFailedRuns->setText(QApplication::translate("MainWindow", "Finish Failed Runs", 0, QApplication::UnicodeUTF8));
        actResetHistory->setText(QApplication::translate("MainWindow", "Reset History", 0, QApplication::UnicodeUTF8));
        actLogToCout->setText(QApplication::translate("MainWindow", "Log to cout", 0, QApplication::UnicodeUTF8));
        actClearMyLogging->setText(QApplication::translate("MainWindow", "Clear my logging  <ctrl>x", 0, QApplication::UnicodeUTF8));
        actClearMyLogging->setIconText(QApplication::translate("MainWindow", "Clear my logging", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actClearMyLogging->setToolTip(QApplication::translate("MainWindow", "Clear my logging", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actLockUpdate->setText(QApplication::translate("MainWindow", "Lock update", 0, QApplication::UnicodeUTF8));
        actLockUpdate->setIconText(QApplication::translate("MainWindow", "LockUpd", 0, QApplication::UnicodeUTF8));
        actExportDB->setText(QApplication::translate("MainWindow", "Export to c++ BDB", 0, QApplication::UnicodeUTF8));
        actExportDBMerged->setText(QApplication::translate("MainWindow", "Export to perl BDB", 0, QApplication::UnicodeUTF8));
        actResetFailedHosts->setText(QApplication::translate("MainWindow", "Reset InActive Hosts  <alt>g", 0, QApplication::UnicodeUTF8));
        actResetFailedHosts->setIconText(QApplication::translate("MainWindow", "Reset InActive Hosts", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actResetFailedHosts->setToolTip(QApplication::translate("MainWindow", "Reset InActive Hosts", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actUpdateAFSToken->setText(QApplication::translate("MainWindow", "Update AFS Token", 0, QApplication::UnicodeUTF8));
        actUseFilter->setText(QApplication::translate("MainWindow", "Use filter", 0, QApplication::UnicodeUTF8));
        actUseFilter->setIconText(QApplication::translate("MainWindow", "UseFilter", 0, QApplication::UnicodeUTF8));
        actSetFilter->setText(QApplication::translate("MainWindow", "Set filter    <ctrl>f", 0, QApplication::UnicodeUTF8));
        actSetFilter->setIconText(QApplication::translate("MainWindow", "Set filter", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actSetFilter->setToolTip(QApplication::translate("MainWindow", "Set filter", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actLockFreeHost->setText(QApplication::translate("MainWindow", "Lock ::getFreeHost", 0, QApplication::UnicodeUTF8));
        actLockFreeHost->setIconText(QApplication::translate("MainWindow", "LockFreeHost", 0, QApplication::UnicodeUTF8));
        actDeleteActiveClients->setText(QApplication::translate("MainWindow", "Delete ActiveClients", 0, QApplication::UnicodeUTF8));
        actOnlyReset->setText(QApplication::translate("MainWindow", "Only Reset", 0, QApplication::UnicodeUTF8));
        actResetRemoveDST->setText(QApplication::translate("MainWindow", "Reset and Remove DST", 0, QApplication::UnicodeUTF8));
        actImportDB->setText(QApplication::translate("MainWindow", "Import from text file", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("MainWindow", "Data", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem1 = treeWidget2->headerItem();
        ___qtreewidgetitem1->setText(0, QApplication::translate("MainWindow", "Forms", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Producer_ActiveHosts", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = qtwActiveHosts->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = qtwActiveHosts->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = qtwActiveHosts->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("MainWindow", "Clients", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = qtwActiveHosts->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("MainWindow", "RunningPr", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = qtwActiveHosts->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("MainWindow", "AllowedPr", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem5 = qtwActiveHosts->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QApplication::translate("MainWindow", "Clock", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem6 = qtwActiveHosts->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QApplication::translate("MainWindow", "CliProcessed", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem7 = qtwActiveHosts->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QApplication::translate("MainWindow", "CliFailed", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem8 = qtwActiveHosts->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QApplication::translate("MainWindow", "NTuples", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem9 = qtwActiveHosts->horizontalHeaderItem(9);
        ___qtablewidgetitem9->setText(QApplication::translate("MainWindow", "EventTags", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem10 = qtwActiveHosts->horizontalHeaderItem(10);
        ___qtablewidgetitem10->setText(QApplication::translate("MainWindow", "Events", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem11 = qtwActiveHosts->horizontalHeaderItem(11);
        ___qtablewidgetitem11->setText(QApplication::translate("MainWindow", "%ofTotal", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem12 = qtwActiveHosts->horizontalHeaderItem(12);
        ___qtablewidgetitem12->setText(QApplication::translate("MainWindow", "Warnings", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem13 = qtwActiveHosts->horizontalHeaderItem(13);
        ___qtablewidgetitem13->setText(QApplication::translate("MainWindow", "Errors", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem14 = qtwActiveHosts->horizontalHeaderItem(14);
        ___qtablewidgetitem14->setText(QApplication::translate("MainWindow", "CPU/Event", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem15 = qtwActiveHosts->horizontalHeaderItem(15);
        ___qtablewidgetitem15->setText(QApplication::translate("MainWindow", "Efficiency", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem16 = qtwActiveHosts->horizontalHeaderItem(16);
        ___qtablewidgetitem16->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Producer_ActiveClients", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem17 = qtwActiveClients->horizontalHeaderItem(0);
        ___qtablewidgetitem17->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem18 = qtwActiveClients->horizontalHeaderItem(1);
        ___qtablewidgetitem18->setText(QApplication::translate("MainWindow", "Run", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem19 = qtwActiveClients->horizontalHeaderItem(2);
        ___qtablewidgetitem19->setText(QApplication::translate("MainWindow", "ID", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem20 = qtwActiveClients->horizontalHeaderItem(3);
        ___qtablewidgetitem20->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem21 = qtwActiveClients->horizontalHeaderItem(4);
        ___qtablewidgetitem21->setText(QApplication::translate("MainWindow", "Mips", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem22 = qtwActiveClients->horizontalHeaderItem(5);
        ___qtablewidgetitem22->setText(QApplication::translate("MainWindow", "Threads", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem23 = qtwActiveClients->horizontalHeaderItem(6);
        ___qtablewidgetitem23->setText(QApplication::translate("MainWindow", "ProcessID", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem24 = qtwActiveClients->horizontalHeaderItem(7);
        ___qtablewidgetitem24->setText(QApplication::translate("MainWindow", "StartTime", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem25 = qtwActiveClients->horizontalHeaderItem(8);
        ___qtablewidgetitem25->setText(QApplication::translate("MainWindow", "LastUpdate", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem26 = qtwActiveClients->horizontalHeaderItem(9);
        ___qtablewidgetitem26->setText(QApplication::translate("MainWindow", "TimeOut", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem27 = qtwActiveClients->horizontalHeaderItem(10);
        ___qtablewidgetitem27->setText(QApplication::translate("MainWindow", "EvLeft", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem28 = qtwActiveClients->horizontalHeaderItem(11);
        ___qtablewidgetitem28->setText(QApplication::translate("MainWindow", "FinishIn", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem29 = qtwActiveClients->horizontalHeaderItem(12);
        ___qtablewidgetitem29->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem30 = qtwActiveClients->horizontalHeaderItem(13);
        ___qtablewidgetitem30->setText(QApplication::translate("MainWindow", "FileName", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Producer processes", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Producer_Runs", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem31 = qtwProducerRuns->horizontalHeaderItem(0);
        ___qtablewidgetitem31->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem32 = qtwProducerRuns->horizontalHeaderItem(1);
        ___qtablewidgetitem32->setText(QApplication::translate("MainWindow", "Run", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem33 = qtwProducerRuns->horizontalHeaderItem(2);
        ___qtablewidgetitem33->setText(QApplication::translate("MainWindow", "Uid", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem34 = qtwProducerRuns->horizontalHeaderItem(3);
        ___qtablewidgetitem34->setText(QApplication::translate("MainWindow", "Time", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem35 = qtwProducerRuns->horizontalHeaderItem(4);
        ___qtablewidgetitem35->setText(QApplication::translate("MainWindow", "FirstEvent", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem36 = qtwProducerRuns->horizontalHeaderItem(5);
        ___qtablewidgetitem36->setText(QApplication::translate("MainWindow", "LastEvent", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem37 = qtwProducerRuns->horizontalHeaderItem(6);
        ___qtablewidgetitem37->setText(QApplication::translate("MainWindow", "Priority", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem38 = qtwProducerRuns->horizontalHeaderItem(7);
        ___qtablewidgetitem38->setText(QApplication::translate("MainWindow", "History", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem39 = qtwProducerRuns->horizontalHeaderItem(8);
        ___qtablewidgetitem39->setText(QApplication::translate("MainWindow", "Fails", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem40 = qtwProducerRuns->horizontalHeaderItem(9);
        ___qtablewidgetitem40->setText(QApplication::translate("MainWindow", "DataMC", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem41 = qtwProducerRuns->horizontalHeaderItem(10);
        ___qtablewidgetitem41->setText(QApplication::translate("MainWindow", "Host", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem42 = qtwProducerRuns->horizontalHeaderItem(11);
        ___qtablewidgetitem42->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem43 = qtwProducerNTuples->horizontalHeaderItem(0);
        ___qtablewidgetitem43->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem44 = qtwProducerNTuples->horizontalHeaderItem(1);
        ___qtablewidgetitem44->setText(QApplication::translate("MainWindow", "Run", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem45 = qtwProducerNTuples->horizontalHeaderItem(2);
        ___qtablewidgetitem45->setText(QApplication::translate("MainWindow", "Time", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem46 = qtwProducerNTuples->horizontalHeaderItem(3);
        ___qtablewidgetitem46->setText(QApplication::translate("MainWindow", "FirstEvent", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem47 = qtwProducerNTuples->horizontalHeaderItem(4);
        ___qtablewidgetitem47->setText(QApplication::translate("MainWindow", "LastEvent", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem48 = qtwProducerNTuples->horizontalHeaderItem(5);
        ___qtablewidgetitem48->setText(QApplication::translate("MainWindow", "Name", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem49 = qtwProducerNTuples->horizontalHeaderItem(6);
        ___qtablewidgetitem49->setText(QApplication::translate("MainWindow", "CRC", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem50 = qtwProducerNTuples->horizontalHeaderItem(7);
        ___qtablewidgetitem50->setText(QApplication::translate("MainWindow", "Size(MB)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem51 = qtwProducerNTuples->horizontalHeaderItem(8);
        ___qtablewidgetitem51->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Producer_NTuples", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Producer i/o", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem52 = qtwDiskUsage->horizontalHeaderItem(0);
        ___qtablewidgetitem52->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem53 = qtwDiskUsage->horizontalHeaderItem(1);
        ___qtablewidgetitem53->setText(QApplication::translate("MainWindow", "FileSystem", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem54 = qtwDiskUsage->horizontalHeaderItem(2);
        ___qtablewidgetitem54->setText(QApplication::translate("MainWindow", "Total(MB)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem55 = qtwDiskUsage->horizontalHeaderItem(3);
        ___qtablewidgetitem55->setText(QApplication::translate("MainWindow", "Free(MB)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem56 = qtwDiskUsage->horizontalHeaderItem(4);
        ___qtablewidgetitem56->setText(QApplication::translate("MainWindow", "%Free", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "DiskUsage", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem57 = qtwDBServActClients->horizontalHeaderItem(0);
        ___qtablewidgetitem57->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem58 = qtwDBServActClients->horizontalHeaderItem(1);
        ___qtablewidgetitem58->setText(QApplication::translate("MainWindow", "ID", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem59 = qtwDBServActClients->horizontalHeaderItem(2);
        ___qtablewidgetitem59->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem60 = qtwDBServActClients->horizontalHeaderItem(3);
        ___qtablewidgetitem60->setText(QApplication::translate("MainWindow", "Mips", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem61 = qtwDBServActClients->horizontalHeaderItem(4);
        ___qtablewidgetitem61->setText(QApplication::translate("MainWindow", "ProcessID", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem62 = qtwDBServActClients->horizontalHeaderItem(5);
        ___qtablewidgetitem62->setText(QApplication::translate("MainWindow", "StartTime", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem63 = qtwDBServActClients->horizontalHeaderItem(6);
        ___qtablewidgetitem63->setText(QApplication::translate("MainWindow", "LastUpdate", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem64 = qtwDBServActClients->horizontalHeaderItem(7);
        ___qtablewidgetitem64->setText(QApplication::translate("MainWindow", "TimeOut", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem65 = qtwDBServActClients->horizontalHeaderItem(8);
        ___qtablewidgetitem65->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem66 = qtwServActClients->horizontalHeaderItem(0);
        ___qtablewidgetitem66->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem67 = qtwServActClients->horizontalHeaderItem(1);
        ___qtablewidgetitem67->setText(QApplication::translate("MainWindow", "ID", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem68 = qtwServActClients->horizontalHeaderItem(2);
        ___qtablewidgetitem68->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem69 = qtwServActClients->horizontalHeaderItem(3);
        ___qtablewidgetitem69->setText(QApplication::translate("MainWindow", "Mips", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem70 = qtwServActClients->horizontalHeaderItem(4);
        ___qtablewidgetitem70->setText(QApplication::translate("MainWindow", "ProcessID", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem71 = qtwServActClients->horizontalHeaderItem(5);
        ___qtablewidgetitem71->setText(QApplication::translate("MainWindow", "StartTime", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem72 = qtwServActClients->horizontalHeaderItem(6);
        ___qtablewidgetitem72->setText(QApplication::translate("MainWindow", "LastUpdate", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem73 = qtwServActClients->horizontalHeaderItem(7);
        ___qtablewidgetitem73->setText(QApplication::translate("MainWindow", "TimeOut", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem74 = qtwServActClients->horizontalHeaderItem(8);
        ___qtablewidgetitem74->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "DBServer_ActiveClients", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "Server_ActiveClients", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("MainWindow", "Service", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("MainWindow", "... select form to edit", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem75 = qtw_sNC->horizontalHeaderItem(0);
        ___qtablewidgetitem75->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem76 = qtw_sNC->horizontalHeaderItem(1);
        ___qtablewidgetitem76->setText(QApplication::translate("MainWindow", "Uid", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem77 = qtw_sNC->horizontalHeaderItem(2);
        ___qtablewidgetitem77->setText(QApplication::translate("MainWindow", "Type", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem78 = qtw_sNC->horizontalHeaderItem(3);
        ___qtablewidgetitem78->setText(QApplication::translate("MainWindow", "MaxClients", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem79 = qtw_sNC->horizontalHeaderItem(4);
        ___qtablewidgetitem79->setText(QApplication::translate("MainWindow", "CPU", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem80 = qtw_sNC->horizontalHeaderItem(5);
        ___qtablewidgetitem80->setText(QApplication::translate("MainWindow", "Memory", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem81 = qtw_sNC->horizontalHeaderItem(6);
        ___qtablewidgetitem81->setText(QApplication::translate("MainWindow", "ScriptPath", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem82 = qtw_sNC->horizontalHeaderItem(7);
        ___qtablewidgetitem82->setText(QApplication::translate("MainWindow", "LogPath", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem83 = qtw_sNC->horizontalHeaderItem(8);
        ___qtablewidgetitem83->setText(QApplication::translate("MainWindow", "Submit", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem84 = qtw_sNC->horizontalHeaderItem(9);
        ___qtablewidgetitem84->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem85 = qtw_sNC->horizontalHeaderItem(10);
        ___qtablewidgetitem85->setText(QApplication::translate("MainWindow", "LoginTheEnd", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("MainWindow", "Server::NominalClient", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("MainWindow", "Server::NominalHost", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem86 = qtw_sNH->horizontalHeaderItem(0);
        ___qtablewidgetitem86->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem87 = qtw_sNH->horizontalHeaderItem(1);
        ___qtablewidgetitem87->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem88 = qtw_sNH->horizontalHeaderItem(2);
        ___qtablewidgetitem88->setText(QApplication::translate("MainWindow", "Interface", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem89 = qtw_sNH->horizontalHeaderItem(3);
        ___qtablewidgetitem89->setText(QApplication::translate("MainWindow", "OS", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem90 = qtw_sNH->horizontalHeaderItem(4);
        ___qtablewidgetitem90->setText(QApplication::translate("MainWindow", "CPUNumb", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem91 = qtw_sNH->horizontalHeaderItem(5);
        ___qtablewidgetitem91->setText(QApplication::translate("MainWindow", "Memory", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem92 = qtw_sNH->horizontalHeaderItem(6);
        ___qtablewidgetitem92->setText(QApplication::translate("MainWindow", "Clock", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem93 = qtw_sNH->horizontalHeaderItem(7);
        ___qtablewidgetitem93->setText(QApplication::translate("MainWindow", "ClientsAllowed", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem94 = qtw_sNH->horizontalHeaderItem(8);
        ___qtablewidgetitem94->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("MainWindow", "Server::NominalKiller", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem95 = qtw_sNK->horizontalHeaderItem(0);
        ___qtablewidgetitem95->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem96 = qtw_sNK->horizontalHeaderItem(1);
        ___qtablewidgetitem96->setText(QApplication::translate("MainWindow", "Uid", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem97 = qtw_sNK->horizontalHeaderItem(2);
        ___qtablewidgetitem97->setText(QApplication::translate("MainWindow", "Type", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem98 = qtw_sNK->horizontalHeaderItem(3);
        ___qtablewidgetitem98->setText(QApplication::translate("MainWindow", "MaxClients", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem99 = qtw_sNK->horizontalHeaderItem(4);
        ___qtablewidgetitem99->setText(QApplication::translate("MainWindow", "CPU", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem100 = qtw_sNK->horizontalHeaderItem(5);
        ___qtablewidgetitem100->setText(QApplication::translate("MainWindow", "Memory", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem101 = qtw_sNK->horizontalHeaderItem(6);
        ___qtablewidgetitem101->setText(QApplication::translate("MainWindow", "ScriptPath", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem102 = qtw_sNK->horizontalHeaderItem(7);
        ___qtablewidgetitem102->setText(QApplication::translate("MainWindow", "LogPath", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem103 = qtw_sNK->horizontalHeaderItem(8);
        ___qtablewidgetitem103->setText(QApplication::translate("MainWindow", "Submit", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem104 = qtw_sNK->horizontalHeaderItem(9);
        ___qtablewidgetitem104->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem105 = qtw_sNK->horizontalHeaderItem(10);
        ___qtablewidgetitem105->setText(QApplication::translate("MainWindow", "LoginTheEnd", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("MainWindow", "Server::SetEnvironment", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem106 = qtw_sSE->horizontalHeaderItem(0);
        ___qtablewidgetitem106->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem107 = qtw_sSE->horizontalHeaderItem(1);
        ___qtablewidgetitem107->setText(QApplication::translate("MainWindow", "EnvironmentVar", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem108 = qtw_sSE->horizontalHeaderItem(2);
        ___qtablewidgetitem108->setText(QApplication::translate("MainWindow", "Path", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("MainWindow", "Producer::NominalClient", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem109 = qtw_pNC->horizontalHeaderItem(0);
        ___qtablewidgetitem109->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem110 = qtw_pNC->horizontalHeaderItem(1);
        ___qtablewidgetitem110->setText(QApplication::translate("MainWindow", "Uid", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem111 = qtw_pNC->horizontalHeaderItem(2);
        ___qtablewidgetitem111->setText(QApplication::translate("MainWindow", "Type", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem112 = qtw_pNC->horizontalHeaderItem(3);
        ___qtablewidgetitem112->setText(QApplication::translate("MainWindow", "MaxClients", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem113 = qtw_pNC->horizontalHeaderItem(4);
        ___qtablewidgetitem113->setText(QApplication::translate("MainWindow", "CPU", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem114 = qtw_pNC->horizontalHeaderItem(5);
        ___qtablewidgetitem114->setText(QApplication::translate("MainWindow", "Memory", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem115 = qtw_pNC->horizontalHeaderItem(6);
        ___qtablewidgetitem115->setText(QApplication::translate("MainWindow", "ScriptPath", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem116 = qtw_pNC->horizontalHeaderItem(7);
        ___qtablewidgetitem116->setText(QApplication::translate("MainWindow", "LogPath", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem117 = qtw_pNC->horizontalHeaderItem(8);
        ___qtablewidgetitem117->setText(QApplication::translate("MainWindow", "Submit", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem118 = qtw_pNC->horizontalHeaderItem(9);
        ___qtablewidgetitem118->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem119 = qtw_pNC->horizontalHeaderItem(10);
        ___qtablewidgetitem119->setText(QApplication::translate("MainWindow", "LoginTheEnd", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("MainWindow", "Producer::ActiveClient", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem120 = qtw_pAC->horizontalHeaderItem(0);
        ___qtablewidgetitem120->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem121 = qtw_pAC->horizontalHeaderItem(1);
        ___qtablewidgetitem121->setText(QApplication::translate("MainWindow", "Uid", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem122 = qtw_pAC->horizontalHeaderItem(2);
        ___qtablewidgetitem122->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem123 = qtw_pAC->horizontalHeaderItem(3);
        ___qtablewidgetitem123->setText(QApplication::translate("MainWindow", "Mips", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem124 = qtw_pAC->horizontalHeaderItem(4);
        ___qtablewidgetitem124->setText(QApplication::translate("MainWindow", "Pid", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem125 = qtw_pAC->horizontalHeaderItem(5);
        ___qtablewidgetitem125->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem126 = qtw_pAC->horizontalHeaderItem(6);
        ___qtablewidgetitem126->setText(QApplication::translate("MainWindow", "Type", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem127 = qtw_pAC->horizontalHeaderItem(7);
        ___qtablewidgetitem127->setText(QApplication::translate("MainWindow", "TimeOut", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem128 = qtw_pAC->horizontalHeaderItem(8);
        ___qtablewidgetitem128->setText(QApplication::translate("MainWindow", "RunTable::FilePath", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("MainWindow", "Producer::NominalHost", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem129 = qtw_pNH->horizontalHeaderItem(0);
        ___qtablewidgetitem129->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem130 = qtw_pNH->horizontalHeaderItem(1);
        ___qtablewidgetitem130->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem131 = qtw_pNH->horizontalHeaderItem(2);
        ___qtablewidgetitem131->setText(QApplication::translate("MainWindow", "Interface", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem132 = qtw_pNH->horizontalHeaderItem(3);
        ___qtablewidgetitem132->setText(QApplication::translate("MainWindow", "OS", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem133 = qtw_pNH->horizontalHeaderItem(4);
        ___qtablewidgetitem133->setText(QApplication::translate("MainWindow", "CPUNumb", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem134 = qtw_pNH->horizontalHeaderItem(5);
        ___qtablewidgetitem134->setText(QApplication::translate("MainWindow", "Memory", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem135 = qtw_pNH->horizontalHeaderItem(6);
        ___qtablewidgetitem135->setText(QApplication::translate("MainWindow", "Clock", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem136 = qtw_pNH->horizontalHeaderItem(7);
        ___qtablewidgetitem136->setText(QApplication::translate("MainWindow", "AllowedPr", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem137 = qtw_pNH->horizontalHeaderItem(8);
        ___qtablewidgetitem137->setText(QApplication::translate("MainWindow", "CliProcessed", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem138 = qtw_pNH->horizontalHeaderItem(9);
        ___qtablewidgetitem138->setText(QApplication::translate("MainWindow", "CliFailed", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem139 = qtw_pNH->horizontalHeaderItem(10);
        ___qtablewidgetitem139->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("MainWindow", "Producer::NominalNTuple", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem140 = qtw_pNNT->horizontalHeaderItem(0);
        ___qtablewidgetitem140->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem141 = qtw_pNNT->horizontalHeaderItem(1);
        ___qtablewidgetitem141->setText(QApplication::translate("MainWindow", "Uid", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem142 = qtw_pNNT->horizontalHeaderItem(2);
        ___qtablewidgetitem142->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem143 = qtw_pNNT->horizontalHeaderItem(3);
        ___qtablewidgetitem143->setText(QApplication::translate("MainWindow", "OutputDirPath", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem144 = qtw_pNNT->horizontalHeaderItem(4);
        ___qtablewidgetitem144->setText(QApplication::translate("MainWindow", "RunMode", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem145 = qtw_pNNT->horizontalHeaderItem(5);
        ___qtablewidgetitem145->setText(QApplication::translate("MainWindow", "UpdFreq", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem146 = qtw_pNNT->horizontalHeaderItem(6);
        ___qtablewidgetitem146->setText(QApplication::translate("MainWindow", "DstType", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("MainWindow", "Producer::ProducedNTuple", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem147 = qtw_pPNT->horizontalHeaderItem(0);
        ___qtablewidgetitem147->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem148 = qtw_pPNT->horizontalHeaderItem(1);
        ___qtablewidgetitem148->setText(QApplication::translate("MainWindow", "Run", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem149 = qtw_pPNT->horizontalHeaderItem(2);
        ___qtablewidgetitem149->setText(QApplication::translate("MainWindow", "Time", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem150 = qtw_pPNT->horizontalHeaderItem(3);
        ___qtablewidgetitem150->setText(QApplication::translate("MainWindow", "FirstEvent", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem151 = qtw_pPNT->horizontalHeaderItem(4);
        ___qtablewidgetitem151->setText(QApplication::translate("MainWindow", "LastEvent", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem152 = qtw_pPNT->horizontalHeaderItem(5);
        ___qtablewidgetitem152->setText(QApplication::translate("MainWindow", "Path", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem153 = qtw_pPNT->horizontalHeaderItem(6);
        ___qtablewidgetitem153->setText(QApplication::translate("MainWindow", "Version", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem154 = qtw_pPNT->horizontalHeaderItem(7);
        ___qtablewidgetitem154->setText(QApplication::translate("MainWindow", "Size", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem155 = qtw_pPNT->horizontalHeaderItem(8);
        ___qtablewidgetitem155->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem156 = qtw_pPNT->horizontalHeaderItem(9);
        ___qtablewidgetitem156->setText(QApplication::translate("MainWindow", "Type", 0, QApplication::UnicodeUTF8));
        label_20->setText(QApplication::translate("MainWindow", "Producer::RunTable", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem157 = qtw_pRT->horizontalHeaderItem(0);
        ___qtablewidgetitem157->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem158 = qtw_pRT->horizontalHeaderItem(1);
        ___qtablewidgetitem158->setText(QApplication::translate("MainWindow", "Uid", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem159 = qtw_pRT->horizontalHeaderItem(2);
        ___qtablewidgetitem159->setText(QApplication::translate("MainWindow", "CUid", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem160 = qtw_pRT->horizontalHeaderItem(3);
        ___qtablewidgetitem160->setText(QApplication::translate("MainWindow", "HostName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem161 = qtw_pRT->horizontalHeaderItem(4);
        ___qtablewidgetitem161->setText(QApplication::translate("MainWindow", "Run", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem162 = qtw_pRT->horizontalHeaderItem(5);
        ___qtablewidgetitem162->setText(QApplication::translate("MainWindow", "FirstEvent", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem163 = qtw_pRT->horizontalHeaderItem(6);
        ___qtablewidgetitem163->setText(QApplication::translate("MainWindow", "LastEvent", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem164 = qtw_pRT->horizontalHeaderItem(7);
        ___qtablewidgetitem164->setText(QApplication::translate("MainWindow", "Priority", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem165 = qtw_pRT->horizontalHeaderItem(8);
        ___qtablewidgetitem165->setText(QApplication::translate("MainWindow", "FilePath", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem166 = qtw_pRT->horizontalHeaderItem(9);
        ___qtablewidgetitem166->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem167 = qtw_pRT->horizontalHeaderItem(10);
        ___qtablewidgetitem167->setText(QApplication::translate("MainWindow", "History", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem168 = qtw_pRT->horizontalHeaderItem(11);
        ___qtablewidgetitem168->setText(QApplication::translate("MainWindow", "StType", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem169 = qtw_pRT->horizontalHeaderItem(12);
        ___qtablewidgetitem169->setText(QApplication::translate("MainWindow", "Fails", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem170 = qtw_pRT->horizontalHeaderItem(13);
        ___qtablewidgetitem170->setText(QApplication::translate("MainWindow", "DataMC", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem171 = qtw_pRT->horizontalHeaderItem(14);
        ___qtablewidgetitem171->setText(QApplication::translate("MainWindow", "TEvent", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem172 = qtw_pRT->horizontalHeaderItem(15);
        ___qtablewidgetitem172->setText(QApplication::translate("MainWindow", "TLEvent", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pbtReplace->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        pbtReplace->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        pbtReplace->setText(QApplication::translate("MainWindow", "Upd <ctrl>u", 0, QApplication::UnicodeUTF8));
        pbtAdd->setText(QApplication::translate("MainWindow", "Add <ctrl>a", 0, QApplication::UnicodeUTF8));
        pbtRemove->setText(QApplication::translate("MainWindow", "Rm <ctrl>r", 0, QApplication::UnicodeUTF8));
        pbtReplaceAll->setText(QApplication::translate("MainWindow", "UpdAll", 0, QApplication::UnicodeUTF8));
        pbtDummy2->setText(QString());
        pbtNew->setText(QApplication::translate("MainWindow", "New", 0, QApplication::UnicodeUTF8));
        pbtMoveUp->setText(QApplication::translate("MainWindow", "MvUp", 0, QApplication::UnicodeUTF8));
        pbtMoveDown->setText(QApplication::translate("MainWindow", "MvDn", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainWindow", "Control", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainWindow", "Vrd log file info", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem173 = qtwVrdLog->horizontalHeaderItem(0);
        ___qtablewidgetitem173->setText(QApplication::translate("MainWindow", "Serv", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem174 = qtwVrdLog->horizontalHeaderItem(1);
        ___qtablewidgetitem174->setText(QApplication::translate("MainWindow", "VrdLog file", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem175 = qtwVrdLog->horizontalHeaderItem(2);
        ___qtablewidgetitem175->setText(QApplication::translate("MainWindow", "LastUpd time", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem176 = qtwVrdLog->horizontalHeaderItem(3);
        ___qtablewidgetitem176->setText(QApplication::translate("MainWindow", "Size", 0, QApplication::UnicodeUTF8));
        label_21->setText(QApplication::translate("MainWindow", "Other log file info", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MainWindow", "My logging", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem177 = qtwOtherLog->horizontalHeaderItem(0);
        ___qtablewidgetitem177->setText(QApplication::translate("MainWindow", "Log file", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem178 = qtwOtherLog->horizontalHeaderItem(1);
        ___qtablewidgetitem178->setText(QApplication::translate("MainWindow", "LastUpd time", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem179 = qtwOtherLog->horizontalHeaderItem(2);
        ___qtablewidgetitem179->setText(QApplication::translate("MainWindow", "Size", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem180 = qtwOtherLog->horizontalHeaderItem(3);
        ___qtablewidgetitem180->setText(QApplication::translate("MainWindow", "Comment", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_5), QApplication::translate("MainWindow", "Logs", 0, QApplication::UnicodeUTF8));
        menuFiles->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuResetFailedRuns->setTitle(QApplication::translate("MainWindow", "Reset Failed Runs", 0, QApplication::UnicodeUTF8));
        menuPreferences->setTitle(QApplication::translate("MainWindow", "Preferences", 0, QApplication::UnicodeUTF8));
        menuTools->setTitle(QApplication::translate("MainWindow", "Tools", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

    public slots:
        	virtual void slUpdateStatus()=0;
        	virtual void mnuResetHosts()=0;
        	virtual void mnuResetFailedHosts()=0;
        	virtual void mnuResetFailedRuns()=0;
        	virtual void mnuResetFailedRunsDST()=0;
        	virtual void mnuResetHistory()=0;
        	virtual void mnuDeleteValidatedDST()=0;
        	virtual void mnuDeleteFailedDST()=0;
    		virtual void mnuDeleteActiveClients()=0;
        	virtual void mnuDeleteRuns()=0;
        	virtual void mnuFinishFailedRuns()=0;
    		virtual void mnuUpdateAFSToken()=0;
        	virtual void mnuQuit()=0;
        	virtual void mnuActiveHosts()=0;
        	virtual void mnuLockUpdate()=0;
        	virtual void mnuUseFilter()=0;
        	virtual void mnuZoomIn()=0;
        	virtual void mnuZoomOut()=0;
        	virtual void mnuClearMyLogging()=0;
        	virtual void mnuExportDB()=0;
        	virtual void mnuExportDBMerged()=0;
        	virtual void mnuImportDB()=0;
        	virtual void mnuSetFilter()=0;
        	virtual void clkTreeWidget(QTreeWidgetItem *, int)=0;
        	virtual void clkTreeWidget2(QTreeWidgetItem *, int)=0;
        	virtual void tabChanged(int)=0;
        	virtual void clkbtReplace(bool)=0;
        	virtual void clkbtAdd(bool)=0;
        	virtual void clkbtRemove(bool)=0;
    		virtual void clkbtNew(bool)=0;
    		virtual void clkbtReplaceAll(bool)=0;
    		virtual void clkbtMoveUp(bool)=0;
    		virtual void clkbtMoveDown(bool)=0;
};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

