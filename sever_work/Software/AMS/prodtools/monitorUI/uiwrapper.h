#ifndef  _UIWRAPPER_H
#define  _UIWRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ios>
#include <sstream>
#include <ostream>
#include <set>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

using std::cout;
using std::endl;
using std::fstream;
using std::stringstream;
using std::set;

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QtCore/QEvent>
#include <QtCore/QSharedMemory>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QStyleFactory>
#include <QtGui/QButtonGroup>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QTableView>
#include <QtGui/QToolBar>
#include <QtGui/QTreeView>
#include <QtGui/QWidget>
#include <QtGui/QAction>
#include <QtGui/QHeaderView>
#include <QtGui/QFont>
#include <QtNetwork/QHostInfo>
#include <QtGui/QFileDialog>
#include <QtCore/QProcess>

#include "ManCommon.hxx"
#include "shmutils.h"
#include "srviface.h"
#include "mainwindow.h"
#include "warning.h"
#include "viewer.h"
#include "filter.h"
#include "editcolumn.h"
#include "selfile.h"

extern event __evAskData;
extern mutex __mxProcData;

//_____________________________________________________________________
class uistring: public string
{
public:
	uistring(): string() { };
	uistring(const std::string &s): string(s) { };
	uistring(const char *s): string(s) { };
	uistring(const QString &qs): string(qs.toUtf8().constData()) { };
	uistring(const QByteArray &ba): string(QString(ba).toUtf8().constData()) { };
public:
	QString q_str() const { return QString(c_str()); };
};

//_____________________________________________________________________
class uiEvent: public QEvent
{
public:
	typedef enum actType {
		ACT_UNK = 0, ACT_DATACHANGED, ACT_VRDLOGCHANGED, ACT_OTHERLOGCHANGED, ACT_UPDSTATUSBAR, ACT_MYLOGGING
	} actType;

	uiEvent(actType action=ACT_UNK, Type type=QEvent::User, void *option=0): QEvent(type)
	{
		_action = action;
		_option = option;
	};
	virtual ~uiEvent()
	{
		ignore();
	};

private:
	actType _action;
	void *_option;

public:
	actType action() const { return _action; };
	void *option() const { return _option; };
};

typedef QTreeWidget  QTree;
typedef QTableWidget QTable;
typedef QTreeWidgetItem  QTreeItem;
typedef QTableWidgetItem QTableItem;

typedef enum itemType
{
	ITYPE_UNK = 0, ITYPE_SERVER, ITYPE_HOST, ITYPE_RUN, ITYPE_UID
} ItemType;

//_____________________________________________________________________
template <typename T, typename U>class item: public T
{
public:
	item(const U *utils, itemType type, srviface *iface): T(type)
	{
		_type = type;
		_iface = iface;
		_utils = utils;
		_utils->init(this);
	};
	virtual ~item()
	{
	};
private:
	const U*  _utils;
	itemType  _type;
	srviface* _iface;
public:
	U &utils() const { return &utils; };
	itemType type() const { return _type; };
	srviface *iface() const { return _iface; };
};

//_____________________________________________________________________
class treeUtils: public qtabUtils
{
	typedef item<QTreeItem,treeUtils> TreeItem;
public:
	explicit treeUtils(QTree *widget) throw()
	{
		_qtw = widget;
	    _qtw->setSelectionBehavior(QAbstractItemView::SelectRows);
	    _qtw->setSelectionMode(QAbstractItemView::SingleSelection);
	    _qtw->setCurrentItem(0);
	    _qtw->installEventFilter(this);
	};
	virtual ~treeUtils()
	{
	};
private:
	treeUtils(treeUtils *source) throw() { };
	treeUtils(treeUtils &source) throw() { };
	void operator=(const treeUtils &source) throw() { };
public:
	QTree* operator->() const throw() { return _qtw; };

private:
	QTree *_qtw;

public:
    friend class ui;
	void init(TreeItem *item) const;
    void hresize() const;
    void vresize() const;

