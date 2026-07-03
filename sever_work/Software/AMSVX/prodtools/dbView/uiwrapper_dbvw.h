#ifndef  _UIWRAPPER_DBVW_H
#define  _UIWRAPPER_DBVW_H

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <ios>
#include <string>
#include <sstream>
#include <ostream>
#include <set>
#include <vector>
#include <map>
#include <list>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <algorithm>

using std::cout;
using std::endl;
using std::fstream;
using std::string;
using std::stringstream;
using std::set;
using std::vector;
using std::map;

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
#include <QtGui/QKeyEvent>
#include <QtNetwork/QHostInfo>
#include <QtGui/QFileDialog>
#include <QtCore/QProcess>

#include "mainw_dbvw.h"
#include "../dbServer/bdbRC.hxx"
#include "../dbServer/bdbTraits.hxx"
#include "ManCommon.hxx"
#include "ManThread.hxx"

extern bdbRC *theDB;
extern void readDB(const char *);
extern void createDB(const char *);
extern void recoverDB(const char *);
extern timespec hrtime(clockid_t);

//_____________________________________________________________________

class uistring: public string
{
private:
	vector<uistring> _tokens;
public:
	uistring(): string() { };
	uistring(const std::string &s): string(s) { };
	uistring(const char *s): string(s) { };
	uistring(const QString &qs): string(qs.toUtf8().constData()) { };
	uistring(const QByteArray &ba): string(QString(ba).toUtf8().constData()) { };
public:
	QString q_str() const { return QString(c_str()); };
public:
	int tokens(const char *pattern) {
		_tokens.clear();
		if (length()) {
			char *s = new char[length()+1];
			copy(s,length());
			s[length()] = '\0';
			char *st = strtok(s,pattern);
			while (st) {
				_tokens.push_back(uistring(st));
			    st = strtok(NULL,pattern);
			}
			delete s;
		}
		return tokens();
	};
	int tokens() const {
		return _tokens.size();
	};
	uistring token(int id) const {
		if (id<tokens()) return _tokens[id];
		  else return uistring("");
	};
};

//_____________________________________________________________________
class uiEvent: public QEvent
{
public:
	typedef enum actType { ACT_UNK = 0, ACT_TABCHANGED } actType;

	uiEvent(actType action=ACT_UNK, Type type=QEvent::User, void *option=0): QEvent(type) {
		_action = action;
		_option = option;
	};
	virtual ~uiEvent() {
		ignore();
	};

private:
	actType _action;
	void *_option;

public:
	actType action() const { return _action; };
	void *option() const { return _option; };
};

//_____________________________________________________________________

template <typename T, typename U>class item: public T
{
public:
	item(const U *utils): T()
	{
		_utils = utils;
		_utils->init(this);
	};
	virtual ~item()
	{
	};
private:
	const U*  _utils;
public:
	U &utils() const { return &utils; };
};
typedef QTreeWidget  QTree;
typedef QTableWidget QTable;
typedef QTreeWidgetItem  QTreeItem;
typedef QTableWidgetItem QTableItem;

//_____________________________________________________________________

class treeUtils: public qtabUtils
{
	typedef item<QTreeItem,treeUtils> TreeItem;
public:
	explicit treeUtils(QTree *widget) throw();
	virtual ~treeUtils() { };
private:
	treeUtils(treeUtils *source) throw() { };
	treeUtils(treeUtils &source) throw() { };
	void operator=(const treeUtils &source) throw() { };
public:
	QTree* operator->() const throw() { return _qtw; };

private:
	QTree *_qtw;

public:
	void init(TreeItem *item) const;
    void hresize() const;
    void vresize() const;

    TreeItem *setItem(const string &text, int column, TreeItem *item) const;

  	// qtabUtils overrides
public slots:
	virtual void clkSelectionChanged();
protected:
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
	map<uistring,int> _columns;
    typedef enum tcOrder { SORT_Ascend=Qt::AscendingOrder, SORT_Descend=Qt::DescendingOrder, SORT_No
    } tcOrder;
    struct {
    	int column;
    	int order;
    } _sort;
    int _hrState;

public:
    friend class ui;
	void init(TableItem *item) const;
    void hresize();
    void vresize() const;