    const TreeItem *parent(const TreeItem *item, itemType type) const;
    const string parent_path(const TreeItem *item, itemType type) const;
    TreeItem *setItem(const string &text, int column, itemType type, srviface *iface) const;
    TreeItem *setItem(const string &text, int column, TreeItem *item) const;
    TreeItem *findNode(const QString &server, const QString &text, itemType type, int column=0) const;
	void rmChildren(TreeItem *parent) const;
    srviface *getServerSelected() const;
    static void clrSelection();

  	// qtabUtils overrides
protected:
	virtual bool eventFilter(QObject *, QEvent *);
};
typedef item<QTreeItem,treeUtils> TreeItem;

//_____________________________________________________________________
class tabUtils: public qtabUtils
{
	typedef item<QTableItem,tabUtils> TableItem;
public:
	explicit tabUtils(QTable *widget) throw();
	virtual ~tabUtils() { };
protected:
	tabUtils(tabUtils *source) throw() { };
	tabUtils(tabUtils &source) throw() { };
	void operator=(const tabUtils &source) throw() { };
public:
	QTable* operator->() const throw() { return _qtw; };

protected:
	QTable *_qtw;
    typedef enum tcOrder { SORT_Ascend=Qt::AscendingOrder, SORT_Descend=Qt::DescendingOrder, SORT_No
    } tcOrder;
    struct {
    	int column;
    	int order;
    } _sort;
    int _hrState;
    bool _editable;

protected:
	void rmRows(const vector<int> &row_ids) const;
public:
    friend class myQMainWindow;
    friend class ui;
	void init(TableItem *item) const;
    void hresize();
    void vresize() const;
    srviface *iface(int row, int column) const;

    int addRow(bool visible=true) const;
    string getItem(int row, int column) const;
    template <typename T> TableItem *setItem(const T &var, int row, int column, srviface *iface) const {
    	TableItem *item = new TableItem(this,ITYPE_UNK,iface);
       	item->setData(Qt::DisplayRole, var);
    	if (!_editable)
    		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    	_qtw->setItem(row,column,item);
    	return item;
    };
	vector<int> findRows(const QString &text, int column, int row=0) const;
	vector<int> findRows(const QString &text, int column, const vector<int> &rows) const;
	int findRow(const QString &text, int column, const vector<int> &rows) const;
	int rmRows(const QString &text, int column, int row=0) const;
	void filterRows(bool visible) const;
	void filterRows(const QString &text, int column, bool visible) const;
	void fillNominalClients(srviface *iface, const DPS::Producer::NCS_var &s_ncs);
	void fillDBServActClients(srviface *iface, const DPS::Client::ACS_var &s_acs);
	void rowSelection(int row=-1) const;
	bool rowSelected(int row) const;
	void nodeSelection(TreeItem *node) const;
    static void clrSelection(const tabUtils *except=0);
    void setColor(const QColor &color, int row) const;
    QColor getColor(int row) const;
    void sort(int column);
    bool sort(bool enable);
    bool editable() const;
    void editable(bool enable);

public:
  	virtual void operation(DPS::Client::RecordChange rc, int row) { };
  	virtual void operation(DPS::Client::RecordChange rc) { };
  	// qtabUtils overrides
public slots:
	virtual void clktbHeader(int logid);
	virtual void clkcSelection(int row, int column);
protected:
	virtual bool eventFilter(QObject *obj, QEvent *event);
};
typedef item<QTableItem,tabUtils> TableItem;

template<> inline TableItem *tabUtils::setItem(const string &text, int row, int column, srviface *iface) const
{
	TableItem *item = new TableItem(this,ITYPE_UNK,iface);
	item->setText(text.c_str());
	if (!_editable)
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
	_qtw->setItem(row,column,item);
	return item;
};
template<> inline TableItem *tabUtils::setItem(const uistring &text, int row, int column, srviface *iface) const
{
	TableItem *item = new TableItem(this,ITYPE_UNK,iface);
	item->setText(text.c_str());
	if (!_editable)
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
	_qtw->setItem(row,column,item);
	return item;
};

//_____________________________________________________________________
// Controls tabUtils handling

#define CTRL_TABCLASS(T,U)                                 \
class T: public tabUtils                                   \
{                                                          \
public:                                                    \
	explicit T(QTable *widget): tabUtils(widget) { };      \
	virtual ~T() { };                                      \
	void operation(DPS::Client::RecordChange rc, int row); \
	U;                                                     \
};
CTRL_TABCLASS(sNC,void operation(DPS::Client::RecordChange rc))
CTRL_TABCLASS(sNH,void operation(DPS::Client::RecordChange rc))
CTRL_TABCLASS(sNK,void operation(DPS::Client::RecordChange rc))
CTRL_TABCLASS(sSE,)
CTRL_TABCLASS(pNC,void operation(DPS::Client::RecordChange rc))
class pAC: public tabUtils
{
public:
	explicit pAC(QTable *widget): tabUtils(widget) { };
	virtual ~pAC() { };
	virtual void operation(DPS::Client::RecordChange rc, int row);
	virtual void clkcSelection(int row, int column);
};
CTRL_TABCLASS(pNH,void operation(DPS::Client::RecordChange rc))
CTRL_TABCLASS(pNNT,)
CTRL_TABCLASS(pPNT,)
class pRT: public tabUtils
{
public:
	explicit pRT(QTable *widget): tabUtils(widget) { };
	virtual ~pRT() { };
	virtual void operation(DPS::Client::RecordChange rc, int row);
	virtual void clkcSelection(int row, int column);
};

//_____________________________________________________________________
// Other tables (double)click

class ActiveHosts: public tabUtils
{
public:
	explicit ActiveHosts(QTable *widget): tabUtils(widget) { };
	virtual ~ActiveHosts() { };
public slots:
	virtual void clkSelection(int row, int column);
};
class ActiveClients: public tabUtils
{
public:
	explicit ActiveClients(QTable *widget): tabUtils(widget) { };
	virtual ~ActiveClients() { };
public slots:
	virtual void clkSelection(int row, int column);
	virtual void dblclkSelection(int row, int column);
};
class ProducerRuns: public tabUtils
{
public:
	explicit ProducerRuns(QTable *widget): tabUtils(widget) { };
	virtual ~ProducerRuns() { };
public slots:
	virtual void clkSelection(int row, int column);
	virtual void dblclkSelection(int row, int column);
};
class ProducerNTuples: public tabUtils
{
public:
	explicit ProducerNTuples(QTable *widget): tabUtils(widget) { };
	virtual ~ProducerNTuples() { };
public slots:
	virtual void clkSelection(int row, int column);
};
class ServActClients: public tabUtils
{
public:
	explicit ServActClients(QTable *widget): tabUtils(widget) { };
	virtual ~ServActClients() { };
public slots:
	virtual void clkSelection(int row, int column);
};
class VrdLog: public tabUtils
{
public:
	explicit VrdLog(QTable *widget): tabUtils(widget) { };
	virtual ~VrdLog() { };
public slots:
	virtual void dblclkSelection(int row, int column);
};
class OtherLog: public tabUtils
{
public:
	explicit OtherLog(QTable *widget): tabUtils(widget) { };
	virtual ~OtherLog() { };
public slots:
	virtual void dblclkSelection(int row, int column);
};

//_____________________________________________________________________
// Enumerate fields of the tables

typedef enum tcTree {
	TRCOL_Name=0
} tcTree;