    int addRow() const;
    template <typename T> T getItem(int row, int column) const {
    	return static_cast<T>(_qtw->item(row,column)->data(Qt::DisplayRole).value<T>());
    };
    template <typename T> T getItem(int row, const char *column) {
    	return getItem<T>(row,_columns[column]);
    };
    int hasSelection() const;
	void rmRow(int row=-1) const;
   template <typename T> void setItem(const T &var, int row, int column) const {
    	TableItem *item = new TableItem(this);
       	item->setData(Qt::DisplayRole, var);
    	_qtw->setItem(row,column,item);
    };
    template <typename T> void setItem(const T &var, int row, const char *column) {
    	return setItem(var,row,_columns[column]);
    };

public:
//void name.subname(int parm) {};
    virtual void dbin(bdbRC *db=0) { };
  	virtual void dbout(bdbRC *db=0) { };
    virtual void dbin_imex(ofstream &fs, bdbRC *db=0) { };
    virtual void dbout_imex(ifstream &fs, bdbRC *db=0) { };
  	// qtabUtils overrides
public slots:
	virtual void clktbHeader(int logid);
	virtual void clkSelection(int row, int column);
	virtual void clkSelectionChanged();
protected:
	virtual bool eventFilter(QObject *obj, QEvent *event);
};
typedef item<QTableItem,tabUtils> TableItem;

template<> inline string tabUtils::getItem(int row, int column) const {
	return uistring(_qtw->item(row,column)->data(Qt::DisplayRole).value<QString>());
};
template<> inline void tabUtils::setItem(const string &text, int row, int column) const {
	TableItem *item = new TableItem(this);
	item->setText(text.c_str());
	_qtw->setItem(row,column,item);
};
template<> inline void tabUtils::setItem(const uistring &text, int row, int column) const {
	TableItem *item = new TableItem(this);
	item->setText(text.c_str());
	_qtw->setItem(row,column,item);
};

//_____________________________________________________________________
// Controls tabUtils handling

#define CTRL_TABCLASS(T,U,V,U2,V2)                    \
class T: public tabUtils                              \
{                                                     \
public:                                               \
	explicit T(QTable *widget): tabUtils(widget) { }; \
	virtual ~T() { };                                 \
	U;												  \
	V;												  \
	U2;												  \
	V2;												  \
};
CTRL_TABCLASS(sNC,void dbin(bdbRC *db=0),void dbout(bdbRC *db=0),void dbin_imex(ofstream &fs, bdbRC *db=0),void dbout_imex(ifstream &fs, bdbRC *db=0))
CTRL_TABCLASS(sNH,void dbin(bdbRC *db=0),void dbout(bdbRC *db=0),void dbin_imex(ofstream &fs, bdbRC *db=0),void dbout_imex(ifstream &fs, bdbRC *db=0))
CTRL_TABCLASS(sNK,void dbin(bdbRC *db=0),void dbout(bdbRC *db=0),void dbin_imex(ofstream &fs, bdbRC *db=0),void dbout_imex(ifstream &fs, bdbRC *db=0))
CTRL_TABCLASS(sSE,void dbin(bdbRC *db=0),void dbout(bdbRC *db=0),void dbin_imex(ofstream &fs, bdbRC *db=0),void dbout_imex(ifstream &fs, bdbRC *db=0))
CTRL_TABCLASS(pNC,void dbin(bdbRC *db=0),void dbout(bdbRC *db=0),void dbin_imex(ofstream &fs, bdbRC *db=0),void dbout_imex(ifstream &fs, bdbRC *db=0))
CTRL_TABCLASS(pAC,void dbin(bdbRC *db=0),void dbout(bdbRC *db=0),void dbin_imex(ofstream &fs, bdbRC *db=0),void dbout_imex(ifstream &fs, bdbRC *db=0))
CTRL_TABCLASS(pNH,void dbin(bdbRC *db=0),void dbout(bdbRC *db=0),void dbin_imex(ofstream &fs, bdbRC *db=0),void dbout_imex(ifstream &fs, bdbRC *db=0))
CTRL_TABCLASS(pNNT,void dbin(bdbRC *db=0),void dbout(bdbRC *db=0),void dbin_imex(ofstream &fs, bdbRC *db=0),void dbout_imex(ifstream &fs, bdbRC *db=0))
CTRL_TABCLASS(pPNT,void dbin(bdbRC *db=0),void dbout(bdbRC *db=0),void dbin_imex(ofstream &fs, bdbRC *db=0),void dbout_imex(ifstream &fs, bdbRC *db=0))
CTRL_TABCLASS(pRT,void dbin(bdbRC *db=0),void dbout(bdbRC *db=0),void dbin_imex(ofstream &fs, bdbRC *db=0),void dbout_imex(ifstream &fs, bdbRC *db=0))