typedef enum tcActHosts {
	TCOLAH_Serv=0, TCOLAH_HostName, TCOLAH_Clients, TCOLAH_RunningPr,
	TCOLAH_AllowedPr, TCOLAH_Clock, TCOLAH_CliProcessed, TCOLAH_CliFailed,
	TCOLAH_NTuples, TCOLAH_EventTags, TCOLAH_Events, TCOLAH_Total,
	TCOLAH_Warnings, TCOLAH_Errors, TCOLAH_CPUEvent, TCOLAH_Efficiency,
	TCOLAH_Status
} tcActHosts;

typedef enum tcActClients {
	TCOL_Serv=0, TCOL_Run, TCOL_Id, TCOL_HostName, TCOL_Mips, TCOL_Threads,
	TCOL_ProcId, TCOL_StartTime, TCOL_LastUpdTime, TCOL_Timeouut,
	TCOL_EvLeft, TCOL_FinishIn, TCOL_Status, TCOL_FileName
} tcActClients;

typedef enum tcProdRuns {
	TCOLPR_Serv=0, TCOLPR_Run, TCOLPR_Uid, TCOLPR_Time,
	TCOLPR_FirstEv, TCOLPR_LastEv, TCOLPR_Prior, TCOLPR_Hist,
	TCOLPR_Fails, TCOLPR_DataMC, TCOLPR_Host, TCOLPR_Status,
	TCOLPR_No
} tcProdRuns;

typedef enum tcProdNTuples {
	TCOLPNT_Serv=0, TCOLPNT_Run, TCOLPNT_Time, TCOLPNT_FirstEv,
	TCOLPNT_LastEv, TCOLPNT_Name, TCOLPNT_CRC, TCOLPNT_Size,
	TCOLPNT_Status
} tcProdNTuples;

typedef enum tcSDBActSlients {
	TCOLSDB_Serv=0, TCOLSDB_Id, TCOLSDB_HostName, TCOLSDB_Mips,
	TCOLSDB_ProcId, TCOLSDB_StartTime, TCOLSDB_LastUpdTime, TCOLSDB_Timeout,
	TCOLSDB_Status
} tcSDBActSlients;

typedef enum tcDiskUsage {
	TCOLDU_Serv=0, TCOLDU_FileSystem, TCOLDU_Total, TCOLDU_Free,
	TCOLDU_PercFree, TCOLDU_OK
} tcDiskUsage;

typedef enum tcVrdLog {
	TCOLVRD_Serv=0, TCOLVRD_File, TCOLVRD_Time, TCOLVRD_Size
} tcVrdLog;

typedef enum tcOtherLog {
	TCOLOTHER_File=0, TCOLOTHER_Time, TCOLOTHER_Size, TCOLOTHER_Comment
} tcOtherLog;

typedef enum tc_sNC {
	TCOLsNC_Serv=0, TCOLsNC_Uid, TCOLsNC_Type, TCOLsNC_MaxClients,
	TCOLsNC_CPU, TCOLsNC_Memory, TCOLsNC_ScriptPath, TCOLsNC_LogPath,
	TCOLsNC_Submit, TCOLsNC_HostName, TCOLsNC_LoginTheEnd
} tc_sNC;

typedef enum tc_sNH {
	TCOLsNH_Serv=0, TCOLsNH_HostName, TCOLsNH_Interface, TCOLsNH_OS,
	TCOLsNH_CPUNumb, TCOLsNH_Memory, TCOLsNH_Clock, TCOLsNH_ClientsAllowed,
	TCOLsNH_Status
} tc_sNH;

typedef enum tc_sSE {
	TCOLsSE_Serv=0, TCOLsSE_Var, TCOLsSE_Path
} tc_sSE;

typedef enum tc_pAC {
	TCOLpAC_Serv=0, TCOLpAC_Uid, TCOLpAC_HostName, TCOLpAC_Mips,
	TCOLpAC_Pid, TCOLpAC_Status, TCOLpAC_Type, TCOLpAC_Timeout, TCOLpAC_FilePath
} tc_pAC;

typedef enum tc_pNH {
	TCOLpNH_Serv=0, TCOLpNH_HostName, TCOLpNH_Interface, TCOLpNH_OS,
	TCOLpNH_CPUNumb, TCOLpNH_Memory, TCOLpNH_Clock, TCOLpNH_ClientsAllowed,
	TCOLpNH_CliProcessed, TCOLpNH_CliFailed, TCOLpNH_Status
} tc_pNH;

typedef enum tc_pNNT {
	TCOLpNNT_Serv=0, TCOLpNNT_Uid, TCOLpNNT_HostName, TCOLpNNT_OutDirPath,
	TCOLpNNT_RunMode, TCOLpNNT_UpdFreq, TCOLpNNT_DstType
} tc_pNNT;

typedef enum tc_pPNT {
	TCOLpPNT_Serv=0, TCOLpPNT_Run, TCOLpPNT_Time, TCOLpPNT_FirstEv,
	TCOLpPNT_LastEv, TCOLpPNT_Path, TCOLpPNT_Version, TCOLpPNT_Size,
	TCOLpPNT_Status, TCOLpPNT_Type
} tc_pPNT;

typedef enum tc_pRT {
	TCOLpRT_Serv=0, TCOLpRT_Uid, TCOLpRT_CUid, TCOLpRT_HostName,
	TCOLpRT_Run, TCOLpRT_FirstEv, TCOLpRT_LastEv, TCOLpRT_Priority,
	TCOLpRT_FilePath, TCOLpRT_Status, TCOLpRT_History, TCOLpRT_StType,
	TCOLpRT_Fails, TCOLpRT_DataMC, TCOLpRT_TEvent, TCOLpRT_TLEvent
} tc_pRT;

class mylogUtils;
class myQMainWindow;

//_____________________________________________________________________

class ui: public Ui_MainWindow
{
public:
	typedef enum uiIconState { ICONST_OK=0, ICONST_WARNING, ICONST_ALARM } uiIconState;
	ui();
	virtual ~ui() {};

protected:
	uiEvent _evIFace;
	time_t _dataRequested;
	imapper<TreeItem *> _map_serv_nodes;
	map<uistring, int> _tabs;
	map<uistring, int> _icon_state;
	map<uistring, tabUtils *> _map_forms;
	map<uistring, pair<tabUtils *, int> > _map_ctrl_forms;
public:
	friend void tabUtils::hresize();
	myQMainWindow *_mw;
	treeUtils *_tuTreeWidget;
	treeUtils *_tuTreeWidget2;
	tabUtils *_tuActiveHosts;
	tabUtils *_tuActiveClients;
	tabUtils *_tuProducerRuns;
	tabUtils *_tuProducerNTuples;
	tabUtils *_tuServActClients;
	tabUtils *_tuDBServActClients;
	tabUtils *_tuDiskUsage;
	tabUtils *_tuVrdLog;
	tabUtils *_tuOtherLog;
	tabUtils *_tusNC;
	tabUtils *_tusNH;
	tabUtils *_tusNK;
	tabUtils *_tusSE;
	tabUtils *_tupNC;
	tabUtils *_tupAC;
	tabUtils *_tupNH;
	tabUtils *_tupNNT;
	tabUtils *_tupPNT;
	tabUtils *_tupRT;
	mylogUtils *_tuMyLog;
	shFilter _shSelFilt;
	shCedit  _shCtrEdit;
	shSfile  _shLogSel;
	int _resFinishIn;

private:
	static const QColor colNormal()  { return QColor(150,255,0); };
	static const QColor colOK()      { return QColor(150,255,120); };
	static const QColor colWarning() { return QColor(255,255,150); };
	static const QColor colAlarm()   { return QColor(255,100,0); };
	static const QColor colUnk()     { return QColor(255,255,255); };

public:
    void treeclick(const TreeItem *node, int column, const tabUtils *source=0) const;
    void tableclick(const tabUtils *table, int row, int column) const;
    friend void tabUtils::nodeSelection(TreeItem *node) const;

public:
    bool acl(const string &user);
    void setupUi(QMainWindow *MainWindow);
	void setDataRequested();
	void kickev(uiEvent::actType type, srviface *iface=0);
	void kickev(uistring *stream);
    void icon(const char *tab_label);
    void icon(const char *tab_label, uiIconState state, bool forced=false);
	typedef enum colors { CLR_UNK=0, CLR_FINISHED, CLR_RUNNING, CLR_WARNING, CLR_ALARM } colors;
	static const QColor color(colors id);
	tabUtils *getVisibleTab() const;
	tabUtils *getVisibleCtrlTab() const;
	const char *getVisibleCtrlTabName(tabUtils *tabu) const;
	typedef enum zoomOper { ZOOM_In=0, ZOOM_Out } zoomOper;
	template <typename T> static void zoom(zoomOper op, T *widget) {
		QFont font = const_cast<QFont &>((*widget)->font());
		int sz = font.pointSize();
		if (op == ZOOM_In) sz++;
		  else sz--;
		font.setPointSize(sz);
		(*widget)->setFont(font);
		widget->vresize();
		widget->hresize();
	};