//_____________________________________________________________________
// Enumerate fields of the tables

typedef enum tcTree {
	TRCOL_Name=0
} tcTree;

//_____________________________________________________________________

namespace idl {
	struct cmpkey {
	   bool operator()(char const *a, char const *b) const { return std::strcmp(a, b)<0; }
	};
	template <typename T>struct cmpval {
	   bool operator()(T a, T b) const { return a<b; }
	};
	template <typename T> class senum {
	public:
		int _sz;
		map<const char *,T,idl::cmpkey> _mapT;
		map<T,const char *,idl::cmpval<T> > _mapK;
		T _default;
	public:
		senum(const char *keys[], int sz, T def) {
			_sz = sz,
			_default = def;
			for (int i=0; i<_sz; i++) {
				_mapT.insert(std::pair<const char *,T>(keys[i],static_cast<T>(i)));
				_mapK.insert(std::pair<T,const char *>(static_cast<T>(i),keys[i]));
			}
		};
		T value(const char *k) {
			typename map<const char *,T>::iterator it;
			if ((it=_mapT.find(k)) != _mapT.end())
				return it->second;
			return _default;
		};
		const char *key(T val) {
			typename map<T,const char *>::iterator it;
			if ((it=_mapK.find(val)) != _mapK.end())
				return it->second;
			return "";
		};
	};
};

class myQMainWindow;
struct MCFids {	// MapCtrlForms identifiers
	int stwid;	// id in the stacked widget
	int dbtyp;	// corresponding dbType
	MCFids() { };
	MCFids(int st, int db) { stwid=st; dbtyp=db; };
};
typedef map<uistring, pair<tabUtils *, MCFids> > MapCtrlForms;

class ui: public Ui_MainWindow
{
public:
	ui();
	virtual ~ui() {};

public:
	myQMainWindow *_mw;
	treeUtils *_tuTreeWidget2;
	MapCtrlForms _map_ctrl_forms;

public:
    void setupUi(QMainWindow *MainWindow);
	void kickev(uiEvent::actType type, void *option=0);
	tabUtils *getVisibleCtrlTab() const;

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
	static time_t stringTime(const string t);

	// Signals and slots
public slots:
	virtual void mnuOpen();
	virtual void mnuNew();
	virtual void mnuNew2();
    virtual void mnuRecover();
    virtual void mnuUpdate();
    virtual void mnuCommit();
    virtual void mnuQuit();
	virtual void mnuZoomIn();
	virtual void mnuZoomOut();
	virtual void mnuExportDB();
	virtual void mnuImportDB();
	virtual void clkTreeWidget2(QTreeWidgetItem *item, int column);
	virtual void clkbtRemove(bool checked);
	virtual void clkbtNew(bool checked);
	virtual void clkbtMoveUp(bool checked);
	virtual void clkbtMoveDown(bool checked);
};
extern ui theUI;

//_____________________________________________________________________
class myQMainWindow: public QMainWindow
{
public:
	myQMainWindow(): QMainWindow() { _ctrl_pressed=false; _alt_pressed=false; _caller = 0;
	setAttribute(Qt::WA_TransparentForMouseEvents, false);};
	virtual ~myQMainWindow() { };
private:
	void *_caller;
protected:
	// Currently I'm not using QKeySequence here
	virtual bool event(QEvent *event);
	bool _ctrl_pressed;
	bool _alt_pressed;
	friend bool qtabUtils::eventFilter(QObject *, QEvent *);
};
#endif	// eof _UIWRAPPER_DBVW_H