	// Signals and slots
public:
	virtual bool event(QEvent *ev);
	void visibleRuns(srviface *iface, vector<int> &runs) const;
	void visibleClients(srviface *iface, vector<int> &clients) const;
public slots:
	virtual void slUpdateStatus();
	virtual void mnuResetHosts();
	virtual void mnuResetFailedHosts();
	virtual void mnuResetFailedRuns();
	virtual void mnuResetFailedRunsDST();
	virtual void mnuResetHistory();
	virtual void mnuDeleteValidatedDST();
	virtual void mnuDeleteFailedDST();
	virtual void mnuDeleteActiveClients();
	virtual void mnuDeleteRuns();
	virtual void mnuFinishFailedRuns();
	virtual void mnuUpdateAFSToken();
	virtual void mnuQuit() { exit(0); };
	virtual void mnuActiveHosts();
	virtual void mnuLockUpdate();
	virtual void mnuUseFilter();
	virtual void mnuZoomIn();
	virtual void mnuZoomOut();
	virtual void mnuClearMyLogging();
	virtual void mnuExportDB();
	virtual void mnuExportDBMerged();
	virtual void mnuImportDB();
	virtual void mnuSetFilter();
	virtual void clkTreeWidget(QTreeWidgetItem *item, int column);
	virtual void clkTreeWidget2(QTreeWidgetItem *item, int column);
	virtual void tabChanged(int index);
	virtual void clkbtReplace(bool checked);
	virtual void clkbtAdd(bool checked);
	virtual void clkbtRemove(bool checked);
	virtual void clkbtNew(bool checked);
	virtual void clkbtReplaceAll(bool checked);
	virtual void clkbtMoveUp(bool checked);
	virtual void clkbtMoveDown(bool checked);
};
extern ui theUI;
extern mylogUtils log();

//_____________________________________________________________________
// Thread-safe wrapper to stringstream output

class mylogUtils: public std::stringstream, qtabUtils
{
public:
	mylogUtils(QPlainTextEdit *widget) throw() { _qtw = widget; _qtw->installEventFilter(this); };
	mylogUtils(const mylogUtils &lu) throw()   { *this = const_cast<mylogUtils &>(lu); };
	virtual ~mylogUtils() { };
protected:
	QPlainTextEdit *_qtw;
private:
	static string prompt() {
		char tfs[32];
		struct timeval tv;
		gettimeofday(&tv,0);
		struct tm *t = localtime(&tv.tv_sec);
		sprintf(tfs,"<%02d.%02d %02d:%02d:%02d.%03d> ",
				t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec,tv.tv_usec/1000);
		return tfs;
	};
public:
    void hresize() const { };
    void vresize() const { };
    QPlainTextEdit* operator ->() const throw() { return _qtw; };
    template <typename T> mylogUtils & operator <<(T s) {
    	reinterpret_cast<stringstream &>(*this) << s;
    	return *this;
    };
    mylogUtils & operator =(mylogUtils &lu) {
    	mylogUtils::_qtw = lu._qtw;
		reinterpret_cast<stringstream &>(*this) << lu.str();
		return *this;
    };
    __ostream_type & operator <<(__ostream_type & (*__pf)(__ostream_type &)) {
		theUI.kickev(new uistring(prompt() + (*this).str()+"\n"));
		(*this).str("");
    	return *this;
    };
	// qtabUtils overrides
protected:
	virtual bool eventFilter(QObject *, QEvent *);
};

//_____________________________________________________________________
class warning: public Ui_Dialog
{
public:
	warning(const char *message): Ui_Dialog()
	{
		_qtw = new QDialog();
		_message = message;
	};
	virtual ~warning()
	{
		delete _qtw;
	};
public:
	QDialog* operator->() const throw() { return _qtw; };

private:
	QDialog *_qtw;
    string _message;
public:
    void setupUi();
    void setTitle(const char *title);
public slots:
	virtual void ok(bool checked);
};

//_____________________________________________________________________
class viewer: public Ui_Form
{
public:
	viewer(): Ui_Form() { _ctrl_pressed = false; _qtw = new QWidget(); };
	virtual ~viewer() { };
public:
    void hresize() const { };
    void vresize() const { };
	QWidget* operator->() const throw() { return _qtw; };

private:
	QWidget *_qtw;
	string _file;
	list<uistring> _contents;
public:
    void setupUi();
    void getFile(const char *file=0);
    void setFile(const char *file=0);
    void setTitle(const char *title);
public slots:
	virtual void clkbtUp(bool checked);
	virtual void clkbtDown(bool checked);
	virtual void clkbtUpd(bool checked);
protected:
	bool _ctrl_pressed;
    virtual bool eventFilter(QObject *obj, QEvent *event);
	virtual bool event(QEvent *ev);
};

//_____________________________________________________________________
class filter: public Ui_FilterDialog
{
public:
	filter(const char *key);
	virtual ~filter() { };
public:
    QDialog* operator->() const throw() { return _qtw; };

private:
    QDialog *_qtw;
    string _key;
public:
    void setupUi();
    void setTitle(const char *title);
public slots:
	virtual void clkbtApply(bool);
	virtual void clkbtReset(bool);
	virtual void clkbtCancel(bool);
};

//_____________________________________________________________________
class editColumn: public Ui_editColumnDialog
{
public:
	editColumn(const char *key);
	virtual ~editColumn() { };
public:
    QDialog* operator->() const throw() { return _qtw; };

private:
    QDialog *_qtw;
    string _key;
public:
    void setupUi();
    void setTitle(const char *title);
public slots:
	virtual void clkbtOK(bool);
	virtual void clkbtCancel(bool);
};

//_____________________________________________________________________
class selFile: public Ui_selFileDialog
{
public:
	selFile(const char *key);
	virtual ~selFile() { };
public:
    QDialog* operator->() const throw() { return _qtw; };

private:
    QDialog *_qtw;
    string _key;
public:
    void setupUi();
    void setTitle(const char *title);
public slots:
	virtual void clkbtOK(bool);
};

//_____________________________________________________________________
class myQMainWindow: public QMainWindow
{
public:
	myQMainWindow(): QMainWindow() { _ctrl_pressed=false; _alt_pressed=false; _caller = 0; };
	virtual ~myQMainWindow() { };
private:
	void *_caller;
protected:
	// Currently I'm not using QKeySequence here
	virtual bool event(QEvent *event);
	bool _ctrl_pressed;
	bool _alt_pressed;
	friend class ProducerRuns;
	friend bool qtabUtils::eventFilter(QObject *, QEvent *);
};

#endif	// eof _UIWRAPPER_H
