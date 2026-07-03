#include "uiwrapper.h"
#include "../dbServer/dbserver.hxx"
#include "../dbServer/bdbRC.hxx"
#include "../dbServer/bdbTraits.hxx"

mylogUtils log() {
	return mylogUtils(*theUI._tuMyLog);
};
int _dbgLevel = 0;

void thrViewer(viewer *v)
{
	_thread<>::yield();
	v->getFile();
	_thread<>::yield();
	uiEvent *pEv = new uiEvent(uiEvent::ACT_UNK,QEvent::User,0);
	QApplication::postEvent(reinterpret_cast<QObject *>(v),pEv);
}

//_____________________________________________________________________
// class treeUtils public

void treeUtils::init(TreeItem *item) const
{
}

void treeUtils::hresize() const
{
	_qtw->resizeColumnToContents(0);
}

void treeUtils::vresize() const
{
}

const TreeItem *treeUtils::parent(const TreeItem *item, itemType type) const
{
	while (item->type() != type)
		item = parent(reinterpret_cast<TreeItem *>(item->parent()),type);
	return item;
}

const string treeUtils::parent_path(const TreeItem *item, itemType type) const
{
	string result;

	while (item->type() != type)
	{
		result = string("/") + uistring(item->text(0)) + result;
		item = parent(reinterpret_cast<TreeItem *>(item->parent()),type);
	}
	result = uistring(item->text(0)) + result;

	return result;
}

TreeItem *treeUtils::setItem(const string &text, int column, itemType type, srviface *iface) const
{
	TreeItem *item = new TreeItem(this,type,iface);
	item->setText(column,text.c_str());
	return item;
}

TreeItem *treeUtils::setItem(const string &text, int column, TreeItem *item) const
{
	item->setText(column,text.c_str());
	return item;
}

TreeItem *treeUtils::findNode(const QString &server, const QString &text, itemType type, int column) const
{
	treeUtils *tree = theUI._tuTreeWidget;

	QList<QTreeItem *> nodes = (*tree)->findItems(text,Qt::MatchExactly|Qt::MatchRecursive);
	for (int i=0; i<nodes.count(); i++)
	{
		TreeItem *node = reinterpret_cast<TreeItem *>(nodes.at(i));
		if (type == ITYPE_UNK || type == node->type())
		{
			const TreeItem *root = parent(node,ITYPE_SERVER);
			if (root->text(0) == server)
			{
				(*tree)->setCurrentItem(node);
				node->setSelected(true);
				return node;
			}
		}
	}
	return 0;
}

void treeUtils::rmChildren(TreeItem *parent) const
{
	QList<QTreeWidgetItem *> children = parent->takeChildren();
	for (int i=0; i<children.count(); i++)
	{
		TreeItem *child = reinterpret_cast<TreeItem *>(children.at(i));
		delete child;
	}
}

srviface *treeUtils::getServerSelected() const
{
	TreeItem *node = reinterpret_cast<TreeItem *>(_qtw->currentItem());
	if (node) return node->iface();
	  else	  return 0;
}

void treeUtils::clrSelection()
{
	QList<QTreeWidgetItem *> selst(theUI._tuTreeWidget->_qtw->selectedItems());
	for (int i=0; i<selst.count(); i++)
		selst.at(i)->setSelected(false);
	theUI.actUpdateAFSToken->setEnabled(false);
}

bool treeUtils::eventFilter(QObject *obj, QEvent *event)
{
	if (qtabUtils::eventFilter(obj,event))
		return _qtw->eventFilter(obj,event);
	return true;
}

//_____________________________________________________________________
// class tabUtils

tabUtils::tabUtils(QTable *widget) throw()
{
	_qtw = widget;
    _qtw->setSelectionBehavior(QAbstractItemView::SelectRows);
    _qtw->setSelectionMode(QAbstractItemView::SingleSelection);
    _qtw->verticalHeader()->setDefaultAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    _sort.column = -1;
    _hrState = 0;
    _editable = false;
    hresize();
    vresize();
    _qtw->installEventFilter(this);
};

void tabUtils::rmRows(const vector<int> &row_ids) const
{
	for (int i=row_ids.size()-1; i>=0; i--)
		_qtw->removeRow(row_ids[i]);
}

void tabUtils::init(TableItem *item) const
{
	item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
////////
//string qqq("QTableView::item:selected { background-color:transparent; color:blue;  border:1px; border-style:solid;}");
//string qqq("QTableView::item:focus { background-color:transparent; color:blue;}");
//_qtw->setStyleSheet(qqq.c_str());
////////
}

void tabUtils::hresize()
{
	map<uistring, pair<tabUtils *, int> >::const_iterator cit;
	tabUtils *tabu = 0;
	for (cit=theUI._map_ctrl_forms.begin(); cit != theUI._map_ctrl_forms.end(); ++cit)
		if (static_cast<void *>(cit->second.first) == static_cast<void *>(this)) {
			tabu = cit->second.first;
			break;
		}
	if (_hrState < 2)
		_qtw->resizeColumnsToContents();
	if (tabu == 0) {
		if (_qtw->rowCount() > 0)
			_hrState = 2;
	}
	else if (_hrState < 2) {
		if (_qtw->isVisible())
			_hrState = 1;
		if (_hrState && _qtw->rowCount() > 0)
			_hrState = 2;
	}
}

void tabUtils::vresize() const
{
	QFont font = const_cast<QFont &>(_qtw->font());
	font.setPointSize(font.pointSize()-2);
	QHeaderView *vhw = _qtw->verticalHeader();
	vhw->setFont(font);
	QFontMetrics fm(_qtw->font());
	_qtw->verticalHeader()->setDefaultSectionSize(fm.height()+2);
}

srviface *tabUtils::iface(int row, int column) const
{
	TableItem *item = reinterpret_cast<TableItem *>(_qtw->item(row,column));
	if (item) return item->iface();
	else	  return 0;
}

int tabUtils::addRow(bool visible) const
{
	int lastrow = _qtw->rowCount();
	_qtw->insertRow(lastrow);
	if (!visible) _qtw->setRowHidden(lastrow,true);
	return lastrow;
}

string tabUtils::getItem(int row, int column) const
{
	TableItem *item = reinterpret_cast<TableItem *>(_qtw->item(row,column));
	if (item) return uistring(item->text());
	  else	  return uistring("");
}

vector<int> tabUtils::findRows(const QString &text, int column, int row) const
{
	vector<int> result;

	for (int i=row; i<_qtw->rowCount(); i++)
	{
		TableItem *qit = reinterpret_cast<TableItem *>(_qtw->item(i,column));
		if (qit && qit->text() == text)
			result.insert(result.end(),i);
	}
	return result;
}

vector<int> tabUtils::findRows(const QString &text, int column, const vector<int> &rows) const
{
	vector<int> result;

	for (unsigned i=0; i<rows.size(); i++)
	{
		TableItem *qit = reinterpret_cast<TableItem *>(_qtw->item(rows[i],column));
		if (qit && qit->text() == text)
			result.insert(result.end(),rows[i]);
	}
	return result;
}

int tabUtils::findRow(const QString &text, int column, const vector<int> &rows) const
{
	for (unsigned i=0; i<rows.size(); i++)
	{
		TableItem *qit = reinterpret_cast<TableItem *>(_qtw->item(rows[i],column));
		if (qit && qit->text() == text)
			return rows[i];
	}
	return -1;
}

int tabUtils::rmRows(const QString &text, int column, int row) const
{
	vector<int> rows2del = findRows(text,column);
	rmRows(rows2del);
	return static_cast<int>(rows2del.size());
}

void tabUtils::filterRows(bool visible) const
{
	if (visible)
		for (int i=0; i<_qtw->rowCount(); i++)
			_qtw->setRowHidden(i,false);
	else
		for (int i=0; i<_qtw->rowCount(); i++)
			_qtw->setRowHidden(i,true);
}

void tabUtils::filterRows(const QString &text, int column, bool visible) const
{
	vector<int> rows = findRows(text,column,0);

	if (visible)
		for (unsigned i=0; i<rows.size(); i++)
			_qtw->setRowHidden(rows[i],false);
	else
		for (unsigned i=0; i<rows.size(); i++)
			_qtw->setRowHidden(rows[i],true);
}

void tabUtils::fillNominalClients(srviface *iface, const DPS::Producer::NCS_var &ncs)
{
	for (int i=0,len=ncs->length(); i<len; i++)
	{
		const DPS::Producer::NominalClient &Nc = ncs[i];

		if (!theUI._shSelFilt.inserver(iface->vno()))
			continue;
		int pos_sNC = addRow(false);
		setItem(iface->vno(),pos_sNC,TCOLsNC_Serv,iface);
		stringstream s;
		s << boost::format("%d") %Nc.uid;
		setItem(s.str(),pos_sNC,TCOLsNC_Uid,iface);
		setItem(iface->strClientType(Nc.Type),pos_sNC,TCOLsNC_Type,iface);
		s.str("");
		s << boost::format("%d") %Nc.MaxClients;
		setItem(s.str(),pos_sNC,TCOLsNC_MaxClients,iface);
		s.str("");
		s << boost::format("%.2f") %Nc.CPUNeeded;
		setItem(s.str(),pos_sNC,TCOLsNC_CPU,iface);
		s.str("");
		s << boost::format("%d") %Nc.MemoryNeeded;
		setItem(s.str(),pos_sNC,TCOLsNC_Memory,iface);
		setItem(string(Nc.WholeScriptPath),pos_sNC,TCOLsNC_ScriptPath,iface);
		setItem(string(Nc.LogPath),pos_sNC,TCOLsNC_LogPath,iface);
		setItem(string(Nc.SubmitCommand),pos_sNC,TCOLsNC_Submit,iface);
		setItem(string(Nc.HostName),pos_sNC,TCOLsNC_HostName,iface);
		s.str("");
		s << boost::format("%d") %Nc.LogInTheEnd;
		setItem(s.str(),pos_sNC,TCOLsNC_LoginTheEnd,iface);
	}
}

void tabUtils::fillDBServActClients(srviface *iface, const DPS::Client::ACS_var &s_acs)
{
	for (int i=0,len=s_acs->length(); i<len; i++)
	{
		const DPS::Client::ActiveClient &ActiveClient = s_acs[i];
		int row = addRow();

		setItem(iface->vno(),row,TCOLSDB_Serv,iface);
		setItem(string(ActiveClient.id.HostName),row,TCOLSDB_HostName,iface);
		setItem(ActiveClient.id.uid,row,TCOLSDB_Id,iface);
		setItem(ActiveClient.id.Mips,row,TCOLSDB_Mips,iface);
		setItem(ActiveClient.id.pid,row,TCOLSDB_ProcId,iface);
		string st = timeString(static_cast<time_t>(ActiveClient.Start));
		setItem(st,row,TCOLSDB_StartTime,iface);
		stringstream s;
		s << boost::format("%d sec ago") %(time(0)-ActiveClient.LastUpdate);
		setItem(s.str(),row,TCOLSDB_LastUpdTime,iface);
		setItem(ActiveClient.TimeOut,row,TCOLSDB_Timeout,iface);
		setItem(iface->strClientStatus(ActiveClient.Status),row,TCOLSDB_Status,iface);
		// color check
		switch (ActiveClient.Status)
		{
		case DPS::Client::Active:
			setColor(ui::color(ui::CLR_RUNNING),row);
			theUI.icon("Service",ui::ICONST_OK);
			break;
		case DPS::Client::Killed:
		case DPS::Client::Lost:
		case DPS::Client::TimeOut:
			setColor(ui::color(ui::CLR_ALARM),row);
			theUI.icon("Service",ui::ICONST_ALARM);
			break;
		default:
			setColor(ui::color(ui::CLR_UNK),row);
			break;
		}
		// opt color check
		if (iface->fails()) {
			setColor(ui::color(ui::CLR_ALARM),row);
			theUI.icon("Service",ui::ICONST_ALARM);
		}
	}
}

void tabUtils::rowSelection(int row) const
{
	if (row < 0)
	{
		_qtw->setCurrentCell(-1,-1);
		_qtw->clearSelection();
	}
	else
		_qtw->setCurrentCell(row,1);
}

bool tabUtils::rowSelected(int row) const
{
	TableItem *item = reinterpret_cast<TableItem *>(_qtw->item(row,0));
	return item->isSelected();
}

void tabUtils::nodeSelection(TreeItem *node) const
{
	if (node)
	{
		(*theUI._tuTreeWidget)->setCurrentItem(node);
		node->setSelected(true);
		theUI.treeclick(node,0,this);
	}
	else
	{
		treeUtils::clrSelection();
		tabUtils::clrSelection(this);
	}
}

void tabUtils::clrSelection(const tabUtils *except)
{
	if (theUI._tuActiveHosts != except)   theUI._tuActiveHosts->rowSelection();
	if (theUI._tuActiveClients != except) theUI._tuActiveClients->rowSelection();
	if (theUI._tuProducerRuns != except)  theUI._tuProducerRuns->rowSelection();
	if (theUI._tuProducerNTuples != except)  theUI._tuProducerNTuples->rowSelection();
	if (theUI._tuServActClients != except)   theUI._tuServActClients->rowSelection();
	if (theUI._tuDBServActClients != except) theUI._tuDBServActClients->rowSelection();
	if (theUI._tuDiskUsage != except) theUI._tuDiskUsage->rowSelection();
	if (theUI._tuVrdLog != except) theUI._tuVrdLog->rowSelection();
	if (theUI._tuOtherLog != except) theUI._tuOtherLog->rowSelection();
}

void tabUtils::setColor(const QColor &color, int row) const
{
	for (int i=0; i<_qtw->columnCount(); i++)
	{
		QTableItem *item = _qtw->item(row,i);
		if (item)
			item->setBackground(QBrush(color));
	}
}

QColor tabUtils::getColor(int row) const
{
	QTableItem *item = _qtw->item(row,0);
	return item->background().color();
}

void tabUtils::sort(int column)
{
	if (column != _sort.column) {
		_sort.column = column;
	    _sort.order = SORT_Ascend;
	}
	if (_sort.order != SORT_No) {
		_qtw->setSortingEnabled(true);
		_qtw->sortByColumn(column,static_cast<Qt::SortOrder>(_sort.order));
		_sort.order = (_sort.order+1) % (static_cast<int>(SORT_No)+1);
		tabUtils *tabu = theUI.getVisibleCtrlTab();
		if (tabu) {
			theUI.pbtMoveUp->setEnabled(false);
			theUI.pbtMoveDown->setEnabled(false);
		}
	}
	else {
		_qtw->setSortingEnabled(false);
		_sort.column = -1;
	}
}

bool tabUtils::sort(bool enable)
{
	bool prev_state;

	if (_sort.column == -1)
		prev_state = false;
	else
		prev_state = true;
	if (enable && _sort.column != -1)
		_qtw->setSortingEnabled(true);
	else
		_qtw->setSortingEnabled(false);
	return prev_state;
}

bool tabUtils::editable() const { return _editable; }

void tabUtils::editable(bool enable) { _editable = enable; }

void tabUtils::clktbHeader(int logid)
{
	sort(logid);
	for (unsigned i=0; i<__thrClients.size(); i++) {
		srviface *iface = __thrClients[i];
		if (_sort.column == -1)
			theUI.kickev(uiEvent::ACT_DATACHANGED,iface);
		else if (iface->fails())
			theUI.kickev(uiEvent::ACT_DATACHANGED,iface);
	}
}

void tabUtils::clkcSelection(int row, int column)
{
	if (_editable) {
		tabUtils *tabu = theUI.getVisibleCtrlTab();
		theUI.pbtReplace->setEnabled(true);
		theUI.pbtAdd->setEnabled(true);
		theUI.pbtRemove->setEnabled(true);
		if (_sort.column == -1) {
			theUI.pbtMoveUp->setEnabled(true);
			theUI.pbtMoveDown->setEnabled(true);
		}
		else {
			theUI.pbtMoveUp->setEnabled(false);
			theUI.pbtMoveDown->setEnabled(false);
		}
	}
}

bool tabUtils::eventFilter(QObject *obj, QEvent *event)
{
	if (qtabUtils::eventFilter(obj,event))
		return _qtw->eventFilter(obj,event);
	return true;
}

//_____________________________________________________________________
// Controls tabUtils handling

void sNC::operation(DPS::Client::RecordChange rc, int row)
{
	if (row < 0)
		return;
	srviface *iface = this->iface(row,TCOLsSE_Serv);

    DPS::Client::NominalClient nc;
    try {
    	nc.uid = boost::lexical_cast<short>(getItem(row,TCOLsNC_Uid));
    	nc.Type = iface->clientType(getItem(row,TCOLsNC_Type).c_str());
        nc.MaxClients = boost::lexical_cast<short>(getItem(row,TCOLsNC_MaxClients));
        nc.CPUNeeded = boost::lexical_cast<float>(getItem(row,TCOLsNC_CPU));
        nc.MemoryNeeded = boost::lexical_cast<short>(getItem(row,TCOLsNC_Memory));
        nc.WholeScriptPath = getItem(row,TCOLsNC_ScriptPath).c_str();
        nc.LogPath = getItem(row,TCOLsNC_LogPath).c_str();
        nc.SubmitCommand = getItem(row,TCOLsNC_Submit).c_str();
        nc.HostName = getItem(row,TCOLsNC_HostName).c_str();
        nc.LogInTheEnd = boost::lexical_cast<short>(getItem(row,TCOLsNC_LoginTheEnd));
    }
    catch(boost::bad_lexical_cast &ex) {
    	log() << "sNC::operation-E- bad record cast" << endl;
    	return;
    }

	DPS::Server_var pServer;
 	DPS::DBServer_var pDBServer;
 	try {
    	CORBA::String_var ref = iface->iors().c_str();
    	pServer = DPS::Server::_narrow(__theOrb->string_to_object(ref));
		ref = iface->iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex) {
    	log() << "sNC::operation-E- corba narrow error on " << iface->server() << endl;
        return;
    }
    try {
		DPS::Client::CID cid = iface->cid(iface->s_acs(),0);
		cid.Type = nc.Type;
    	pServer->sendNC(cid,nc,rc);
		cid = iface->cid(iface->dbs_acs(),0);
		cid.Type = nc.Type;
		DPS::Client::CID_var cidv = new DPS::Client::CID(cid);
		DPS::Client::NominalClient_var ncv = new DPS::Client::NominalClient(nc);
		pDBServer->sendNC(cidv,ncv,rc);
		log() << "sNC::operation-I-sendNC uid " << getItem(row,TCOLsNC_Uid) << ", host " << getItem(row,TCOLsNC_HostName) << endl;
    }
    catch(DPS::DBProblem &dbex){
    	log() << "sNC::operation-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
    }
    catch (CORBA::SystemException &ex) {
    	log() << "sNC::operation-E- corba send error on " << iface->server() << endl;
    }
}

void sNC::operation(DPS::Client::RecordChange rc) 
{ 
	if (this != theUI.getVisibleCtrlTab()) return;

	vector<int> vis_rows;
	for (int len=(*this)->rowCount(),i=0; i<len; i++)
		if (!(*this)->isRowHidden(i)) vis_rows.push_back(i);
	if (!vis_rows.size()) return;

	srviface *iface = theUI._tuTreeWidget2->getServerSelected();

	DPS::Client::NCS_var orb_ncs(new DPS::Client::NCS);
	DPS::Client::ClientType ncType = DPS::Client::Generic;
	int len = vis_rows.size();
	orb_ncs->length(len);
	for (int i=0; i<len; i++) {
	    DPS::Client::NominalClient nc;
		int pos = vis_rows[i];
	    try {
	    	nc.uid = boost::lexical_cast<short>(getItem(pos,TCOLsNC_Uid));
	    	nc.Type = ncType = iface->clientType(getItem(pos,TCOLsNC_Type).c_str());
	        nc.MaxClients = boost::lexical_cast<short>(getItem(pos,TCOLsNC_MaxClients));
	        nc.CPUNeeded = boost::lexical_cast<float>(getItem(pos,TCOLsNC_CPU));
	        nc.MemoryNeeded = boost::lexical_cast<short>(getItem(pos,TCOLsNC_Memory));
	        nc.WholeScriptPath = getItem(pos,TCOLsNC_ScriptPath).c_str();
	        nc.LogPath = getItem(pos,TCOLsNC_LogPath).c_str();
	        nc.SubmitCommand = getItem(pos,TCOLsNC_Submit).c_str();
	        nc.HostName = getItem(pos,TCOLsNC_HostName).c_str();
	        nc.LogInTheEnd = boost::lexical_cast<short>(getItem(pos,TCOLsNC_LoginTheEnd));
			orb_ncs[i] = nc;
	    }
	    catch(boost::bad_lexical_cast &ex) {
	    	log() << "sNC::operation-E- bad record cast" << endl;
	    	return;
	    }
	}

	DPS::Server_var pServer;
 	DPS::DBServer_var pDBServer;
 	try {
   		CORBA::String_var ref = iface->iors().c_str();
   		pServer = DPS::Server::_narrow(__theOrb->string_to_object(ref));
		ref = iface->iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex) {
    	log() << "sNC::operation-E- corba narrow error on " << iface->server() << endl;
        return;
    }
    try {
		DPS::Client::CID cid = iface->cid(iface->s_acs(),0);
		cid.Type = ncType;
		pServer->sendNCS(cid,orb_ncs);
		cid = iface->cid(iface->dbs_acs(),0);
		cid.Type = ncType;
		pDBServer->sendNCS(cid,orb_ncs);
		log() << "sNC::operation-I- sendNCS done on " << iface->server() << endl;
    }
    catch(DPS::DBProblem &dbex){
    	log() << "sNC::operation-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
    }
    catch (CORBA::SystemException &ex) {
    	log() << "sNC::operation-E- corba send error on " << iface->server() << endl;
    }
}

void sNH::operation(DPS::Client::RecordChange rc, int row)
{
	if (row < 0)
		return;
	srviface *iface = this->iface(row,TCOLsNH_Serv);

	DPS::Client::NominalHost nh;
    DPS::Client::HostStatus Status;
    short ClientsAllowed;
	try
    {
    	nh.HostName = getItem(row,TCOLsNH_HostName).c_str();
    	nh.Interface = getItem(row,TCOLsNH_Interface).c_str();
    	nh.OS = getItem(row,TCOLsNH_OS).c_str();
    	nh.CPUNumber = boost::lexical_cast<short>(getItem(row,TCOLsNH_CPUNumb));
    	nh.Memory = boost::lexical_cast<short>(getItem(row,TCOLsNH_Memory));
    	nh.Clock = boost::lexical_cast<long>(getItem(row,TCOLsNH_Clock));
        Status = iface->hostStatus(getItem(row,TCOLsNH_Status).c_str());
        ClientsAllowed = boost::lexical_cast<short>(getItem(row,TCOLsNH_ClientsAllowed));
    }
    catch(boost::bad_lexical_cast &ex)
    {
    	log() << "sNH::operation-E- bad record cast" << endl;
    	return;
    }

    DPS::Client::ActiveHost ah;
    DPS::Client::ActiveHost *pah;
    int ahid = iface->findAhsByHost(getItem(row,TCOLsNH_HostName));
    if (ahid >= 0)
    {
    	DPS::Client::ActiveHost &ah = const_cast<DPS::Client::ActiveHost &>(iface->ahs()[ahid]);
    	pah = const_cast<DPS::Client::ActiveHost *>(&ah);
		ah.ClientsAllowed = ClientsAllowed;
		ah.Status = Status;
		ah.Clock = nh.Clock;
		ah.Interface = nh.Interface;
    }
    else
    {
    	pah = &ah;
    	ah.ClientsAllowed = ClientsAllowed;
    	ah.Status = Status;
		ah.HostName = nh.HostName;
		ah.ClientsRunning = 0;
		ah.ClientsProcessed = 0;
		ah.ClientsFailed = 0;
		ah.ClientsKilled = 0;
		ah.Clock = nh.Clock;
		ah.Interface = nh.Interface;
    }

	DPS::Server_var pServer;
 	DPS::DBServer_var pDBServer;
 	try
 	{
    	CORBA::String_var ref = iface->iors().c_str();
    	pServer = DPS::Server::_narrow(__theOrb->string_to_object(ref));
		ref = iface->iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex) {
    	log() << "sNH::operation-E- corba narrow error on " << iface->server() << endl;
        return;
    }
    try
    {
        DPS::Client::CID cid = iface->cid(iface->s_acs(),0);
        cid.Type = DPS::Client::Server;
    	pServer->sendNH(cid,nh,rc);
    	pServer->sendAH(cid,*pah,rc);
		cid = iface->cid(iface->dbs_acs(),0);
        cid.Type = DPS::Client::Server;
		DPS::Client::CID_var cidv = new DPS::Client::CID(cid);
		DPS::Client::NominalHost_var nhv = new DPS::Client::NominalHost(nh);
		pDBServer->sendNH(cidv,nhv,rc);
		DPS::Client::ActiveHost_var ahv = new DPS::Client::ActiveHost(*pah);
		pDBServer->sendAH(cidv,ahv,rc);
		log() << "sNH::operation-I- " << "sendAH " << getItem(row,TCOLsNH_HostName) << endl;
    }
    catch(DPS::DBProblem &dbex){
    	log() << "sNH::operation-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
    }
    catch (CORBA::SystemException &ex) {
    	log() << "sNH::operation-E- corba send error on " << iface->server() << endl;
    }
}

void sNH::operation(DPS::Client::RecordChange rc)
{
}

void sNK::operation(DPS::Client::RecordChange rc, int row)
{
	if (row < 0)
		return;
	srviface *iface = this->iface(row,TCOLsSE_Serv);

    DPS::Client::NominalClient nk;
    try
    {
    	nk.uid = boost::lexical_cast<short>(getItem(row,TCOLsNC_Uid));
    	nk.Type = iface->clientType(getItem(row,TCOLsNC_Type).c_str());
        nk.MaxClients = boost::lexical_cast<short>(getItem(row,TCOLsNC_MaxClients));
        nk.CPUNeeded = boost::lexical_cast<float>(getItem(row,TCOLsNC_CPU));
        nk.MemoryNeeded = boost::lexical_cast<short>(getItem(row,TCOLsNC_Memory));
        nk.WholeScriptPath = getItem(row,TCOLsNC_ScriptPath).c_str();
        nk.LogPath = getItem(row,TCOLsNC_LogPath).c_str();
        nk.SubmitCommand = getItem(row,TCOLsNC_Submit).c_str();
        nk.HostName = getItem(row,TCOLsNC_HostName).c_str();
        nk.LogInTheEnd = boost::lexical_cast<short>(getItem(row,TCOLsNC_LoginTheEnd));
    }
    catch(boost::bad_lexical_cast &ex)
    {
    	log() << "sNK::operation-E- bad record cast" << endl;
    	return;
    }

	DPS::Server_var pServer;
 	DPS::DBServer_var pDBServer;
 	try
 	{
    	CORBA::String_var ref = iface->iors().c_str();
    	pServer = DPS::Server::_narrow(__theOrb->string_to_object(ref));
		ref = iface->iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex) {
    	log() << "sNK::operation-E- corba narrow error on " << iface->server() << endl;
        return;
    }
    try
    {
        DPS::Client::CID cid = iface->cid(iface->s_acs(),0);
        cid.Type = nk.Type;
    	pServer->sendNK(cid,nk,rc);
		cid = iface->cid(iface->dbs_acs(),0);
        cid.Type = nk.Type;
		DPS::Client::CID_var cidv = new DPS::Client::CID(cid);
		DPS::Client::NominalClient_var nkv = new DPS::Client::NominalClient(nk);
		pDBServer->sendNK(cidv,nkv,rc);
		log() << "sNK::operation-I-sendNK uid " << getItem(row,TCOLsNC_Uid) << ", host " << getItem(row,TCOLsNC_HostName) << endl;
    }
    catch(DPS::DBProblem &dbex){
    	log() << "sNK::operation-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
    }
    catch (CORBA::SystemException &ex) {
    	log() << "sNK::operation-E- corba send error on " << iface->server() << endl;
    }
}

void sNK::operation(DPS::Client::RecordChange rc)
{
	if (this != theUI.getVisibleCtrlTab()) return;

	vector<int> vis_rows;
	for (int len=(*this)->rowCount(),i=0; i<len; i++)
		if (!(*this)->isRowHidden(i)) vis_rows.push_back(i);
	if (!vis_rows.size()) return;

	srviface *iface = theUI._tuTreeWidget2->getServerSelected();

	DPS::Client::NCS_var orb_ncs(new DPS::Client::NCS);
	DPS::Client::ClientType ncType = DPS::Client::Generic;
	int len = vis_rows.size();
	orb_ncs->length(len);
	for (int i=0; i<len; i++) {
	    DPS::Client::NominalClient nc;
		int pos = vis_rows[i];
	    try {
	    	nc.uid = boost::lexical_cast<short>(getItem(pos,TCOLsNC_Uid));
	    	nc.Type = ncType = iface->clientType(getItem(pos,TCOLsNC_Type).c_str());
	        nc.MaxClients = boost::lexical_cast<short>(getItem(pos,TCOLsNC_MaxClients));
	        nc.CPUNeeded = boost::lexical_cast<float>(getItem(pos,TCOLsNC_CPU));
	        nc.MemoryNeeded = boost::lexical_cast<short>(getItem(pos,TCOLsNC_Memory));
	        nc.WholeScriptPath = getItem(pos,TCOLsNC_ScriptPath).c_str();
	        nc.LogPath = getItem(pos,TCOLsNC_LogPath).c_str();
	        nc.SubmitCommand = getItem(pos,TCOLsNC_Submit).c_str();
	        nc.HostName = getItem(pos,TCOLsNC_HostName).c_str();
	        nc.LogInTheEnd = boost::lexical_cast<short>(getItem(pos,TCOLsNC_LoginTheEnd));
			orb_ncs[i] = nc;
	    }
	    catch(boost::bad_lexical_cast &ex) {
	    	log() << "sNK::operation-E- bad record cast" << endl;
	    	return;
	    }
	}

	DPS::Server_var pServer;
 	DPS::DBServer_var pDBServer;
 	try {
   		CORBA::String_var ref = iface->iors().c_str();
   		pServer = DPS::Server::_narrow(__theOrb->string_to_object(ref));
		ref = iface->iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex) {
    	log() << "sNK::operation-E- corba narrow error on " << iface->server() << endl;
        return;
    }
    try {
		DPS::Client::CID cid = iface->cid(iface->s_acs(),0);
		cid.Type = ncType;
		pServer->sendNKS(cid,orb_ncs);
		cid = iface->cid(iface->dbs_acs(),0);
		cid.Type = ncType;
		pDBServer->sendNKS(cid,orb_ncs);
		log() << "sNK::operation-I- sendNKS done on " << iface->server() << endl;
    }
    catch(DPS::DBProblem &dbex){
    	log() << "sNK::operation-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
    }
    catch (CORBA::SystemException &ex) {
    	log() << "sNK::operation-E- corba send error on " << iface->server() << endl;
    }
}

void sSE::operation(DPS::Client::RecordChange rc, int row)
{
	if (rc == DPS::Client::Delete)
	{
		log() << "sSE::operation-F- remove is not supported here" << endl;
        return;
	}
	if (row < 0)
		return;
	srviface *iface = this->iface(row,TCOLsSE_Serv);

	DPS::Server_var pServer;
	DPS::DBServer_var pDBServer;
	try
    {
    	CORBA::String_var ref = iface->iors().c_str();
    	pServer = DPS::Server::_narrow(__theOrb->string_to_object(ref));
    	ref = iface->iord().c_str();
    	pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex) {
    	log() << "sSE::operation-E- corba narrow error on " << iface->server() << endl;
        return;
    }
    try
    {
        DPS::Client::CID cid = iface->cid(iface->s_acs(),0);
        cid.Type = DPS::Client::Server;
    	string var  = getItem(row,TCOLsSE_Var);
    	string path = getItem(row,TCOLsSE_Path);
    	pServer->setEnv(cid,var.c_str(),path.c_str());
    	if (var == "AMSLogVrd") {
    		int envid = iface->_map_env_by_var.n_key(var.c_str());
    		if (envid >= 0)
    			iface->_map_env_by_var.rm(envid);
    		iface->_map_env_by_var.add(var.c_str(),path);
    		iface->vrd().config = var;
    	}
    	pDBServer->setEnv(cid,var.c_str(),path.c_str());
    	log() << "sSE::operation-I- " << "setEnv $" << var << endl;
    }
    catch (CORBA::SystemException &ex) {
    	log() << "sSE::operation-E- corba send error on " << iface->server() << endl;
    }
}

void pNC::operation(DPS::Client::RecordChange rc, int row)
{
	if (row < 0)
		return;
	srviface *iface = this->iface(row,TCOLsSE_Serv);

    DPS::Client::NominalClient nc;
    try {
    	nc.uid = boost::lexical_cast<short>(getItem(row,TCOLsNC_Uid));
    	nc.Type = iface->clientType(getItem(row,TCOLsNC_Type).c_str());
        nc.MaxClients = boost::lexical_cast<short>(getItem(row,TCOLsNC_MaxClients));
        nc.CPUNeeded = boost::lexical_cast<float>(getItem(row,TCOLsNC_CPU));
        nc.MemoryNeeded = boost::lexical_cast<short>(getItem(row,TCOLsNC_Memory));
        nc.WholeScriptPath = getItem(row,TCOLsNC_ScriptPath).c_str();
        nc.LogPath = getItem(row,TCOLsNC_LogPath).c_str();
        nc.SubmitCommand = getItem(row,TCOLsNC_Submit).c_str();
        nc.HostName = getItem(row,TCOLsNC_HostName).c_str();
        nc.LogInTheEnd = boost::lexical_cast<short>(getItem(row,TCOLsNC_LoginTheEnd));
    }
    catch(boost::bad_lexical_cast &ex) {
    	log() << "pNC::operation-E- bad record cast" << endl;
    	return;
    }

	DPS::Server_var pServer;
 	DPS::DBServer_var pDBServer;
 	try {
   		CORBA::String_var ref = iface->iors().c_str();
   		pServer = DPS::Server::_narrow(__theOrb->string_to_object(ref));
		ref = iface->iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex) {
    	log() << "pNC::operation-E- corba narrow error on " << iface->server() << endl;
        return;
    }
    try {
		DPS::Client::CID cid = iface->cid(iface->s_acs(),0);
		cid.Type = nc.Type;
		pServer->sendNC(cid,nc,rc);
		cid = iface->cid(iface->dbs_acs(),0);
		cid.Type = nc.Type;
		DPS::Client::CID_var cidv = new DPS::Client::CID(cid);
		DPS::Client::NominalClient_var ncv = new DPS::Client::NominalClient(nc);
		pDBServer->sendNC(cidv,ncv,rc);
		log() << "pNC::operation-I- sendNC uid " << getItem(row,TCOLsNC_Uid) <<", host " << getItem(row,TCOLsNC_HostName) << endl;
    }
    catch(DPS::DBProblem &dbex){
    	log() << "pNC::operation-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
    }
    catch (CORBA::SystemException &ex) {
    	log() << "pNC::operation-E- corba send error on " << iface->server() << endl;
    }
}

void pNC::operation(DPS::Client::RecordChange rc) 
{
	if (this != theUI.getVisibleCtrlTab()) return;
	
	vector<int> vis_rows;
	for (int len=(*this)->rowCount(),i=0; i<len; i++)
		if (!(*this)->isRowHidden(i)) vis_rows.push_back(i);
	if (!vis_rows.size()) return;
	
	srviface *iface = theUI._tuTreeWidget2->getServerSelected();

	DPS::Client::NCS_var orb_ncs(new DPS::Client::NCS);
	DPS::Client::ClientType ncType = DPS::Client::Generic;
	int len = vis_rows.size();
	orb_ncs->length(len);
	for (int i=0; i<len; i++) {
	    DPS::Client::NominalClient nc;
		int pos = vis_rows[i]; 
	    try {
	    	nc.uid = boost::lexical_cast<short>(getItem(pos,TCOLsNC_Uid));
	    	nc.Type = ncType = iface->clientType(getItem(pos,TCOLsNC_Type).c_str());
	        nc.MaxClients = boost::lexical_cast<short>(getItem(pos,TCOLsNC_MaxClients));
	        nc.CPUNeeded = boost::lexical_cast<float>(getItem(pos,TCOLsNC_CPU));
	        nc.MemoryNeeded = boost::lexical_cast<short>(getItem(pos,TCOLsNC_Memory));
	        nc.WholeScriptPath = getItem(pos,TCOLsNC_ScriptPath).c_str();
	        nc.LogPath = getItem(pos,TCOLsNC_LogPath).c_str();
	        nc.SubmitCommand = getItem(pos,TCOLsNC_Submit).c_str();
	        nc.HostName = getItem(pos,TCOLsNC_HostName).c_str();
	        nc.LogInTheEnd = boost::lexical_cast<short>(getItem(pos,TCOLsNC_LoginTheEnd));
			orb_ncs[i] = nc;
	    } 
	    catch(boost::bad_lexical_cast &ex) {
	    	log() << "pNC::operation-E- bad record cast" << endl;
	    	return;
	    }
	}

	DPS::Server_var pServer;
 	DPS::DBServer_var pDBServer;
 	try {
   		CORBA::String_var ref = iface->iors().c_str();
   		pServer = DPS::Server::_narrow(__theOrb->string_to_object(ref));
		ref = iface->iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex) {
    	log() << "pNC::operation-E- corba narrow error on " << iface->server() << endl;
        return;
    }
    try {
		DPS::Client::CID cid = iface->cid(iface->s_acs(),0);
		cid.Type = ncType;
		pServer->sendNCS(cid,orb_ncs);
		cid = iface->cid(iface->dbs_acs(),0);
		cid.Type = ncType;
		pDBServer->sendNCS(cid,orb_ncs);
		log() << "pNC::operation-I- sendNCS done on " << iface->server() << endl;
    }
    catch(DPS::DBProblem &dbex){
    	log() << "pNC::operation-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
    }
    catch (CORBA::SystemException &ex) {
    	log() << "pNC::operation-E- corba send error on " << iface->server() << endl;
    }
}

void pAC::operation(DPS::Client::RecordChange rc, int row)
{
	if (row < 0)
		return;
	srviface *iface = this->iface(row,TCOLpAC_Serv);

	long uid;
	DPS::Client::ClientStatusType StatusType;
	time_t TimeOut;
	try {
    	uid = boost::lexical_cast<long>(getItem(row,TCOLpAC_Uid));
		StatusType = iface->clientStatusType(getItem(row,TCOLpAC_Type).c_str());
		TimeOut = boost::lexical_cast<time_t>(getItem(row,TCOLpAC_Timeout));
    }
	catch(boost::bad_lexical_cast &ex) {
		log() << "pAC::operation-E- bad record cast" << endl;
		return;
	}

	int acid = iface->findAcsByUid(uid);
	if (acid < 0) {
		log() << "pAC::operation-E- uid " << getItem(row,TCOLpAC_Uid) << " has no active client" << endl;
		return;
	}
	DPS::Client::ActiveClient &ac = const_cast<DPS::Client::ActiveClient &>(iface->acs()[acid]);
	ac.StatusType = StatusType;
	ac.TimeOut = TimeOut;

    if (rc != DPS::Client::Delete) rc = DPS::Client::Update;
    try {
    	log() << "pAC::operation-I- uid " << getItem(row,TCOLpAC_Uid) << endl;
    	iface->sendActiveClient(ac,rc);
    }
    catch (string &ex) {
    	log() << ex << endl;
    }
}

void pNH::operation(DPS::Client::RecordChange rc, int row)
{
	if (row < 0)
		return;
	srviface *iface = this->iface(row,TCOLpNH_Serv);

	DPS::Client::NominalHost nh;
    DPS::Client::HostStatus Status;
    short ClientsAllowed;
    long ClientsProcessed;
    long ClientsFailed;
	try {
    	nh.HostName = getItem(row,TCOLpNH_HostName).c_str();
    	nh.Interface = getItem(row,TCOLpNH_Interface).c_str();
    	nh.OS = getItem(row,TCOLpNH_OS).c_str();
    	nh.CPUNumber = boost::lexical_cast<short>(getItem(row,TCOLpNH_CPUNumb));
    	nh.Memory = boost::lexical_cast<short>(getItem(row,TCOLpNH_Memory));
    	nh.Clock = boost::lexical_cast<long>(getItem(row,TCOLpNH_Clock));
        Status = iface->hostStatus(getItem(row,TCOLpNH_Status).c_str());
        ClientsAllowed = boost::lexical_cast<short>(getItem(row,TCOLpNH_ClientsAllowed));
        ClientsProcessed = boost::lexical_cast<long>(getItem(row,TCOLpNH_CliProcessed));
        ClientsFailed = boost::lexical_cast<long>(getItem(row,TCOLpNH_CliFailed));
    }
    catch(boost::bad_lexical_cast &ex) {
    	log() << "pNH::operation-E- bad record cast" << endl;
    	return;
    }

    DPS::Client::ActiveHost ah;
    DPS::Client::ActiveHost *pah;
    int ahid = iface->findAhsByHost(getItem(row,TCOLpNH_HostName));
    if (ahid >= 0) {
    	DPS::Client::ActiveHost &ah = const_cast<DPS::Client::ActiveHost &>(iface->ahs()[ahid]);
    	pah = const_cast<DPS::Client::ActiveHost *>(&ah);
		ah.ClientsAllowed = ClientsAllowed;
		ah.ClientsProcessed = ClientsProcessed;
		ah.ClientsFailed = ClientsFailed;
		ah.Status = Status;
		ah.Clock = nh.Clock;
		ah.Interface = nh.Interface;
    }
    else {
    	pah = &ah;
    	ah.ClientsAllowed = ClientsAllowed;
    	ah.Status = Status;
		ah.HostName = nh.HostName;
		ah.ClientsRunning = 0;
		ah.ClientsProcessed = 0;
		ah.ClientsFailed = 0;
		ah.ClientsKilled = 0;
		ah.Clock = nh.Clock;
		ah.Interface = nh.Interface;
        ah.LastFailed = 0;
        ah.LastUpdate = time(0);
    }

	DPS::Server_var pServer;
 	DPS::DBServer_var pDBServer;
 	try {
    	CORBA::String_var ref = iface->iors().c_str();
    	pServer = DPS::Server::_narrow(__theOrb->string_to_object(ref));
		ref = iface->iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex) {
    	log() << "pNH::operation-E- corba narrow error on " << iface->server() << endl;
        return;
    }
    try {
        DPS::Client::CID cid = iface->cid(iface->s_acs(),0);
        cid.Type = DPS::Client::Server;
    	pServer->sendNH(cid,nh,rc);
        cid.Type = DPS::Client::Producer;
    	pServer->sendAH(cid,*pah,rc);
		cid = iface->cid(iface->dbs_acs(),0);
        cid.Type = DPS::Client::Server;
		DPS::Client::CID_var nhcidv = new DPS::Client::CID(cid);
		DPS::Client::NominalHost_var nhv = new DPS::Client::NominalHost(nh);
		pDBServer->sendNH(nhcidv,nhv,rc);
        cid.Type = DPS::Client::Producer;
		DPS::Client::CID_var ahcidv = new DPS::Client::CID(cid);
		DPS::Client::ActiveHost_var ahv = new DPS::Client::ActiveHost(*pah);
		pDBServer->sendAH(ahcidv,ahv,rc);
		log() << "pNH::operation-I- " << "sendNH " << getItem(row,TCOLpNH_HostName) << endl;
    }
    catch(DPS::DBProblem &dbex){
    	log() << "pNH::operation-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
    }
    catch (CORBA::SystemException &ex) {
    	log() << "pNH::operation-E- corba send error on " << iface->server() << endl;
    }
}

void pNH::operation(DPS::Client::RecordChange rc)
{
	if (this != theUI.getVisibleCtrlTab()) return;

	vector<int> vis_rows;
	for (int len=(*this)->rowCount(),i=0; i<len; i++)
		if (!(*this)->isRowHidden(i)) vis_rows.push_back(i);
	if (!vis_rows.size()) return;

	srviface *iface = theUI._tuTreeWidget2->getServerSelected();

	DPS::Client::NHS_var orb_nhs(new DPS::Client::NHS);
	DPS::Client::AHS_var orb_ahs(new DPS::Client::AHS);
	int len = vis_rows.size();
	orb_nhs->length(len);
	orb_ahs->length(len);
	for (int i=0; i<len; i++) {
	    int pos = vis_rows[i];

	    DPS::Client::NominalHost nh;
	    DPS::Client::HostStatus Status;
	    short ClientsAllowed;
	    long ClientsProcessed;
	    long ClientsFailed;
		try {
	    	nh.HostName = getItem(pos,TCOLpNH_HostName).c_str();
	    	nh.Interface = getItem(pos,TCOLpNH_Interface).c_str();
	    	nh.OS = getItem(pos,TCOLpNH_OS).c_str();
	    	nh.CPUNumber = boost::lexical_cast<short>(getItem(pos,TCOLpNH_CPUNumb));
	    	nh.Memory = boost::lexical_cast<short>(getItem(pos,TCOLpNH_Memory));
	    	nh.Clock = boost::lexical_cast<long>(getItem(pos,TCOLpNH_Clock));
	        Status = iface->hostStatus(getItem(pos,TCOLpNH_Status).c_str());
	        ClientsAllowed = boost::lexical_cast<short>(getItem(pos,TCOLpNH_ClientsAllowed));
	        ClientsProcessed = boost::lexical_cast<long>(getItem(pos,TCOLpNH_CliProcessed));
	        ClientsFailed = boost::lexical_cast<long>(getItem(pos,TCOLpNH_CliFailed));
	        orb_nhs[i] = nh;
	    }
	    catch(boost::bad_lexical_cast &ex) {
	    	log() << "pNHS::operation-E- bad record cast" << endl;
	    	return;
	    }
	    DPS::Client::ActiveHost ah;
	    DPS::Client::ActiveHost *pah;
	    int ahid = iface->findAhsByHost(getItem(pos,TCOLpNH_HostName));
	    if (ahid >= 0) {
	    	DPS::Client::ActiveHost &ah = const_cast<DPS::Client::ActiveHost &>(iface->ahs()[ahid]);
	    	pah = const_cast<DPS::Client::ActiveHost *>(&ah);
			ah.ClientsAllowed = ClientsAllowed;
			ah.ClientsProcessed = ClientsProcessed;
			ah.ClientsFailed = ClientsFailed;
			ah.Status = Status;
			ah.Clock = nh.Clock;
			ah.Interface = nh.Interface;
	    }
	    else {
	    	pah = &ah;
	    	ah.ClientsAllowed = ClientsAllowed;
	    	ah.Status = Status;
			ah.HostName = nh.HostName;
			ah.ClientsRunning = 0;
			ah.ClientsProcessed = 0;
			ah.ClientsFailed = 0;
			ah.ClientsKilled = 0;
			ah.Clock = nh.Clock;
			ah.Interface = nh.Interface;
	        ah.LastFailed = 0;
	        ah.LastUpdate = time(0);
	    }
        orb_ahs[i] = *pah;
	}

	DPS::Server_var pServer;
 	DPS::DBServer_var pDBServer;
 	try {
    	CORBA::String_var ref = iface->iors().c_str();
    	pServer = DPS::Server::_narrow(__theOrb->string_to_object(ref));
		ref = iface->iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex) {
    	log() << "pNHS::operation-E- corba narrow error on " << iface->server() << endl;
        return;
    }
    try {
        DPS::Client::CID cid = iface->cid(iface->s_acs(),0);
        cid.Type = DPS::Client::Server;
    	pServer->sendNHS(cid,orb_nhs);
        cid.Type = DPS::Client::Producer;
    	pServer->sendAHS(cid,orb_ahs);
		cid = iface->cid(iface->dbs_acs(),0);
        cid.Type = DPS::Client::Server;
		pDBServer->sendNHS(cid,orb_nhs);
        cid.Type = DPS::Client::Producer;
		pDBServer->sendAHS(cid,orb_ahs);
		log() << "pNC::operation-I- sendNHS done on " << iface->server() << endl;
    }
    catch(DPS::DBProblem &dbex){
    	log() << "pNHS::operation-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
    }
    catch (CORBA::SystemException &ex) {
    	log() << "pNHS::operation-E- corba send error on " << iface->server() << endl;
    }
}

void pNNT::operation(DPS::Client::RecordChange rc, int row)
{
	if (row < 0)
		return;
	srviface *iface = this->iface(row,TCOLpNNT_Serv);

	DPS::Producer::DSTInfo dsti;
	try
    {
		dsti.type = iface->dstType(getItem(row,TCOLpNNT_DstType).c_str());
		dsti.uid = boost::lexical_cast<unsigned long>(getItem(row,TCOLpNNT_Uid));
		dsti.HostName = getItem(row,TCOLpNNT_HostName).c_str();
		dsti.OutputDirPath = getItem(row,TCOLpNNT_OutDirPath).c_str();
		dsti.Mode = iface->runMode(getItem(row,TCOLpNNT_RunMode).c_str());
		dsti.UpdateFreq = boost::lexical_cast<long>(getItem(row,TCOLpNNT_UpdFreq));
    }
	catch(boost::bad_lexical_cast &ex) {
		log() << "pNNT::operation-E- bad record cast" << endl;
		return;
	}
 	DPS::Producer_var pProducer;
 	DPS::DBServer_var pDBServer;
   try {
    	CORBA::String_var ref = iface->iorp().c_str();
    	pProducer = DPS::Producer::_narrow(__theOrb->string_to_object(ref));
		ref = iface->iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex) {
    	log() << "pNNT::operation-E- corba narrow error on " << iface->server() << endl;
        return;
    }
    try {
		pProducer->sendDSTInfo(dsti,rc);
		pDBServer->sendDSTInfo(dsti,rc);
		log() << "pNNT::operation-I-sendDSTInfo uid " << getItem(row,TCOLpNNT_Uid) << endl;
    }
    catch (CORBA::SystemException &ex) {
    	log() << "pNNT::operation-E- corba send error on " << iface->server() << endl;
    }
}

void pPNT::operation(DPS::Client::RecordChange rc, int row)
{
	if (row < 0)
		return;
	srviface *iface = this->iface(row,TCOLpPNT_Serv);

	DPS::Producer::DST dst;
	try
    {
		string s = getItem(row,TCOLpPNT_Time);
		CharStringToken st(s.c_str()," \t\r\n");
		if (st.tokens() >= 2)
		{
			CharStringToken sdate(st.token(0),".");
			CharStringToken stime(st.token(1),":");
			if (sdate.tokens() >= 3 && stime.tokens() >= 3)
			{
				struct tm t;
				t.tm_hour = atoi(stime.token(0));
				t.tm_min = atoi(stime.token(1));
				t.tm_sec = atoi(stime.token(2));
				t.tm_year = atoi(sdate.token(0))-1900;
				t.tm_mon = atoi(sdate.token(1))-1;
				t.tm_mday = atoi(sdate.token(2));
				dst.Insert = mktime(&t);
				goto nextparse;
			}
		}
		log() << "pPNT::operation-E- bad record cast" << endl;
		return;
nextparse:
		dst.Run = boost::lexical_cast<long>(getItem(row,TCOLpPNT_Run));
		dst.FirstEvent = boost::lexical_cast<long>(getItem(row,TCOLpPNT_FirstEv));
		dst.LastEvent = boost::lexical_cast<long>(getItem(row,TCOLpPNT_LastEv));
		dst.Status = iface->dstStatus(getItem(row,TCOLpPNT_Status).c_str());
		dst.Type = iface->dstType(getItem(row,TCOLpPNT_Type).c_str());
		dst.size = boost::lexical_cast<long>(getItem(row,TCOLpPNT_Size));
		dst.Name = getItem(row,TCOLpPNT_Path).c_str();
		dst.Version = getItem(row,TCOLpPNT_Version).c_str();
    }
	catch(boost::bad_lexical_cast &ex) {
		log() << "pPNT::operation-E- bad record cast" << endl;
		return;
	}

 	DPS::Producer_var pProducer;
	DPS::Client::CID cid = iface->mycid();
    try {
    	CORBA::String_var ref = iface->iorp().c_str();
    	pProducer = DPS::Producer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex) {
    	log() << "pPNT::operation-E- corba narrow error on " << iface->server() << endl;
        return;
    }
    try {
		pProducer->sendDSTEnd(cid,dst,rc);
		log() << "pPNT::operation-I-sendDSTEnd run " << getItem(row,TCOLpPNT_Run) << endl;
    }
    catch (CORBA::SystemException &ex) {
    	log() << "pPNT::operation-E- corba send error on " << iface->server() << endl;
    }
}

void pRT::operation(DPS::Client::RecordChange rc, int row)
{
	if (row < 0)
		return;
	srviface *iface = this->iface(row,TCOLpRT_Serv);

	unsigned long Run;
	unsigned long uid;
	long FirstEvent;
	long LastEvent;
	time_t TFEvent;
	time_t TLEvent;
	long Priority;
	string FilePath;
	DPS::Producer::RunStatus Status;
	DPS::Producer::RunStatus History;
	unsigned long CounterFail;
	unsigned long cuid;
	long DataMC;
	string HostName;
	try
    {
    	Run = boost::lexical_cast<unsigned long>(getItem(row,TCOLpRT_Run));
    	uid = boost::lexical_cast<unsigned long>(getItem(row,TCOLpRT_Uid));
    	FirstEvent = boost::lexical_cast<long>(getItem(row,TCOLpRT_FirstEv));
    	LastEvent = boost::lexical_cast<long>(getItem(row,TCOLpRT_LastEv));
    	TFEvent = boost::lexical_cast<time_t>(getItem(row,TCOLpRT_TEvent));
    	TLEvent = boost::lexical_cast<time_t>(getItem(row,TCOLpRT_TLEvent));
    	Priority = boost::lexical_cast<long>(getItem(row,TCOLpRT_Priority));
    	FilePath = getItem(row,TCOLpRT_FilePath);
    	Status = iface->runStatus(getItem(row,TCOLpRT_Status).c_str());
    	History = iface->runStatus(getItem(row,TCOLpRT_History).c_str());
    	CounterFail = boost::lexical_cast<unsigned long>(getItem(row,TCOLpRT_Fails));
    	cuid = boost::lexical_cast<unsigned long>(getItem(row,TCOLpRT_CUid));
    	DataMC = boost::lexical_cast<long>(getItem(row,TCOLpRT_DataMC));
    	HostName = getItem(row,TCOLpRT_HostName);
    }
	catch(boost::bad_lexical_cast &ex) {
		log() << "pRT::operation-E- bad record cast" << endl;
		return;
	}

    int runid = iface->findRunEvInfoByRunUid(Run,uid);
    if (runid < 0) {
    	log() << "pRT::operation-E- run " << getItem(row,TCOLpRT_Run) << " not found" << endl;
		return;
    }
    if (rc == DPS::Client::Delete && iface->findAcsByUid(uid) >= 0) {
    	log() << "pRT::operation-E- run " << getItem(row,TCOLpRT_Run) << " has active client, should be killed before" << endl;
		return;
    }

	DPS::Producer::RunEvInfo &run = const_cast<DPS::Producer::RunEvInfo &>(iface->res()[runid]);
	run.uid = uid;
	run.FirstEvent = FirstEvent;
	run.LastEvent = LastEvent;
	run.TFEvent = TFEvent;
	run.TLEvent = TLEvent;
	run.Priority = Priority;
	run.FilePath = FilePath.c_str();
	run.Status = Status;
	run.History = History;
	run.CounterFail = CounterFail;
	run.cuid = cuid;
	run.DataMC = DataMC;
	run.cinfo.HostName = HostName.c_str();

    if (rc != DPS::Client::Delete) rc = DPS::Client::Update;
    try {
    	iface->sendRun(run,rc);
    	log() << "pRT::operation-I- run " << getItem(row,TCOLpRT_Run) <<
    			 ", uid " << getItem(row,TCOLpRT_Uid) << endl;
    }
    catch (string &ex) {
    	log() << ex << endl;
    }
}

//_____________________________________________________________________
// Other tables (double)click

void pAC::clkcSelection(int row, int column)
{
	tabUtils::clkcSelection(row,column);
	uistring suid = getItem(row,TCOLpAC_Uid);
	vector<int> vprt = theUI._tupRT->findRows(suid.q_str(),TCOLpRT_Uid);
	if (vprt.size())
	{
		if (!theUI._tupRT->rowSelected(vprt[0])) {
			theUI._tupRT->clkcSelection(vprt[0],0);
			theUI._tupRT->rowSelection(vprt[0]);
		}
	}
	else
		theUI._tupRT->rowSelection();
}

void pRT::clkcSelection(int row, int column)
{
	tabUtils::clkcSelection(row,column);
	uistring suid = getItem(row,TCOLpRT_Uid);
	vector<int> vpac = theUI._tupAC->findRows(suid.q_str(),TCOLpAC_Uid);
	if (vpac.size())
	{
		if (!theUI._tupAC->rowSelected(vpac[0])) {
			theUI._tupAC->clkcSelection(vpac[0],0);
			theUI._tupAC->rowSelection(vpac[0]);
		}
	}
	else
		theUI._tupAC->rowSelection();
}

void ActiveHosts::clkSelection(int row, int column)
{
	theUI.tableclick(this,row,TCOLAH_HostName);
}

void ActiveClients::clkSelection(int row, int column)
{
	theUI.tableclick(this,row,TCOL_Run);
}

void ActiveClients::dblclkSelection(int row, int column)
{
	srviface *iface = this->iface(row,column);
	try {
		int runid = iface->findRunEvInfoByUid(boost::lexical_cast<long>(getItem(row,TCOL_Id)));
		if (runid >= 0) {
			const DPS::Producer::RunEvInfo RunEvInfo = iface->res()[runid];
			string sfile("/afs/cern.ch/ams/Offline/AMSDataDir/prod.log/scripts/");
			sfile += RunEvInfo.FilePath;
			log() << "ActiveClients::dblclkSelection-I- " << sfile << " is being shown" << endl;
			viewer *myViewer = new viewer();
			myViewer->setupUi();
			_thread<viewer *> thr;
			myViewer->setFile(sfile.c_str());
			thr.run(&thrViewer,myViewer);
			(*myViewer)->show();
			return;
		}
    }
	catch(boost::bad_lexical_cast &ex) {
	}
	log() << "ActiveClients::dblclkSelection-F- failed" << endl;
}

void ProducerRuns::clkSelection(int row, int column)
{
	theUI.tableclick(this,row,TCOLPR_Run);
}

void ProducerRuns::dblclkSelection(int row, int column)
{
	srviface *iface = this->iface(row,column);
	string srun = getItem(row,TCOLPR_Run);
	string suid = getItem(row,TCOLPR_Uid);
	string shost = getItem(row,TCOLPR_Host);

	try {
		long run = boost::lexical_cast<long>(srun);
		long uid = boost::lexical_cast<long>(suid);
	    if (!(QApplication::keyboardModifiers() & Qt::ControlModifier)) {
	    	if (run <= 0) {
				log() << "ProducerRuns::dblclkSelection-E- wrong run: " << srun <<endl;
				return;
	    	}
			string sfile;
			long uid = boost::lexical_cast<long>(suid);
			int acid = iface->findAcsByUid(uid);
			bool is_condor = false;;
			if (acid>=0 && iface->acs()[acid].id.pid>0) {
				string sahost = (const char *)iface->acs()[acid].id.HostName;
				CharStringToken st(sahost,".");
				if (st.tokens() >= 1) {
					const DPS::Server::NCS_var &NCs = iface->ncs();
					for (int i=0,l=NCs->length(); i<l; i++) {
						DPS::Client::NominalClient nc = NCs[i];
						string slogpath = (const char *)nc.LogPath;
						string shost = (const char *)nc.HostName;
						if (shost==st.token(0) && slogpath.find("condor")!=string::npos) {
								is_condor = true;
								break;
						}
					}
				}
			}
			if (__baseUser==common::user() && acid>=0 && iface->acs()[acid].id.pid>=1000000) {
				stringstream s;
				s << boost::format("%d") %iface->acs()[acid].id.pid;
				sfile = shost + "@lsf::" + s.str();
			}
			else if (__baseUser == common::user() && is_condor) {
				stringstream s;
				s << boost::format("%d.%d") %iface->acs()[acid].id.pid %iface->acs()[acid].id.ppid;
				sfile = shost + "@condor::" + s.str();
			}
			else {
				string cmd = "ls -ltr --time-style=full-iso /afs/cern.ch/ams/local/logs/*Producer.*"+suid+"*.log* | tail -10";
				process proc;
			    if (!proc.run(cmd)) {
			    	vector<flm> logfl;
				    while (proc.getline(sfile)) {
						flm logf(sfile);
						if (!logf.parse()) {
							sfile = logf.file;
							if (__baseUser==common::user() && (acid>=0 && srviface::pingHost(shost))) {
								CharStringToken st(shost,".");
								string myhost = common::hostname();
								CharStringToken st2(myhost,".");
								if (st.tokens() == st2.tokens()) {
									if (shost != myhost) sfile = shost + "::" + sfile;
								}
								else if (strcmp(st.token(0),st2.token(0)))
									sfile = shost + "::" + sfile;
								logf.file = sfile;
							}
							logfl.push_back(logf);
						}
						else {
							log()<<"ProducerRuns::dblclkSelection-E- parse error on: "<<proc.lastcmd()<<endl;
							return;
						}
				    }
				    theUI._shLogSel._cash._data.clear();
				    unsigned szlist = logfl.size();
				    int idlist = logfl.size()-1;
				    if (idlist < 0)
				    	return;
				    else if (idlist > 0) {
						for (unsigned i=0; i<szlist; i++)
							theUI._shLogSel._cash._data.push_back(logfl[i].file);
						theUI._shLogSel._cash._selected = szlist-1;
						theUI._shLogSel << theUI._shLogSel._cash;
						selFile *file = new selFile(theUI._shLogSel.key());
						file->setupUi();
						QPoint posMouse = QCursor::pos();
						(*file)->move(posMouse);
						(*file)->exec();
						theUI._shLogSel >> theUI._shLogSel._cash;
						idlist = theUI._shLogSel._cash._selected;
				    }
				    sfile = logfl[idlist].file;
			    }
				else {
					log()<<"ProducerRuns::dblclkSelection-E- exec error on: "<<proc.lastcmd()<<endl;
					return;
				}
			}
			log()<<"ProducerRuns::dblclkSelection-I- "<<sfile<<" is being shown"<<endl;

			viewer *myViewer = new viewer();
			myViewer->setupUi();
			_thread<viewer *> thr;
			myViewer->setFile(sfile.c_str());
			thr.run(&thrViewer,myViewer);
			(*myViewer)->show();
		}
		else {
			int runid = iface->findRunEvInfoByRunUid(run,uid);
			if (runid >= 0) {
				const DPS::Producer::RunEvInfo RunEvInfo = iface->res()[runid];
				string sfile("/afs/cern.ch/ams/Offline/AMSDataDir/prod.log/scripts/");
				sfile += RunEvInfo.FilePath;
				log() << "ProducerRuns::dblclkSelection-I- " << sfile << " is being shown" << endl;
				viewer *myViewer = new viewer();
				myViewer->setupUi();
				_thread<viewer *> thr;
				myViewer->setFile(sfile.c_str());
				thr.run(&thrViewer,myViewer);
				(*myViewer)->show();
				return;
			}
			else
				log() << "ProducerRuns::dblclkSelection-E- wrong run: " << srun <<endl;
		}
    }
	catch(boost::bad_lexical_cast &ex) {
	}
	log() << "ProducerRuns::dblclkSelection-F- failed" << endl;
}

void ProducerNTuples::clkSelection(int row, int column)
{
	theUI.tableclick(this,row,TCOLPNT_Run);
}

void ServActClients::clkSelection(int row, int column)
{
	theUI.tableclick(this,row,-1);
}

void VrdLog::dblclkSelection(int row, int column)
{
	string vrdLogFile = getItem(row,TCOLVRD_File);
	if (vrdLogFile.size() <= 2) {
		log() << "VrdLog::dblclkSelection-F- failed" << endl;
		return;
	}
	CharStringToken st(vrdLogFile,"/");
	if (st.tokens() <= 2) {
		log() << "VrdLog::dblclkSelection-F- suspicious file name: " << vrdLogFile << ", rejected" << endl;
		return;
	}
	if (__baseUser == common::user()) {
		map<const char *, string>::const_iterator it = __logHost.find("vrd.acron");
		if (it != __logHost.end()) {
			string use_host = __logHost["vrd.acron"];
			CharStringToken st2(st.token(st.tokens()-1),".");
			if (st2.tokens()>4) {
				string s = string(st2.token(2))+"."+st2.token(3);
				if (s == "cern.ch")
					use_host = string(st2.token(1))+"."+s;
			}
			if (use_host != common::hostname())
				vrdLogFile = use_host + "::" + vrdLogFile;
		}
	}
	log() << "VrdLog::dblclkSelection-I- " << vrdLogFile << " is being shown" << endl;
	viewer *myViewer = new viewer();
	myViewer->setupUi();
	_thread<viewer *> thr;
	myViewer->setFile(vrdLogFile.c_str());
	thr.run(&thrViewer,myViewer);
	(*myViewer)->show();
}

void OtherLog::dblclkSelection(int row, int column)
{
	string otherLogFile = getItem(row,TCOLOTHER_File);
	string comment = getItem(row,TCOLOTHER_Comment);
	if (otherLogFile.size() <= 2) {
		log() << "OtherLog::dblclkSelection-F- failed" << endl;
		return;
	}
	CharStringToken st(otherLogFile,"/");
	if (st.tokens() <= 2) {
		log() << "OtherLog::dblclkSelection-F- suspicious file name: " << otherLogFile << ", rejected" << endl;
		return;
	}
	if (__baseUser == common::user()) {
		map<const char *, string>::const_iterator it = __logHost.find("frame_decode.acron");
		if (it != __logHost.end()) {
			string use_host;
			if (comment.find("SCDB") != string::npos) use_host = __logHost["scdb.acron"];
			  else use_host = __logHost["frame_decode.acron"];
			if (use_host != common::hostname())
				otherLogFile = use_host + "::" + otherLogFile;
		}
	}
	log() << "OtherLog::dblclkSelection-I- " << otherLogFile << " is being shown" << endl;
	viewer *myViewer = new viewer();
	myViewer->setupUi();
	_thread<viewer *> thr;
	myViewer->setFile(otherLogFile.c_str());
	thr.run(&thrViewer,myViewer);
	(*myViewer)->show();
}

//_____________________________________________________________________
// class ui private

ui::ui(): Ui_MainWindow()
{
	_evIFace.accept();
	_dataRequested = 0;
	_resFinishIn = 2;
	srand(static_cast<unsigned>(getpid()));
}

void ui::treeclick(const TreeItem *node, int column, const tabUtils *source) const
{
	srviface *iface = node->iface();
	uistring name = node->text(TRCOL_Name);

	vector<int> vActiveHosts = _tuActiveHosts->findRows(uistring(iface->vno()).q_str(),TCOLAH_Serv);
	vector<int> vActiveClients = _tuActiveClients->findRows(uistring(iface->vno()).q_str(),TCOL_Serv);
	vector<int> vProducerRuns = _tuProducerRuns->findRows(uistring(iface->vno()).q_str(),TCOLPR_Serv);
	vector<int> vProducerNTuples = _tuProducerNTuples->findRows(uistring(iface->vno()).q_str(),TCOLPNT_Serv);
	vector<int> vServActClients = _tuServActClients->findRows(uistring(iface->vno()).q_str(),TCOLSDB_Serv);
	vector<int> vDBServActClients = _tuDBServActClients->findRows(uistring(iface->vno()).q_str(),TCOLSDB_Serv);
	vector<int> vDiskUsage = _tuDiskUsage->findRows(uistring(iface->vno()).q_str(),TCOLDU_Serv);
	int pos;
	bool ntuples_done = false;

	tabUtils::clrSelection(source);
	switch (node->type())
	{
	case ITYPE_SERVER:
	{
		if (vServActClients.size() && source != _tuServActClients)
			_tuServActClients->rowSelection(vServActClients[0]);
		if (vDBServActClients.size() && source != _tuDBServActClients)
			_tuDBServActClients->rowSelection(vDBServActClients[0]);
		break;
	}
	case ITYPE_HOST:
	{
		if (vActiveHosts.size() && source != _tuActiveHosts)
		{
			 pos = _tuActiveHosts->findRow(name.q_str(),TCOLAH_HostName,vActiveHosts);
			 _tuActiveHosts->rowSelection(pos);
		}
		break;
	}
	case ITYPE_UID:
	{
		if (vActiveClients.size() && source != _tuActiveClients)
		{
			pos = _tuActiveClients->findRow(name.q_str(),TCOL_Id,vActiveClients);
			_tuActiveClients->rowSelection(pos);
		}
		if (vProducerRuns.size() && source != _tuProducerRuns)
		{
			pos = _tuProducerRuns->findRow(name.q_str(),TCOLPR_Uid,vProducerRuns);
			_tuProducerRuns->rowSelection(pos);
		}
		if (vProducerNTuples.size() && source != _tuProducerNTuples)
		{
			pos = -1;
			int runid = iface->findRunEvInfoByUid(atol(name.c_str()));
			if (runid >= 0)
			{
				const DPS::Producer::RunEvInfo RunEvInfo = iface->res()[runid];
				int dstid = iface->findLastDstByRun(RunEvInfo.Run);
				if (dstid >= 0)
				{
					const DPS::Producer::DST &Dst = iface->dsts()[dstid];
					stringstream s;
					s << boost::format("%d") %RunEvInfo.Run;
					uistring st = timeString(static_cast<time_t>(Dst.Insert));
					vector<int> ntuples = _tuProducerNTuples->findRows(uistring(s.str()).q_str(),TCOLPNT_Run,vProducerNTuples);
					vector<int> vpos = _tuProducerNTuples->findRows(st.q_str(),TCOLPNT_Time,ntuples);
					if (vpos.size())
						pos = vpos[0];
				}
			}
			_tuProducerNTuples->rowSelection(pos);
		}
		if (vActiveHosts.size() && source != _tuActiveHosts)
		{
			vector<int> vActiveClients2 = _tuActiveClients->findRows(name.q_str(),TCOL_Id,vActiveClients);
			if (vActiveClients2.size() > 0) {
				uistring host = _tuActiveClients->getItem(vActiveClients2[0],TCOL_HostName);
				pos = _tuActiveHosts->findRow(host.q_str(),TCOLAH_HostName,vActiveHosts);
			}
			else
				pos = -1;
			_tuActiveHosts->rowSelection(pos);
		}
		break;
	}
	case ITYPE_RUN:
	{
		if (vActiveClients.size() && source != _tuActiveClients)
		{
			pos = _tuActiveClients->findRow(name.q_str(),TCOL_Run,vActiveClients);
			_tuActiveClients->rowSelection(pos);
		}
		if (vProducerRuns.size() && source != _tuProducerRuns)
		{
			 pos = _tuProducerRuns->findRow(name.q_str(),TCOLPR_Run,vProducerRuns);
			 _tuProducerRuns->rowSelection(pos);
			 if (pos < 0) {
				pos = _tuProducerNTuples->findRow(name.q_str(),TCOLPNT_Run,vProducerNTuples);
				if (pos >= 0) {
					if (source != _tuProducerNTuples)
						_tuProducerNTuples->rowSelection(pos);
					ntuples_done = true;
				}
			 }
		}
		if (ntuples_done == false && vProducerNTuples.size() && source != _tuProducerNTuples)
		{
			pos = -1;
			int dstid = iface->findLastDstByRun(atol(name.c_str()));
			if (dstid >= 0)
			{
				const DPS::Producer::DST &Dst = iface->dsts()[dstid];
				uistring st = timeString(static_cast<time_t>(Dst.Insert));
				vector<int> ntuples = _tuProducerNTuples->findRows(name.q_str(),TCOLPNT_Run,vProducerNTuples);
				vector<int> vpos = _tuProducerNTuples->findRows(st.q_str(),TCOLPNT_Time,ntuples);
				if (vpos.size())
					pos = vpos[0];
			}
			_tuProducerNTuples->rowSelection(pos);
		}
		if (vActiveHosts.size() && source != _tuActiveHosts)
		{
			vector<int> vActiveClients2 = _tuActiveClients->findRows(name.q_str(),TCOL_Run,vActiveClients);
			if (vActiveClients2.size() > 0) {
				uistring host = _tuActiveClients->getItem(vActiveClients2[0],TCOL_HostName);
				pos = _tuActiveHosts->findRow(host.q_str(),TCOLAH_HostName,vActiveHosts);
			}
			else
				pos = -1;
			_tuActiveHosts->rowSelection(pos);
		}
		break;
	}
	default:
		break;
	}

	// Highlight Update AFS token menu item
	if (iface != _tuTreeWidget->getServerSelected())
		return;
	actUpdateAFSToken->setEnabled(false);
	const DPS::Client::ACS_var &s_acs = iface->dbs_acs();
	for (int i=0,len=s_acs->length(); i<len && !actUpdateAFSToken->isEnabled(); i++) {
		const DPS::Client::ActiveClient &ActiveClient = s_acs[i];
		string achost(ActiveClient.id.HostName);
		if (achost == iface->ipaddr())
			actUpdateAFSToken->setEnabled(true);
		else
			for (unsigned j=0; j<iface->ipalias().size(); j++)
				if (achost == iface->ipalias()[j])
					actUpdateAFSToken->setEnabled(true);
	}
}

void ui::tableclick(const tabUtils *table, int row, int column) const
{
	TableItem *tableItem;
	srviface *iface;
	TreeItem *node;

	treeUtils *tree = theUI._tuTreeWidget;
	if (column >= 0)
	{
		tableItem = reinterpret_cast<TableItem *>((*table)->item(row,column));
		iface = tableItem->iface();
		node = tree->findNode(uistring(iface->server()).q_str(),tableItem->text(),ITYPE_UNK);
	}
	else
	{
		tableItem = reinterpret_cast<TableItem *>((*table)->item(row,0));
		iface = tableItem->iface();
		node = tree->findNode(uistring(iface->server()).q_str(),uistring(iface->server()).q_str(),ITYPE_UNK);
	}
	table->nodeSelection(node);
}

// class ui public

bool ui::acl(const string &user)
{
	if (user == "admin" || common::user() == "ams") {
		_tusNC->editable(true);
		_tusNH->editable(true);
		_tusNK->editable(true);
		_tusSE->editable(true);
		_tupNC->editable(true);
		_tupAC->editable(true);
		_tupNH->editable(true);
		_tupNNT->editable(true);
		_tupPNT->editable(true);
		_tupRT->editable(true);
		if (user != "admin") {
			actDeleteActiveClients->setEnabled(false);
			actDeleteRuns->setEnabled(false);
		    actExportDB->setEnabled(false);
		    actExportDBMerged->setEnabled(false);
		}
		return true;
	}
	else {
		actDeleteActiveClients->setEnabled(false);
		actDeleteRuns->setEnabled(false);
		actFinishFailedRuns->setEnabled(false);
	    actExportDB->setEnabled(false);
	    actExportDBMerged->setEnabled(false);
		return false;
	}
}

void ui::setupUi(QMainWindow *MainWindow)
{
	Ui_MainWindow::setupUi(MainWindow);
	_mw = dynamic_cast<myQMainWindow *>(MainWindow);

	uistring title("AMS Production Monitor (");
	title += common::user() + "@" + uistring(QHostInfo::localHostName()) + ")";
	MainWindow->setWindowTitle(title.q_str());

	_tuTreeWidget = new treeUtils(treeWidget);
	_tuTreeWidget2 = new treeUtils(treeWidget2);
	_tuActiveHosts = new ActiveHosts(qtwActiveHosts);
	_tuActiveClients = new ActiveClients(qtwActiveClients);
	_tuProducerRuns = new ProducerRuns(qtwProducerRuns);
	_tuProducerNTuples = new ProducerNTuples(qtwProducerNTuples);
	_tuServActClients = new ServActClients(qtwServActClients);
	_tuDBServActClients = new ServActClients(qtwDBServActClients);
	_tuDiskUsage = new tabUtils(qtwDiskUsage);
	_tuVrdLog = new VrdLog(qtwVrdLog);
	_tuOtherLog = new OtherLog(qtwOtherLog);
	_tuMyLog = new mylogUtils(qtwMyLogging);

	_tusNC = new sNC(qtw_sNC);
	_tusNH = new sNH(qtw_sNH);
	_tusNK = new sNK(qtw_sNK);
	_tusSE = new sSE(qtw_sSE);
	_tupNC = new pNC(qtw_pNC);
	_tupAC = new pAC(qtw_pAC);
	_tupNH = new pNH(qtw_pNH);
	_tupNNT = new pNNT(qtw_pNNT);
	_tupPNT = new pPNT(qtw_pPNT);
	(*_tupPNT)->setColumnHidden(TCOLpPNT_Version,true);
	_tupRT = new pRT(qtw_pRT);

	for (int i=0; i<tabWidget->count(); i++)
	{
		uistring s(tabWidget->tabText(i));
		_tabs[s] = i;
		_icon_state[s] = 0;
	}

	_map_forms["Producer_ActiveHosts"] = _tuActiveHosts;
	_map_forms["Producer_ActiveClients"] = _tuActiveClients;
	_map_forms["Producer_Runs"] = _tuProducerRuns;
	_map_forms["Producer_NTuples"] = _tuProducerNTuples;
	_map_forms["Server_ActiveClients"] = _tuServActClients;
	_map_forms["DBServer_ActiveClients"] = _tuDBServActClients;
	_map_forms["DiskUsage"] = _tuDiskUsage;
	_map_forms["Vrd log file info"] = _tuVrdLog;
	_map_forms["Other log file info"] = _tuOtherLog;

	_map_ctrl_forms["Server/Nominal Client"] = pair<tabUtils *, int>(_tusNC,1);
	_map_ctrl_forms["Server/Nominal Host"] = pair<tabUtils *, int>(_tusNH,2);
	_map_ctrl_forms["Server/Nominal Killer"] = pair<tabUtils *, int>(_tusNK,3);
	_map_ctrl_forms["Server/Set Environment"] = pair<tabUtils *, int>(_tusSE,4);
	_map_ctrl_forms["Producer/Nominal Client"] = pair<tabUtils *, int>(_tupNC,5);
	_map_ctrl_forms["Producer/Active Client"] = pair<tabUtils *, int>(_tupAC,6);
	_map_ctrl_forms["Producer/Nominal Host"] = pair<tabUtils *, int>(_tupNH,7);
	_map_ctrl_forms["Producer/Nominal NTuple"] = pair<tabUtils *, int>(_tupNNT,8);
	_map_ctrl_forms["Producer/Produced NTuple"] = pair<tabUtils *, int>(_tupPNT,9);
	_map_ctrl_forms["Producer/Run Table"] = pair<tabUtils *, int>(_tupRT,10);

	QObject::connect(actUpdateStatus, SIGNAL(triggered()), this, SLOT(slUpdateStatus()));
	QObject::connect(actResetHosts, SIGNAL(triggered()), this, SLOT(mnuResetHosts()));
	QObject::connect(actResetFailedHosts, SIGNAL(triggered()), this, SLOT(mnuResetFailedHosts()));
	QObject::connect(actDeleteActiveClients, SIGNAL(triggered()), this, SLOT(mnuDeleteActiveClients()));
	QObject::connect(actDeleteRuns, SIGNAL(triggered()), this, SLOT(mnuDeleteRuns()));
	QObject::connect(actOnlyReset, SIGNAL(triggered()), this, SLOT(mnuResetFailedRuns()));
	QObject::connect(actResetRemoveDST, SIGNAL(triggered()), this, SLOT(mnuResetFailedRunsDST()));
	QObject::connect(actResetHistory, SIGNAL(triggered()), this, SLOT(mnuResetHistory()));
	QObject::connect(actDeleteValidatedDST, SIGNAL(triggered()), this, SLOT(mnuDeleteValidatedDST()));
	QObject::connect(actDeleteFailedDST, SIGNAL(triggered()), this, SLOT(mnuDeleteFailedDST()));
	QObject::connect(actFinishFailedRuns, SIGNAL(triggered()), this, SLOT(mnuFinishFailedRuns()));
	QObject::connect(actUpdateAFSToken, SIGNAL(triggered()), this, SLOT(mnuUpdateAFSToken()));
	QObject::connect(actQuit, SIGNAL(triggered()), this, SLOT(mnuQuit()));
	QObject::connect(actActiveHosts, SIGNAL(triggered()), this, SLOT(mnuActiveHosts()));
	QObject::connect(actLockUpdate, SIGNAL(triggered()), this, SLOT(mnuLockUpdate()));
	QObject::connect(actUseFilter, SIGNAL(triggered()), this, SLOT(mnuUseFilter()));
	QObject::connect(actZoomIn, SIGNAL(triggered()), this, SLOT(mnuZoomIn()));
	QObject::connect(actZoomOut, SIGNAL(triggered()), this, SLOT(mnuZoomOut()));
	QObject::connect(actClearMyLogging, SIGNAL(triggered()), this, SLOT(mnuClearMyLogging()));
	QObject::connect(actExportDB, SIGNAL(triggered()), this, SLOT(mnuExportDB()));
	QObject::connect(actExportDBMerged, SIGNAL(triggered()), this, SLOT(mnuExportDBMerged()));
	QObject::connect(actImportDB, SIGNAL(triggered()), this, SLOT(mnuImportDB()));
	QObject::connect(actSetFilter, SIGNAL(triggered()), this, SLOT(mnuSetFilter()));

	QObject::connect(theUI.pbtReplace,SIGNAL(clicked(bool)),this, SLOT(clkbtReplace(bool)));
	QObject::connect(theUI.pbtAdd,SIGNAL(clicked(bool)),this, SLOT(clkbtAdd(bool)));
	QObject::connect(theUI.pbtRemove,SIGNAL(clicked(bool)),this, SLOT(clkbtRemove(bool)));
	QObject::connect(theUI.pbtNew,SIGNAL(clicked(bool)),this, SLOT(clkbtNew(bool)));
	QObject::connect(theUI.pbtReplaceAll,SIGNAL(clicked(bool)),this, SLOT(clkbtReplaceAll(bool)));
	QObject::connect(theUI.pbtMoveUp,SIGNAL(clicked(bool)),this, SLOT(clkbtMoveUp(bool)));
	QObject::connect(theUI.pbtMoveDown,SIGNAL(clicked(bool)),this, SLOT(clkbtMoveDown(bool)));

	QObject::connect(treeWidget,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this, SLOT(clkTreeWidget(QTreeWidgetItem *, int)));
	QObject::connect(treeWidget2,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this, SLOT(clkTreeWidget2(QTreeWidgetItem *, int)));
	QObject::connect(qtwActiveHosts,SIGNAL(cellClicked(int, int)),_tuActiveHosts, SLOT(clkSelection(int, int)));
	QObject::connect(qtwActiveClients,SIGNAL(cellClicked(int, int)),_tuActiveClients, SLOT(clkSelection(int, int)));
	QObject::connect(qtwActiveClients,SIGNAL(cellDoubleClicked(int, int)),_tuActiveClients, SLOT(dblclkSelection(int, int)));
	QObject::connect(qtwProducerRuns,SIGNAL(cellClicked(int, int)),_tuProducerRuns, SLOT(clkSelection(int, int)));
	QObject::connect(qtwProducerRuns,SIGNAL(cellDoubleClicked(int, int)),_tuProducerRuns, SLOT(dblclkSelection(int, int)));
	QObject::connect(qtwProducerNTuples,SIGNAL(cellClicked(int, int)),_tuProducerNTuples, SLOT(clkSelection(int, int)));
	QObject::connect(qtwServActClients,SIGNAL(cellClicked(int, int)),_tuServActClients, SLOT(clkSelection(int, int)));
	QObject::connect(qtwDBServActClients,SIGNAL(cellClicked(int, int)),_tuDBServActClients, SLOT(clkSelection(int, int)));
	QObject::connect(qtwVrdLog,SIGNAL(cellDoubleClicked(int, int)),_tuVrdLog, SLOT(dblclkSelection(int, int)));
	QObject::connect(qtwOtherLog,SIGNAL(cellDoubleClicked(int, int)),_tuOtherLog, SLOT(dblclkSelection(int, int)));

	QObject::connect(qtw_sNC,SIGNAL(cellClicked(int, int)),_tusNC,SLOT(clkcSelection(int, int)));
	QObject::connect(qtw_sNH,SIGNAL(cellClicked(int, int)),_tusNH,SLOT(clkcSelection(int, int)));
	QObject::connect(qtw_sNK,SIGNAL(cellClicked(int, int)),_tusNK,SLOT(clkcSelection(int, int)));
	QObject::connect(qtw_sSE,SIGNAL(cellClicked(int, int)),_tusSE,SLOT(clkcSelection(int, int)));
	QObject::connect(qtw_pNC,SIGNAL(cellClicked(int, int)),_tupNC,SLOT(clkcSelection(int, int)));
	QObject::connect(qtw_pAC,SIGNAL(cellClicked(int, int)),_tupAC,SLOT(clkcSelection(int, int)));
	QObject::connect(qtw_pNH,SIGNAL(cellClicked(int, int)),_tupNH,SLOT(clkcSelection(int, int)));
	QObject::connect(qtw_pNNT,SIGNAL(cellClicked(int, int)),_tupNNT,SLOT(clkcSelection(int, int)));
	QObject::connect(qtw_pPNT,SIGNAL(cellClicked(int, int)),_tupPNT,SLOT(clkcSelection(int, int)));
	QObject::connect(qtw_pRT,SIGNAL(cellClicked(int, int)),_tupRT,SLOT(clkcSelection(int, int)));

	QObject::connect(tabWidget,SIGNAL(currentChanged(int)),this, SLOT(tabChanged(int)));

	QObject::connect(qtwActiveHosts->horizontalHeader(),SIGNAL(sectionClicked(int)),_tuActiveHosts, SLOT(clktbHeader(int)));
	QObject::connect(qtwActiveClients->horizontalHeader(),SIGNAL(sectionClicked(int)),_tuActiveClients, SLOT(clktbHeader(int)));
	QObject::connect(qtwProducerRuns->horizontalHeader(),SIGNAL(sectionClicked(int)),_tuProducerRuns, SLOT(clktbHeader(int)));
	QObject::connect(qtwProducerNTuples->horizontalHeader(),SIGNAL(sectionClicked(int)),_tuProducerNTuples, SLOT(clktbHeader(int)));
	QObject::connect(qtwServActClients->horizontalHeader(),SIGNAL(sectionClicked(int)),_tuServActClients, SLOT(clktbHeader(int)));
	QObject::connect(qtwDBServActClients->horizontalHeader(),SIGNAL(sectionClicked(int)),_tuDBServActClients, SLOT(clktbHeader(int)));
	QObject::connect(qtwDiskUsage->horizontalHeader(),SIGNAL(sectionClicked(int)),_tuDiskUsage, SLOT(clktbHeader(int)));
	QObject::connect(qtwVrdLog->horizontalHeader(),SIGNAL(sectionClicked(int)),_tuVrdLog,SLOT(clktbHeader(int)));
	QObject::connect(qtwOtherLog->horizontalHeader(),SIGNAL(sectionClicked(int)),_tuOtherLog,SLOT(clktbHeader(int)));

	QObject::connect(qtw_sNC->horizontalHeader(),SIGNAL(sectionClicked(int)),_tusNC,SLOT(clktbHeader(int)));
	QObject::connect(qtw_sNH->horizontalHeader(),SIGNAL(sectionClicked(int)),_tusNH,SLOT(clktbHeader(int)));
	QObject::connect(qtw_sNK->horizontalHeader(),SIGNAL(sectionClicked(int)),_tusNK,SLOT(clktbHeader(int)));
	QObject::connect(qtw_sSE->horizontalHeader(),SIGNAL(sectionClicked(int)),_tusSE,SLOT(clktbHeader(int)));
	QObject::connect(qtw_pNC->horizontalHeader(),SIGNAL(sectionClicked(int)),_tupNC,SLOT(clktbHeader(int)));
	QObject::connect(qtw_pAC->horizontalHeader(),SIGNAL(sectionClicked(int)),_tupAC,SLOT(clktbHeader(int)));
	QObject::connect(qtw_pNH->horizontalHeader(),SIGNAL(sectionClicked(int)),_tupNH,SLOT(clktbHeader(int)));
	QObject::connect(qtw_pNNT->horizontalHeader(),SIGNAL(sectionClicked(int)),_tupNNT,SLOT(clktbHeader(int)));
	QObject::connect(qtw_pPNT->horizontalHeader(),SIGNAL(sectionClicked(int)),_tupPNT,SLOT(clktbHeader(int)));
	QObject::connect(qtw_pRT->horizontalHeader(),SIGNAL(sectionClicked(int)),_tupRT,SLOT(clktbHeader(int)));

	QMetaObject::connectSlotsByName(MainWindow);
}

void ui::setDataRequested() { _dataRequested = time(0); }

void ui::kickev(uiEvent::actType type, srviface *iface)
{
	uiEvent *pEv = new uiEvent(type,QEvent::User,iface);
	QApplication::postEvent(reinterpret_cast<QObject *>(this),pEv);
}

void ui::kickev(uistring *stream)
{
	uiEvent *pEv = new uiEvent(uiEvent::ACT_MYLOGGING,QEvent::User,stream);
	QApplication::postEvent(reinterpret_cast<QObject *>(this),pEv);
}

void ui::icon(const char *tab_label)
{
	static const char *_opened_icons[] = { ":/rc/obg.png", ":/rc/oby.png", ":/rc/obr.png" };
	static const char *_closed_icons[] = { ":/rc/cbg.png", ":/rc/cby.png", ":/rc/cbr.png" };

	if (_tabs.find(tab_label) != _tabs.end())
	{
		int tabid = _tabs[tab_label];
		int iconid = _icon_state[tab_label];

		QIcon tabicon;
		if (tabWidget->currentIndex() == tabid)
			tabicon = QIcon(_opened_icons[iconid]);
		else
			tabicon = QIcon(_closed_icons[iconid]);
		tabWidget->setTabIcon(tabid,tabicon);
	}
}

void ui::icon(const char *tab_label, uiIconState state, bool forced)
{
	if (_icon_state.find(tab_label) != _icon_state.end())
	{
		if (!forced)
			if (state <= _icon_state[tab_label])
				return;
		_icon_state[tab_label] = state;
	}
}

const QColor ui::color(colors id)
{
	const QColor (*color_tab[5])() = {
			&ui::colUnk, &ui::colOK, &ui::colNormal, &ui::colWarning, &ui::colAlarm
	};
	return color_tab[id]();
}

tabUtils *ui::getVisibleTab() const
{
	for (map<uistring, tabUtils *>::const_iterator it=_map_forms.begin();
		 it != _map_forms.end();
		 ++it)
	{
		tabUtils *tabu = it->second;
		if ((*tabu)->isVisible() && (*tabu)->hasFocus())
			return tabu;
	}
	return 0;
}

tabUtils *ui::getVisibleCtrlTab() const
{
	for (map<uistring, pair<tabUtils *, int> >::const_iterator it=_map_ctrl_forms.begin();
		 it != _map_ctrl_forms.end();
		 ++it)
	{
		tabUtils *tabu = it->second.first;
		if ((*tabu)->isVisible())
			return tabu;
	}
	return 0;
}

const char *ui::getVisibleCtrlTabName(tabUtils *tabu) const
{
	for (map<uistring, pair<tabUtils *, int> >::const_iterator it=_map_ctrl_forms.begin();
		 it != _map_ctrl_forms.end();
		 ++it)
	{
		if (tabu == it->second.first)
			return it->first.c_str();
	}
	return 0;
}

bool ui::event(QEvent *ev)
{
	uiEvent *eviface = reinterpret_cast<uiEvent *>(ev);
	srviface *iface = reinterpret_cast<srviface *>(eviface->option());

	// Get filter setup only once for the whole event
	_shSelFilt >> _shSelFilt._cash;

	// -- event action --

	if (eviface->action() == uiEvent::ACT_UPDSTATUSBAR)
	{
		stringstream s;
		s << boost::format("Data requested [sec ago]: %d") %(time(0)-_dataRequested);
		statusBar->showMessage(uistring(s.str()).q_str());
		return true;
	}

	// -- event action --

	else if (eviface->action() == uiEvent::ACT_MYLOGGING)
	{
		uistring *str = reinterpret_cast<uistring *>(eviface->option());
		if (str) {
			if (!actLogToCout->isChecked()) {
				(*_tuMyLog)->moveCursor(QTextCursor::End);
				(*_tuMyLog)->insertPlainText(str->q_str());
				(*_tuMyLog)->moveCursor(QTextCursor::End);
			}
			else
				cout << *str;
			delete str;
		}
		else if (!actLogToCout->isChecked()) {
			(*_tuMyLog)->clear();
			(*_tuMyLog)->moveCursor(QTextCursor::End);
		}
		return true;
	}

	// check lost corba connection
	if (iface->fails())
	{
		icon("Service",ui::ICONST_ALARM);
		uistring server = iface->vno();
		vector<int> servers = _tuServActClients->findRows(server.q_str(),TCOLSDB_Serv);
		vector<int> dbservers = _tuDBServActClients->findRows(server.q_str(),TCOLSDB_Serv);
		if (servers.size()) {
			_tuServActClients->setColor(ui::color(ui::CLR_ALARM),servers[0]);
			(*_tuServActClients)->update();
		}
		if (dbservers.size()) {
			_tuDBServActClients->setColor(ui::color(ui::CLR_ALARM),dbservers[0]);
			(*_tuDBServActClients)->update();
		}
		if (_shSelFilt.inserver(server)) {
			tabWidget->setCurrentIndex(_tabs["Service"]);
			icon("Service");
			return true;
		}
		else
			icon("Service");
	}

	// -- event action --

	if (eviface->action() == uiEvent::ACT_DATACHANGED)
	{
		treeUtils::clrSelection();
		tabUtils::clrSelection();

		TreeItem *qserver;
		int servid = _map_serv_nodes.n_key(iface->server().c_str());
		if (servid < 0)
		{
			// First TreeItem appearence
			qserver = _tuTreeWidget->setItem(iface->server(),TRCOL_Name,ITYPE_SERVER,iface);
			treeWidget->addTopLevelItem(qserver);
			_map_serv_nodes.ins(iface->server().c_str(),qserver,0);

			// Not the same for the Control view
			TreeItem *qserver2 = _tuTreeWidget2->setItem(iface->server(),TRCOL_Name,ITYPE_SERVER,iface);
			treeWidget2->addTopLevelItem(qserver2);
			qserver2->setExpanded(true);
			TreeItem *serv2 = _tuTreeWidget2->setItem("Server",TRCOL_Name,ITYPE_HOST,iface);
			qserver2->insertChild(0,serv2);
			TreeItem *form2 = _tuTreeWidget2->setItem("Nominal Client",TRCOL_Name,ITYPE_RUN,iface);
			serv2->insertChild(0,form2);
			form2 = _tuTreeWidget2->setItem("Nominal Host",TRCOL_Name,ITYPE_RUN,iface);
			serv2->insertChild(0,form2);
			form2 = _tuTreeWidget2->setItem("Nominal Killer",TRCOL_Name,ITYPE_RUN,iface);
			serv2->insertChild(0,form2);
			form2 = _tuTreeWidget2->setItem("Set Environment",TRCOL_Name,ITYPE_RUN,iface);
			serv2->insertChild(0,form2);
			TreeItem *prod2 = _tuTreeWidget2->setItem("Producer",TRCOL_Name,ITYPE_HOST,iface);
			qserver2->insertChild(0,prod2);
			form2 = _tuTreeWidget2->setItem("Nominal Client",TRCOL_Name,ITYPE_RUN,iface);
			prod2->insertChild(0,form2);
			form2 = _tuTreeWidget2->setItem("Active Client",TRCOL_Name,ITYPE_RUN,iface);
			prod2->insertChild(0,form2);
			form2 = _tuTreeWidget2->setItem("Nominal Host",TRCOL_Name,ITYPE_RUN,iface);
			prod2->insertChild(0,form2);
			form2 = _tuTreeWidget2->setItem("Nominal NTuple",TRCOL_Name,ITYPE_RUN,iface);
			prod2->insertChild(0,form2);
			form2 = _tuTreeWidget2->setItem("Produced NTuple",TRCOL_Name,ITYPE_RUN,iface);
			prod2->insertChild(0,form2);
			form2 = _tuTreeWidget2->setItem("Run Table",TRCOL_Name,ITYPE_RUN,iface);
			prod2->insertChild(0,form2);
		}
		else
			// TreeItem has been already appeared
			qserver = _map_serv_nodes.get(servid);

		// Tree view
		_tuTreeWidget->rmChildren(qserver);
		(*_tuTreeWidget)->sortItems(0,Qt::AscendingOrder);
		// key string represents: /host[/run][/uid]
		map<string, TreeItem*> map_tree;
		for (unsigned i=0; i<iface->res()->length(); i++) {
			const DPS::Producer::RunEvInfo RunEvInfo = iface->res()[i];

			string sHost;
			int runid = iface->findAcsByRun(RunEvInfo.Run);
			if (runid >= 0) sHost = iface->acs()[runid].id.HostName;
			  else          sHost = string(RunEvInfo.cinfo.HostName);
			if (sHost.length() > 1)
			{
				int hostid = iface->findAhsByHost(sHost);
				if (hostid >= 0)
					sHost = string(iface->ahs()[hostid].HostName);
			}
			else
				sHost = "unknown host";
			TreeItem *qah = srviface::find(map_tree,sHost);
			if (qah == srviface::found_nothing(map_tree)) {
				qah = _tuTreeWidget->setItem(sHost,TRCOL_Name,ITYPE_HOST,iface);
				qserver->insertChild(0,qah);
				map_tree.insert(pair<string, TreeItem*>(sHost,qah));
			}
			stringstream s;

			s << boost::format("%d") %RunEvInfo.Run;
			string skRun = sHost + "/" + s.str();
			TreeItem *qrun = srviface::find(map_tree,skRun);
			if (qrun == srviface::found_nothing(map_tree)) {
				qrun = _tuTreeWidget->setItem(s.str(),TRCOL_Name,ITYPE_RUN,iface);
				qah->insertChild(0,qrun);
				map_tree.insert(pair<string, TreeItem*>(skRun,qrun));
			}
			s.str("");
			s << boost::format("%d") %RunEvInfo.uid;
			string skUid = skRun + "/" + s.str();
			TreeItem *quid = srviface::find(map_tree,skUid);
			if (quid == srviface::found_nothing(map_tree)) {
				quid = _tuTreeWidget->setItem(s.str(),TRCOL_Name,ITYPE_UID,iface);
				qrun->insertChild(0,quid);
				map_tree.insert(pair<string, TreeItem*>(skUid,quid));
			}
		}
		for (int i=0,len=iface->dsts()->length(); i<len; i++)
		{
			const DPS::Producer::DST Dst = iface->dsts()[i];
			vector<int> runids = iface->findRunEvInfoByRun(Dst.Run);
			if (runids.size())
				continue;

			string sHost;
			CharStringToken st(Dst.Name,":");
			int hostid = iface->findAhsByHost(string(st.token(0)));
			if (hostid >= 0)
				sHost = string(iface->ahs()[hostid].HostName);
			else
				sHost = "unknown host";
			TreeItem *qah = srviface::find(map_tree,sHost);
			if (qah == srviface::found_nothing(map_tree)) {
				qah = _tuTreeWidget->setItem(sHost,TRCOL_Name,ITYPE_HOST,iface);
				qserver->insertChild(0,qah);
				map_tree.insert(pair<string, TreeItem*>(sHost,qah));
			}
			stringstream s;

			s << boost::format("%d") %Dst.Run;
			string skRun = sHost + "/" + s.str();
			TreeItem *qrun = srviface::find(map_tree,skRun);
			if (qrun == srviface::found_nothing(map_tree)) {
				qrun = _tuTreeWidget->setItem(s.str(),TRCOL_Name,ITYPE_RUN,iface);
				qah->insertChild(0,qrun);
				map_tree.insert(pair<string, TreeItem*>(skRun,qrun));
			}
		}
		if (actActiveHosts->isChecked() == false)
			for (int i=0,len=iface->ahs()->length(); i<len; i++) {
				const DPS::Client::ActiveHost &ActiveHost = iface->ahs()[i];
				string sHost(ActiveHost.HostName);
				TreeItem *qah = srviface::find(map_tree,sHost);
				if (qah == srviface::found_nothing(map_tree)) {
					qah = _tuTreeWidget->setItem(sHost,TRCOL_Name,ITYPE_HOST,iface);
					qserver->insertChild(0,qah);
					map_tree.insert(pair<string, TreeItem*>(sHost,qah));
				}
			}
		qserver->sortChildren(0,Qt::AscendingOrder);

		// Active Hosts view
		bool sort_state = _tuActiveHosts->sort(false);
		_tuActiveHosts->rmRows(iface->vno(),TCOLAH_Serv);
		int Total = 0;
		for (int i=0,len=iface->ahs()->length(); i<len; i++)
		{
			const DPS::Client::ActiveHost &ActiveHost = iface->ahs()[i];
			string host(ActiveHost.HostName);
			vector<int> vrun = iface->findRunEvInfoByHost(host);
			for (unsigned i=0; i<vrun.size(); i++)
			{
				const DPS::Producer::RunEvInfo RunEvInfo = iface->res()[vrun[i]];

				int tEvents = RunEvInfo.LastEvent - RunEvInfo.FirstEvent;
				if (tEvents > 0)
					Total += tEvents;
			}
		}
		for (int i=0,len=iface->ahs()->length(); i<len; i++)
		{
			const DPS::Client::ActiveHost &ActiveHost = iface->ahs()[i];

			string host(ActiveHost.HostName);
			vector<int> acs = iface->findAcsByHost(host);
			if ((actActiveHosts->isChecked() == true && acs.size()) ||
				 actActiveHosts->isChecked() == false)
			{
				string svno = iface->vno();
				if (!_shSelFilt.inserver(svno) || !_shSelFilt.inhost(host))
					continue;
				int pos = _tuActiveHosts->addRow();
				_tuActiveHosts->setItem(iface->vno(),pos,TCOLAH_Serv,iface);
				_tuActiveHosts->setItem(host,pos,TCOLAH_HostName,iface);
				_tuActiveHosts->setItem(static_cast<uint>(acs.size()),pos,TCOLAH_Clients,iface);
				_tuActiveHosts->setItem(ActiveHost.ClientsRunning,pos,TCOLAH_RunningPr,iface);
				_tuActiveHosts->setItem(ActiveHost.ClientsAllowed,pos,TCOLAH_AllowedPr,iface);
				_tuActiveHosts->setItem(ActiveHost.Clock,pos,TCOLAH_Clock,iface);
				_tuActiveHosts->setItem(ActiveHost.ClientsProcessed,pos,TCOLAH_CliProcessed,iface);
				_tuActiveHosts->setItem(ActiveHost.ClientsFailed,pos,TCOLAH_CliFailed,iface);
				int NTuples = 0;
				int EvTags = 0;
				vector<int> vdst = iface->findDstByHost(host);
				for (unsigned i=0; i<vdst.size(); i++)
				{
					const DPS::Producer::DST Dst = iface->dsts()[vdst[i]];
					if (Dst.Type == DPS::Producer::Ntuple ||
						Dst.Type == DPS::Producer::RootFile ||
						Dst.Type == DPS::Producer::RawFile)
							NTuples++;
					else
						// Dst.Type == DPS::Producer::EventTag)
						EvTags++;
				}
				_tuActiveHosts->setItem(NTuples,pos,TCOLAH_NTuples,iface);
				_tuActiveHosts->setItem(EvTags,pos,TCOLAH_EventTags,iface);
				int Warnings = 0;
				int Errors = 0;
				int Events = 0;
				float CPUTimeSpent = 0;
				float TimeSpent = 0;
				vector<int> vrun = iface->findRunEvInfoByHost(host);
				for (unsigned i=0; i<vrun.size(); i++)
				{
					const DPS::Producer::RunEvInfo RunEvInfo = iface->res()[vrun[i]];

					if (RunEvInfo.Status == DPS::Producer::Finished ||
						RunEvInfo.Status == DPS::Producer::Processing)
					{
						Warnings += RunEvInfo.cinfo.ErrorsFound;
						Errors += RunEvInfo.cinfo.CriticalErrorsFound;
						int tEvents = RunEvInfo.LastEvent - RunEvInfo.FirstEvent;
						if (RunEvInfo.cinfo.EventsProcessed > tEvents)
							Events += tEvents;
						else
							Events += RunEvInfo.cinfo.EventsProcessed;
						CPUTimeSpent += RunEvInfo.cinfo.CPUTimeSpent;
						TimeSpent += RunEvInfo.cinfo.TimeSpent;
					}
				}
				_tuActiveHosts->setItem(Events,pos,TCOLAH_Events,iface);
				float percTotal = 100.0 * static_cast<float>(Events) / Total;
				_tuActiveHosts->setItem(percTotal,pos,TCOLAH_Total,iface);
				_tuActiveHosts->setItem(Warnings,pos,TCOLAH_Warnings,iface);
				_tuActiveHosts->setItem(Errors,pos,TCOLAH_Errors,iface);
				int ClientsRunning = static_cast<int>(ActiveHost.ClientsRunning);
				if (ClientsRunning == 0)
					ClientsRunning++;
				float CPUEvent = CPUTimeSpent / ((Events+1) * ClientsRunning);
				_tuActiveHosts->setItem(CPUEvent,pos,TCOLAH_CPUEvent,iface);
				float Efficiensy = 0;
				if (TimeSpent)
				{
					int hostid = iface->findNhsByHost(host);
					if (hostid >= 0)
						Efficiensy = CPUTimeSpent * ClientsRunning /
									 ((iface->nhs()[hostid].CPUNumber+0.01) * TimeSpent);
				}
				_tuActiveHosts->setItem(Efficiensy,pos,TCOLAH_Efficiency,iface);
				_tuActiveHosts->setItem(iface->strHostStatus(ActiveHost.Status),pos,TCOLAH_Status,iface);
				switch (ActiveHost.Status)
				{
				case DPS::Client::NoResponse:
					_tuActiveHosts->setColor(color(CLR_ALARM),pos);
					icon("Producer processes",ICONST_ALARM);
					break;
				case DPS::Client::LastClientFailed:
				case DPS::Client::InActive:
					_tuActiveHosts->setColor(color(CLR_WARNING),pos);
					icon("Producer processes",ICONST_WARNING);
					break;
				default:
					_tuActiveHosts->setColor(color(CLR_RUNNING),pos);
					break;
				}
			}
		}
		if (sort_state) _tuActiveHosts->sort(sort_state);

		// Active Clients view
acview:
		sort_state = _tuActiveClients->sort(false);
		_tuActiveClients->rmRows(iface->vno(),TCOL_Serv);
		int resfinin = 2;
		for (int i=0,len=iface->acs()->length(); i<len; i++)
		{
			const DPS::Client::ActiveClient &ActiveClient = iface->acs()[i];

			string sActiveHost(ActiveClient.id.HostName);
			for (int i2=0,len2=ActiveClient.ars.length(); i2<len2; i2++)
			{
				DPS::Producer::RunEvInfo RunEvInfo;

				string svno = iface->vno();
				if (!_shSelFilt.inserver(svno) ||
					!_shSelFilt.inhost(sActiveHost) || _shSelFilt.acBadFilt(ActiveClient))
						continue;
				int runid = iface->findRunEvInfoByUid(ActiveClient.id.uid);
				long Run = -1;
				int evLeft = 0;
				if (runid >= 0) {
					RunEvInfo = iface->res()[runid];
					Run = RunEvInfo.Run;
					if (_shSelFilt.runBadFilt(RunEvInfo))
							continue;
				}
				else if (_shSelFilt.runUseFilt())
					continue;
				int pos = _tuActiveClients->addRow();
				_tuActiveClients->setItem(iface->vno(),pos,TCOL_Serv,iface);
				_tuActiveClients->setItem(static_cast<uint>(Run),pos,TCOL_Run,iface);
				_tuActiveClients->setItem(ActiveClient.id.uid,pos,TCOL_Id,iface);
				_tuActiveClients->setItem(sActiveHost,pos,TCOL_HostName,iface);
				_tuActiveClients->setItem(ActiveClient.id.Mips,pos,TCOL_Mips,iface);
				_tuActiveClients->setItem(ActiveClient.id.threads,pos,TCOL_Threads,iface);
				_tuActiveClients->setItem(ActiveClient.id.pid,pos,TCOL_ProcId,iface);
				string st = timeString(static_cast<time_t>(ActiveClient.Start));
				_tuActiveClients->setItem(st,pos,TCOL_StartTime,iface);
				stringstream s;
				s << boost::format("%d sec ago") %(time(0)-ActiveClient.LastUpdate);
				_tuActiveClients->setItem(s.str(),pos,TCOL_LastUpdTime,iface);
				_tuActiveClients->setItem(ActiveClient.TimeOut,pos,TCOL_Timeouut,iface);
				if (runid >= 0)
					if (RunEvInfo.Status == DPS::Producer::Processing)
						evLeft = RunEvInfo.LastEvent - RunEvInfo.cinfo.LastEventProcessed;
				_tuActiveClients->setItem(evLeft,pos,TCOL_EvLeft,iface);
				time_t totalev = RunEvInfo.LastEvent-RunEvInfo.FirstEvent+1;
				time_t evleft = RunEvInfo.LastEvent-RunEvInfo.cinfo.LastEventProcessed;
				time_t evdone = 1 + totalev - evleft;
				time_t finishin = 0;
				if (evdone)
					finishin = (ActiveClient.LastUpdate-ActiveClient.Start)*evleft/evdone;
				st = gmtimeString(finishin,false,_resFinishIn);
				s.str("");
				s << boost::format("%d") %(finishin/3600);
				int hl = s.str().size();
				if (hl > resfinin) resfinin = hl;
				_tuActiveClients->setItem(st,pos,TCOL_FinishIn,iface);
				_tuActiveClients->setItem(iface->strClientStatus(ActiveClient.Status),pos,TCOL_Status,iface);
				string sf;
				if (runid >= 0) {
					int dstid = iface->findLastDstByRun(RunEvInfo.Run);
					if (dstid >= 0) {
						CharStringToken sft(iface->dsts()[dstid].Name);
						sft.strtok("/");
						for (int i=sft.tokens()-1; i>=0; i++)
							if (strlen(sft.token(i)) > 1) {
								sf = sft.token(i);
								break;
							}
					}
				}
				_tuActiveClients->setItem(sf,pos,TCOL_FileName,iface);
				switch (ActiveClient.Status)
				{
				case DPS::Client::Active:
					if (sf.length() > 1)
						_tuActiveClients->setColor(color(CLR_RUNNING),pos);
					else {
						_tuActiveClients->setColor(color(CLR_WARNING),pos);
						icon("Producer processes",ICONST_WARNING);
					}
					break;
				case DPS::Client::Registered:
				case DPS::Client::Submitted:
					_tuActiveClients->setColor(color(CLR_WARNING),pos);
					icon("Producer processes",ICONST_WARNING);
					break;
				case DPS::Client::Killed:
				case DPS::Client::Lost:
				case DPS::Client::TimeOut:
					_tuActiveClients->setColor(color(CLR_ALARM),pos);
					icon("Producer processes",ICONST_ALARM);
					break;
				default:
					_tuActiveClients->setColor(color(CLR_UNK),pos);
					break;
				}
			}
		}
		if (resfinin > 4)
			resfinin = 4;
		if (resfinin != _resFinishIn) {
				_resFinishIn = resfinin;
				goto acview;
		}
		if (sort_state) _tuActiveClients->sort(sort_state);
		icon("Producer processes");

		// Producer runs view
		sort_state = _tuProducerRuns->sort(false);
		_tuProducerRuns->rmRows(iface->vno(),TCOLPR_Serv);
		for (unsigned i=0; i<iface->res()->length(); i++)
		{
			const DPS::Producer::RunEvInfo RunEvInfo = iface->res()[i];

			string svno = iface->vno();
			string sRunHost(RunEvInfo.cinfo.HostName);
			if (!_shSelFilt.inserver(svno) || !_shSelFilt.inhost(sRunHost) || _shSelFilt.runBadFilt(RunEvInfo))
				continue;
			if (_shSelFilt.acUseFilt()) {
				int acid = iface->findAcsByUid(RunEvInfo.uid);
				if (acid >= 0) {
					const DPS::Client::ActiveClient &ActiveClient = iface->acs()[acid];
					if (_shSelFilt.acBadFilt(ActiveClient))
						continue;
				}
				else
					continue;
			}
			int pos = _tuProducerRuns->addRow();
			_tuProducerRuns->setItem(iface->vno(),pos,TCOLPR_Serv,iface);
			_tuProducerRuns->setItem(RunEvInfo.Run,pos,TCOLPR_Run,iface);
			_tuProducerRuns->setItem(RunEvInfo.uid,pos,TCOLPR_Uid,iface);
			string st = timeString(static_cast<time_t>(RunEvInfo.SubmitTime));
			_tuProducerRuns->setItem(st,pos,TCOLPR_Time,iface);
			_tuProducerRuns->setItem(RunEvInfo.FirstEvent,pos,TCOLPR_FirstEv,iface);
			_tuProducerRuns->setItem(RunEvInfo.LastEvent,pos,TCOLPR_LastEv,iface);
			_tuProducerRuns->setItem(RunEvInfo.Priority,pos,TCOLPR_Prior,iface);
			_tuProducerRuns->setItem(iface->strRunStatus(RunEvInfo.History),pos,TCOLPR_Hist,iface);
			_tuProducerRuns->setItem(RunEvInfo.CounterFail,pos,TCOLPR_Fails,iface);
			_tuProducerRuns->setItem(RunEvInfo.DataMC,pos,TCOLPR_DataMC,iface);
			_tuProducerRuns->setItem(sRunHost,pos,TCOLPR_Host,iface);
			_tuProducerRuns->setItem(iface->strRunStatus(RunEvInfo.Status),pos,TCOLPR_Status,iface);
			switch (RunEvInfo.Status)
			{
			case DPS::Producer::Processing:
			case DPS::Producer::Allocated:
				if (iface->findAcsByRun(RunEvInfo.Run) >= 0) _tuProducerRuns->setColor(color(CLR_RUNNING),pos);
				  else goto iswarn;
				break;
			case DPS::Producer::Failed:
			case DPS::Producer::Canceled:
iswarn:			_tuProducerRuns->setColor(color(CLR_WARNING),pos);
				icon("Producer i/o",ICONST_WARNING);
				break;
			case DPS::Producer::Finished:
				_tuProducerRuns->setColor(color(CLR_FINISHED),pos);
				break;
			default:
				_tuProducerRuns->setColor(color(CLR_UNK),pos);
				break;
			}
		}
		if (sort_state) _tuProducerRuns->sort(sort_state);

		// Producer NTuples view
		sort_state = _tuProducerNTuples->sort(false);
		_tuProducerNTuples->rmRows(iface->vno(),TCOLPNT_Serv);
		for (int i=0,len=iface->dsts()->length(); i<len; i++)
		{
			const DPS::Producer::DST &Dst = iface->dsts()[i];

			string svno = iface->vno();
			if (!_shSelFilt.inserver(svno)) continue;

			vector<int> runids = iface->findRunEvInfoByRun(Dst.Run);
			int sz_runids = static_cast<int>(runids.size());
			bool show_dst = false;
			if (sz_runids==0 && !_shSelFilt.runUseFilt()) show_dst = true;
			for (int j=0; j<sz_runids && !show_dst; j++) {
				int runid = runids[j];
				if (runid >= 0) {
					const DPS::Producer::RunEvInfo RunEvInfo = iface->res()[runid];
					string sRunHost(RunEvInfo.cinfo.HostName);
					if (!_shSelFilt.inhost(sRunHost) || _shSelFilt.runBadFilt(RunEvInfo))
						continue;
					if (_shSelFilt.acUseFilt()) {
						int acid = iface->findAcsByUid(RunEvInfo.uid);
						if (acid >= 0) {
							const DPS::Client::ActiveClient &ActiveClient = iface->acs()[acid];
							if (_shSelFilt.acBadFilt(ActiveClient))
								continue;
						}
						else continue;
					}
				}
				else if (_shSelFilt.runUseFilt())
					continue;
				show_dst = true;
			}
			if (!show_dst) continue;

			int pos = _tuProducerNTuples->addRow();
			_tuProducerNTuples->setItem(iface->vno(),pos,TCOLPNT_Serv,iface);
			_tuProducerNTuples->setItem(Dst.Run,pos,TCOLPNT_Run,iface);
			string st = timeString(static_cast<time_t>(Dst.Insert));
			_tuProducerNTuples->setItem(st,pos,TCOLPNT_Time,iface);
			_tuProducerNTuples->setItem(Dst.FirstEvent,pos,TCOLPNT_FirstEv,iface);
			_tuProducerNTuples->setItem(Dst.LastEvent,pos,TCOLPNT_LastEv,iface);
			_tuProducerNTuples->setItem(string(Dst.Name),pos,TCOLPNT_Name,iface);
			_tuProducerNTuples->setItem(Dst.crc,pos,TCOLPNT_CRC,iface);
			_tuProducerNTuples->setItem(Dst.size,pos,TCOLPNT_Size,iface);
			_tuProducerNTuples->setItem(iface->strDSTStatus(Dst.Status),pos,TCOLPNT_Status,iface);
			if (sz_runids) {
				switch (Dst.Status) {
				case DPS::Producer::InProgress:
					_tuProducerNTuples->setColor(color(CLR_RUNNING),pos);
					break;
				case DPS::Producer::Failure:
					_tuProducerNTuples->setColor(color(CLR_WARNING),pos);
					icon("Producer i/o",ICONST_WARNING);
					break;
				default:
					_tuProducerNTuples->setColor(color(CLR_FINISHED),pos);
					break;
				}
			}
			else {
				_tuProducerNTuples->setColor(color(CLR_WARNING),pos);
				icon("Producer i/o",ICONST_WARNING);
			}
		}
		if (sort_state) _tuProducerNTuples->sort(sort_state);
		icon("Producer i/o");

		// Server Active Clients view
		sort_state = _tuServActClients->sort(false);
		_tuServActClients->rmRows(iface->vno(),TCOLSDB_Serv);
		_tuServActClients->fillDBServActClients(iface,iface->s_acs());
		if (sort_state) _tuServActClients->sort(sort_state);
		// DBServer Active Clients view
		sort_state = _tuDBServActClients->sort(false);
		_tuDBServActClients->rmRows(iface->vno(),TCOLSDB_Serv);
		_tuDBServActClients->fillDBServActClients(iface,iface->dbs_acs());
		if (sort_state) _tuDBServActClients->sort(sort_state);
		// color check
		if (iface->dbfails()) {
			uistring server = iface->vno();
			vector<int> servers = _tuDBServActClients->findRows(server.q_str(),TCOLSDB_Serv);
			if (servers.size()) {
				if (iface->pinged()) {
					_tuDBServActClients->setColor(ui::color(ui::CLR_ALARM),servers[0]);
					icon("Service",ICONST_ALARM);
				}
				else {
					_tuDBServActClients->setColor(ui::color(ui::CLR_WARNING),servers[0]);
					icon("Service",ICONST_WARNING);
				}
			}
		}
		// Disk Usage view
		sort_state = _tuDiskUsage->sort(false);
		_tuDiskUsage->rmRows(iface->vno(),TCOLDU_Serv);
		set<string> dict;
		for (int i=0,len=iface->dsts()->length(); i<len; i++)
		{
			const DPS::Producer::DST &Dst = iface->dsts()[i];
			if (Dst.Type == DPS::Producer::EventTag) continue;

			CharStringToken fstoken(string(Dst.Name).c_str());
			fstoken.strtok("/");
			if (fstoken.tokens() < 2) {
				// Write to log window in the future
				continue;
			}
			string fs;
			if (fstoken.token(1)[0] == 'f') fs = string("/") + fstoken.token(1);
			  else							 fs = string(fstoken.token(0)) + "/" + fstoken.token(1);
			if (dict.find(fs) == dict.end()) dict.insert(fs);
			  else							 continue;

			int pos = _tuDiskUsage->addRow();
			_tuDiskUsage->setItem(iface->vno(),pos,TCOLDU_Serv,iface);
			_tuDiskUsage->setItem(fs,pos,TCOLDU_FileSystem,iface);
			_tuDiskUsage->setItem(Dst.TotalSpace,pos,TCOLDU_Total,iface);
			_tuDiskUsage->setItem(Dst.FreeSpace,pos,TCOLDU_Free,iface);
			int perc = static_cast<int>((static_cast<float>(Dst.FreeSpace)/(Dst.TotalSpace+2)*100));
			if (Dst.FreeSpace < 0 || Dst.TotalSpace < 0)
				perc = 0 - abs(perc);
			_tuDiskUsage->setItem(perc,pos,TCOLDU_PercFree,iface);
			if (Dst.FreeSpace >= 0 && Dst.FreeSpace < 2000 && perc < 10) {
				_tuDiskUsage->setColor(color(CLR_ALARM),pos);
				icon("Service",ICONST_ALARM);
			}
			else if ((Dst.TotalSpace < 0 || Dst.FreeSpace < 0 || Dst.FreeSpace < 10000) &&
					 strstr(fs.c_str(),"/castor") == 0 && strstr(fs.c_str(),"/eos") == 0) {
				_tuDiskUsage->setColor(color(CLR_WARNING),pos);
				icon("Service",ICONST_WARNING);
			}
			else
				_tuDiskUsage->setColor(color(CLR_RUNNING),pos);
		}
		for (int i=0,len=iface->vec_s_acs().size(); i<len; i++)
		{
			DPS::Server::DB_var pdb = iface->vec_s_acs()[i];

			int pos = _tuDiskUsage->addRow();
			_tuDiskUsage->setItem(iface->vno(),pos,TCOLDU_Serv,iface);
			_tuDiskUsage->setItem(string(pdb->fs),pos,TCOLDU_FileSystem,iface);
			_tuDiskUsage->setItem(static_cast<uint>(pdb->dbtotal),pos,TCOLDU_Total,iface);
			_tuDiskUsage->setItem(static_cast<uint>(pdb->dbfree),pos,TCOLDU_Free,iface);
			int perc = static_cast<int>((static_cast<float>(pdb->dbfree)/(pdb->dbtotal+2)*100));
			if (pdb->dbfree < 0 || pdb->dbtotal < 0)
				perc = 0 - abs(perc);
			_tuDiskUsage->setItem(perc,pos,TCOLDU_PercFree,iface);
			_tuDiskUsage->setColor(color(CLR_RUNNING),pos);
		}
		if (sort_state) _tuDiskUsage->sort(sort_state);
		icon("Service");

		// Server::NominalClient view
		sort_state = _tusNC->sort(false);
		_tusNC->filterRows(true);
		_tusNC->rmRows(iface->vno(),TCOLsNC_Serv);
		_tusNC->fillNominalClients(iface,iface->s_ncs());
		if (sort_state) _tusNC->sort(sort_state);

		// Server::NominalHost view
		sort_state = _tusNH->sort(false);
		_tusNH->filterRows(true);
		_tusNH->rmRows(iface->vno(),TCOLsNH_Serv);
		for (int i=0,len=iface->s_nhs()->length(); i<len; i++)
		{
			const DPS::Producer::NominalHost &Nh = iface->s_nhs()[i];

			string sHost(Nh.HostName);
			if (!_shSelFilt.inserver(iface->vno()) || !_shSelFilt.inhost(sHost))
				continue;
			int pos = _tusNH->addRow(false);
			_tusNH->setItem(iface->vno(),pos,TCOLsNH_Serv,iface);
			_tusNH->setItem(sHost,pos,TCOLsNH_HostName,iface);
			_tusNH->setItem(string(Nh.Interface),pos,TCOLsNH_Interface,iface);
			_tusNH->setItem(string(Nh.OS),pos,TCOLsNH_OS,iface);
			stringstream s;
			s << boost::format("%d") %Nh.CPUNumber;
			_tusNH->setItem(s.str(),pos,TCOLsNH_CPUNumb,iface);
			s.str("");
			s << boost::format("%d") %Nh.Memory;
			_tusNH->setItem(s.str(),pos,TCOLsNH_Memory,iface);
			s.str("");
			s << boost::format("%d") %Nh.Clock;
			_tusNH->setItem(s.str(),pos,TCOLsNH_Clock,iface);
			s.str("");
			s << boost::format("%d") %0;
			_tusNH->setItem(s.str(),pos,TCOLsNH_ClientsAllowed,iface);
			_tusNH->setItem("NotFound",pos,TCOLsNH_Status,iface);
		}
		if (sort_state) _tusNH->sort(sort_state);

		// Server::NominalKiller view
		sort_state = _tusNK->sort(false);
		_tusNK->filterRows(true);
		_tusNK->rmRows(iface->vno(),TCOLsNC_Serv);
		_tusNK->fillNominalClients(iface,iface->s_nks());
		if (sort_state) _tusNK->sort(sort_state);

		// Server::SetEnvironment view
		sort_state = _tusSE->sort(false);
		_tusSE->filterRows(true);
		_tusSE->rmRows(iface->vno(),TCOLsSE_Serv);
		for (int i=0,len=iface->env().len(); i<len; i++)
		{
			if (!_shSelFilt.inserver(iface->vno()))
				continue;
			int pos = _tusSE->addRow(false);
			_tusSE->setItem(iface->vno(),pos,TCOLsSE_Serv,iface);
			_tusSE->setItem(iface->env().key(i),pos,TCOLsSE_Var,iface);
			_tusSE->setItem(iface->env().get(i),pos,TCOLsSE_Path,iface);
		}
		if (sort_state) _tusSE->sort(sort_state);

		// Producer::NominalClient view
		sort_state = _tupNC->sort(false);
		_tupNC->filterRows(true);
		_tupNC->rmRows(iface->vno(),TCOLsNC_Serv);
		_tupNC->fillNominalClients(iface,iface->ncs());
		if (sort_state) _tupNC->sort(sort_state);

		// Producer::ActiveClient view
		sort_state = _tupAC->sort(false);
		_tupAC->filterRows(true);
		_tupAC->rmRows(iface->vno(),TCOLpAC_Serv);
		for (int i=0,len=iface->acs()->length(); i<len; i++)
		{
			const DPS::Client::ActiveClient &ActiveClient = iface->acs()[i];

			string sActiveHost(ActiveClient.id.HostName);
			if (!_shSelFilt.inserver(iface->vno()) || !_shSelFilt.inhost(sActiveHost) || _shSelFilt.acBadFilt(ActiveClient))
				continue;
			int runid = iface->findRunEvInfoByUid(ActiveClient.id.uid);
			if (runid >= 0) {
				const DPS::Producer::RunEvInfo RunEvInfo = iface->res()[runid];
				if (_shSelFilt.runBadFilt(RunEvInfo))
					continue;
			}
			else if (_shSelFilt.runUseFilt())
				continue;
			int pos = _tupAC->addRow(false);
			_tupAC->setItem(iface->vno(),pos,TCOLpAC_Serv,iface);
			stringstream s;
			s << boost::format("%d") %ActiveClient.id.uid;
			uistring suid = s.str();
			_tupAC->setItem(suid,pos,TCOLpAC_Uid,iface);
			_tupAC->setItem(sActiveHost,pos,TCOLpAC_HostName,iface);
			s.str("");
			s << boost::format("%d") %(static_cast<int>(ActiveClient.id.Mips));
			_tupAC->setItem(s.str(),pos,TCOLpAC_Mips,iface);
			s.str("");
			s << boost::format("%d") %(static_cast<int>(ActiveClient.id.pid));
			_tupAC->setItem(s.str(),pos,TCOLpAC_Pid,iface);
			_tupAC->setItem(iface->strClientStatus(ActiveClient.Status),pos,TCOLpAC_Status,iface);
			_tupAC->setItem(iface->strClientStatusType(ActiveClient.StatusType),pos,TCOLpAC_Type,iface);
			s.str("");
			s << boost::format("%d") %(ActiveClient.TimeOut);
			_tupAC->setItem(s.str(),pos,TCOLpAC_Timeout,iface);
			if (runid >= 0) {
				const DPS::Producer::RunEvInfo RunEvInfo = iface->res()[runid];
				_tupAC->setItem(string(RunEvInfo.FilePath),pos,TCOLpAC_FilePath,iface);
			}
			else
				_tupAC->setItem("not found",pos,TCOLpAC_FilePath,iface);
		}
		if (sort_state) _tupAC->sort(sort_state);

		// Producer::NominalHost view
		sort_state = _tupNH->sort(false);
		_tupNH->filterRows(true);
		_tupNH->rmRows(iface->vno(),TCOLpNH_Serv);
		for (int i=0,len=iface->nhs()->length(); i<len; i++)
		{
			const DPS::Producer::NominalHost &Nh = iface->nhs()[i];

			string sHost(Nh.HostName);
			if (!_shSelFilt.inserver(iface->vno()) || !_shSelFilt.inhost(sHost))
				continue;
			int pos = _tupNH->addRow(false);
			_tupNH->setItem(iface->vno(),pos,TCOLpNH_Serv,iface);
			_tupNH->setItem(sHost,pos,TCOLpNH_HostName,iface);
			_tupNH->setItem(string(Nh.Interface),pos,TCOLpNH_Interface,iface);
			_tupNH->setItem(string(Nh.OS),pos,TCOLpNH_OS,iface);
			stringstream s;
			s << boost::format("%d") %Nh.CPUNumber;
			_tupNH->setItem(s.str(),pos,TCOLpNH_CPUNumb,iface);
			s.str("");
			s << boost::format("%d") %Nh.Memory;
			_tupNH->setItem(s.str(),pos,TCOLpNH_Memory,iface);
			s.str("");
			s << boost::format("%d") %Nh.Clock;
			_tupNH->setItem(s.str(),pos,TCOLpNH_Clock,iface);
			s.str("");
			s << boost::format("%d") %0;
			int hostid = iface->findAhsByHost(sHost);
			if (hostid >= 0)
			{
				s.str("");
				s << boost::format("%d") %iface->ahs()[hostid].ClientsAllowed;
				_tupNH->setItem(s.str(),pos,TCOLpNH_ClientsAllowed,iface);
				s.str("");
				s << boost::format("%d") %iface->ahs()[hostid].ClientsProcessed;
				_tupNH->setItem(s.str(),pos,TCOLpNH_CliProcessed,iface);
				s.str("");
				s << boost::format("%d") %iface->ahs()[hostid].ClientsFailed;
				_tupNH->setItem(s.str(),pos,TCOLpNH_CliFailed,iface);

				_tupNH->setItem(iface->strHostStatus(iface->ahs()[hostid].Status),pos,TCOLpNH_Status,iface);
			}
		}
		if (sort_state) _tupNH->sort(sort_state);

		// Producer::NominalNTuple view
		sort_state = _tupNNT->sort(false);
		_tupNNT->filterRows(true);
		_tupNNT->rmRows(iface->vno(),TCOLpNNT_Serv);
		for (int i=0,len=iface->dstis()->length(); i<len; i++)
		{
			const DPS::Producer::DSTInfo &Dsti = iface->dstis()[i];

			if (!_shSelFilt.inserver(iface->vno()))
				continue;
			int pos = _tupNNT->addRow(false);
			_tupNNT->setItem(iface->vno(),pos,TCOLpNNT_Serv,iface);
			stringstream s;
			s << boost::format("%d") %Dsti.uid;
			_tupNNT->setItem(s.str(),pos,TCOLpNNT_Uid,iface);
			_tupNNT->setItem(string(Dsti.HostName),pos,TCOLpNNT_HostName,iface);
			_tupNNT->setItem(string(Dsti.OutputDirPath),pos,TCOLpNNT_OutDirPath,iface);
			_tupNNT->setItem(iface->strRunMode(Dsti.Mode),pos,TCOLpNNT_RunMode,iface);
			s.str("");
			s << boost::format("%d") %Dsti.UpdateFreq;
			_tupNNT->setItem(s.str(),pos,TCOLpNNT_UpdFreq,iface);
			_tupNNT->setItem(iface->strDSTType(Dsti.type),pos,TCOLpNNT_DstType,iface);
		}
		if (sort_state) _tupNNT->sort(sort_state);

		// Producer::ProducedNTuple view
		sort_state = _tupPNT->sort(false);
		_tupPNT->filterRows(true);
		_tupPNT->rmRows(iface->vno(),TCOLpNNT_Serv);
		for (int i=0,len=iface->dsts()->length(); i<len; i++)
		{
			const DPS::Producer::DST &Dst = iface->dsts()[i];

			vector<int> runids = iface->findRunEvInfoByRun(Dst.Run);
			int sz_runids = static_cast<int>(runids.size());
			bool show_dst = false;
			if (sz_runids==0 && !_shSelFilt.runUseFilt()) show_dst = true;
			for (int j=0; j<sz_runids && !show_dst; j++) {
				int runid = runids[j];
				if (runid >= 0) {
					const DPS::Producer::RunEvInfo RunEvInfo = iface->res()[runid];
					string sRunHost(RunEvInfo.cinfo.HostName);
					if (!_shSelFilt.inserver(iface->vno()) || !_shSelFilt.inhost(sRunHost) || _shSelFilt.runBadFilt(RunEvInfo))
						continue;
					if (_shSelFilt.acUseFilt()) {
						int acid = iface->findAcsByUid(RunEvInfo.uid);
						if (acid >= 0) {
							const DPS::Client::ActiveClient &ActiveClient = iface->acs()[acid];
							if (_shSelFilt.acBadFilt(ActiveClient))
								continue;
						}
						else continue;
					}
				}
				else if (_shSelFilt.inuse() && _shSelFilt.runUseFilt())
					continue;
				show_dst = true;
			}
			if (!show_dst) continue;

			int pos = _tupPNT->addRow(false);
			_tupPNT->setItem(iface->vno(),pos,TCOLpPNT_Serv,iface);
			stringstream s;
			s << boost::format("%d") %Dst.Run;
			_tupPNT->setItem(s.str(),pos,TCOLpPNT_Run,iface);
			string st = timeString(static_cast<time_t>(Dst.Insert));
			_tupPNT->setItem(st,pos,TCOLpPNT_Time,iface);
			s.str("");
			s << boost::format("%d") %Dst.FirstEvent;
			_tupPNT->setItem(s.str(),pos,TCOLpPNT_FirstEv,iface);
			s.str("");
			s << boost::format("%d") %Dst.LastEvent;
			_tupPNT->setItem(s.str(),pos,TCOLpPNT_LastEv,iface);
			_tupPNT->setItem(string(Dst.Name),pos,TCOLpPNT_Path,iface);
			_tupPNT->setItem(string(Dst.Version),pos,TCOLpPNT_Version,iface);
			s.str("");
			s << boost::format("%d") %Dst.size;
			_tupPNT->setItem(s.str(),pos,TCOLpPNT_Size,iface);
			_tupPNT->setItem(iface->strDSTStatus(Dst.Status),pos,TCOLpPNT_Status,iface);
			_tupPNT->setItem(iface->strDSTType(Dst.Type),pos,TCOLpPNT_Type,iface);
		}
		if (sort_state) _tupPNT->sort(sort_state);

		// Producer::RunTable view
		sort_state = _tupRT->sort(false);
		_tupRT->filterRows(true);
		_tupRT->rmRows(iface->vno(),TCOLpRT_Serv);
		for (unsigned i=0; i<iface->res()->length(); i++)
		{
			const DPS::Producer::RunEvInfo RunEvInfo = iface->res()[i];

			string sRunHost(RunEvInfo.cinfo.HostName);
			if (!_shSelFilt.inserver(iface->vno()) || !_shSelFilt.inhost(sRunHost) || _shSelFilt.runBadFilt(RunEvInfo))
				continue;
			if (_shSelFilt.acUseFilt()) {
				int acid = iface->findAcsByUid(RunEvInfo.uid);
				if (acid >= 0) {
					const DPS::Client::ActiveClient &ActiveClient = iface->acs()[acid];
					if (_shSelFilt.acBadFilt(ActiveClient))
						continue;
				}
				else
					continue;
			}
			int pos = _tupRT->addRow();
			_tupRT->setItem(iface->vno(),pos,TCOLpRT_Serv,iface);
			stringstream s;
			s << boost::format("%d") %RunEvInfo.uid;
			uistring suid = s.str();
			_tupRT->setItem(suid,pos,TCOLpRT_Uid,iface);
			s.str("");
			s << boost::format("%d") %RunEvInfo.cuid;
			_tupRT->setItem(s.str(),pos,TCOLpRT_CUid,iface);
			_tupRT->setItem(sRunHost,pos,TCOLpRT_HostName,iface);
			s.str("");
			s << boost::format("%d") %RunEvInfo.Run;
			_tupRT->setItem(s.str(),pos,TCOLpRT_Run,iface);
			s.str("");
			s << boost::format("%d") %RunEvInfo.FirstEvent;
			_tupRT->setItem(s.str(),pos,TCOLpRT_FirstEv,iface);
			s.str("");
			s << boost::format("%d") %RunEvInfo.LastEvent;
			_tupRT->setItem(s.str(),pos,TCOLpRT_LastEv,iface);
			s.str("");
			s << boost::format("%d") %RunEvInfo.Priority;
			_tupRT->setItem(s.str(),pos,TCOLpRT_Priority,iface);
			_tupRT->setItem(string(RunEvInfo.FilePath),pos,TCOLpRT_FilePath,iface);
			_tupRT->setItem(iface->strRunStatus(RunEvInfo.Status),pos,TCOLpRT_Status,iface);
			_tupRT->setItem(iface->strRunStatus(RunEvInfo.History),pos,TCOLpRT_History,iface);
			int acid = iface->findAcsByUid(RunEvInfo.uid);
			if (acid >= 0)
				_tupRT->setItem(iface->strClientStatusType(iface->acs()[acid].StatusType),pos,TCOLpRT_StType,iface);
			else
				_tupRT->setItem("Undefined",pos,TCOLpRT_StType,iface);
			s.str("");
			s << boost::format("%d") %RunEvInfo.CounterFail;
			_tupRT->setItem(s.str(),pos,TCOLpRT_Fails,iface);
			s.str("");
			s << boost::format("%d") %RunEvInfo.DataMC;
			_tupRT->setItem(s.str(),pos,TCOLpRT_DataMC,iface);
			s.str("");
			s << boost::format("%d") %RunEvInfo.TFEvent;
			_tupRT->setItem(s.str(),pos,TCOLpRT_TEvent,iface);
			s.str("");
			s << boost::format("%d") %RunEvInfo.TLEvent;
			_tupRT->setItem(s.str(),pos,TCOLpRT_TLEvent,iface);
		}
		if (sort_state) _tupRT->sort(sort_state);

		// For Control table selection
		TreeItem *selection = reinterpret_cast<TreeItem *>((*_tuTreeWidget2)->currentItem());
		if (selection && selection->type() == ITYPE_RUN)
		{
	    	string path = _tuTreeWidget2->parent_path(selection,ITYPE_HOST);

	    	tabUtils *tabu = _map_ctrl_forms[path].first;
	    	tabu->filterRows(false);
	 		tabu->filterRows(QString(selection->iface()->vno()),0,true);
	 		tabu->hresize();
			(*tabu)->update();
			// Selected Control table has no selected rows
			pbtReplace->setEnabled(false);
			pbtAdd->setEnabled(false);
			pbtRemove->setEnabled(false);
			pbtMoveUp->setEnabled(false);
			pbtMoveDown->setEnabled(false);
		}
	}

	// -- event action --

	else if (eviface->action() == uiEvent::ACT_VRDLOGCHANGED)
	{
		if (iface->vrd().configured())
		{
			bool sort_state = _tuVrdLog->sort(false);
			_tuVrdLog->rmRows(iface->vno(),TCOLVRD_Serv);
			for (unsigned i=0; i<iface->_vrd.size(); i++) {
				if (!iface->vrd(i).mapped()) continue;
				int pos = _tuVrdLog->addRow();
				_tuVrdLog->setItem(iface->vno(),pos,TCOLVRD_Serv,iface);
				_tuVrdLog->setItem(iface->vrd(i).file,pos,TCOLVRD_File,iface);
				string st = timeString(iface->vrd(i).time);
				_tuVrdLog->setItem(st,pos,TCOLVRD_Time,iface);
				stringstream s;
				s << boost::format("%d") %iface->vrd(i).size;
				_tuVrdLog->setItem(s.str(),pos,TCOLVRD_Size,iface);
				if (iface->vrd(i).file.size() == 0) {
					_tuVrdLog->setColor(color(CLR_ALARM),pos);
					icon("Logs",ICONST_ALARM);
				}
				else {
					time_t now = time(0);

					if (now-iface->vrd(i).time >= 90*60 && now-iface->vrd(i).time <= 150*60) {
						_tuVrdLog->setColor(color(CLR_WARNING),pos);
						icon("Logs",ICONST_WARNING);
					}
					else if (now-iface->vrd(i).time > 150*60) {
						_tuVrdLog->setColor(color(CLR_ALARM),pos);
						icon("Logs",ICONST_ALARM);
					}
					else
						_tuVrdLog->setColor(color(CLR_RUNNING),pos);
				}
			}
			if (sort_state) _tuVrdLog->sort(sort_state);
			icon("Logs");
		}
	}

	// -- event action --

	else if (eviface->action() == uiEvent::ACT_OTHERLOGCHANGED)
	{
		bool sort_state = _tuOtherLog->sort(false);
		time_t now = time(0);
		if (__dfCursor.mapped())
		{
			_tuOtherLog->rmRows("Deframing cursor",TCOLOTHER_Comment);
			int pos = _tuOtherLog->addRow();
			_tuOtherLog->setItem(__dfCursor.file,pos,TCOLOTHER_File,iface);
			string st = timeString(__dfCursor.time);
			_tuOtherLog->setItem(st,pos,TCOLOTHER_Time,iface);
			stringstream s;
			s << boost::format("%d") %__dfCursor.size;
			_tuOtherLog->setItem(s.str(),pos,TCOLOTHER_Size,iface);
			_tuOtherLog->setItem("Deframing cursor",pos,TCOLOTHER_Comment,iface);

			if (now-__dfCursor.time >= 30*60 && now-__dfCursor.time <= 60*60) {
				_tuOtherLog->setColor(color(CLR_WARNING),pos);
				icon("Logs",ICONST_WARNING);
			}
			else if (now-__dfCursor.time > 60*60) {
				_tuOtherLog->setColor(color(CLR_ALARM),pos);
				icon("Logs",ICONST_ALARM);
			}
			else
				_tuOtherLog->setColor(color(CLR_RUNNING),pos);
		}
		if (__dfFrame.mapped())
		{
			_tuOtherLog->rmRows("Last frame",TCOLOTHER_Comment);
			int pos = _tuOtherLog->addRow();
			_tuOtherLog->setItem(__dfFrame.file,pos,TCOLOTHER_File,iface);
			string st = timeString(__dfFrame.time);
			_tuOtherLog->setItem(st,pos,TCOLOTHER_Time,iface);
			stringstream s;
			s << boost::format("%d") %__dfFrame.size;
			_tuOtherLog->setItem(s.str(),pos,TCOLOTHER_Size,iface);
			_tuOtherLog->setItem("Last frame",pos,TCOLOTHER_Comment,iface);

			if (now-__dfFrame.time >= 30*60 && now-__dfFrame.time <= 60*60) {
				_tuOtherLog->setColor(color(CLR_WARNING),pos);
				icon("Logs",ICONST_WARNING);
			}
			else if (now-__dfFrame.time > 60*60) {
				_tuOtherLog->setColor(color(CLR_ALARM),pos);
				icon("Logs",ICONST_ALARM);
			}
			else
				_tuOtherLog->setColor(color(CLR_RUNNING),pos);
		}
		if (__df.mapped())
		{
			_tuOtherLog->rmRows("Deframing log",TCOLOTHER_Comment);
			int pos = _tuOtherLog->addRow();
			_tuOtherLog->setItem(__df.file,pos,TCOLOTHER_File,iface);
			string st = timeString(__df.time);
			_tuOtherLog->setItem(st,pos,TCOLOTHER_Time,iface);
			stringstream s;
			s << boost::format("%d") %__df.size;
			_tuOtherLog->setItem(s.str(),pos,TCOLOTHER_Size,iface);
			_tuOtherLog->setItem("Deframing log",pos,TCOLOTHER_Comment,iface);

			if (now-__df.time >= 30*60 && now-__df.time <= 60*60) {
				_tuOtherLog->setColor(color(CLR_WARNING),pos);
				icon("Logs",ICONST_WARNING);
			}
			else if (now-__df.time > 60*60) {
				_tuOtherLog->setColor(color(CLR_ALARM),pos);
				icon("Logs",ICONST_ALARM);
			}
			else
				_tuOtherLog->setColor(color(CLR_RUNNING),pos);
		}
		if (__transfer.mapped())
		{
			_tuOtherLog->rmRows("Transfer log",TCOLOTHER_Comment);
			int pos = _tuOtherLog->addRow();
			_tuOtherLog->setItem(__transfer.file,pos,TCOLOTHER_File,iface);
			string st = timeString(__transfer.time);
			_tuOtherLog->setItem(st,pos,TCOLOTHER_Time,iface);
			stringstream s;
			s << boost::format("%d") %__transfer.size;
			_tuOtherLog->setItem(s.str(),pos,TCOLOTHER_Size,iface);
			_tuOtherLog->setItem("Transfer log",pos,TCOLOTHER_Comment,iface);

			if (now-__transfer.time >= 30*60 && now-__transfer.time <= 60*60) {
				_tuOtherLog->setColor(color(CLR_WARNING),pos);
				icon("Logs",ICONST_WARNING);
			}
			else if (now-__transfer.time > 60*60) {
				_tuOtherLog->setColor(color(CLR_ALARM),pos);
				icon("Logs",ICONST_ALARM);
			}
			else
				_tuOtherLog->setColor(color(CLR_RUNNING),pos);
		}
		if (__scdbFile.mapped())
		{
			_tuOtherLog->rmRows("Last SCDB file",TCOLOTHER_Comment);
			int pos = _tuOtherLog->addRow();
			_tuOtherLog->setItem(__scdbFile.file,pos,TCOLOTHER_File,iface);
			string st = timeString(__scdbFile.time);
			_tuOtherLog->setItem(st,pos,TCOLOTHER_Time,iface);
			stringstream s;
			s << boost::format("%d") %__scdbFile.size;
			_tuOtherLog->setItem(s.str(),pos,TCOLOTHER_Size,iface);
			_tuOtherLog->setItem("Last SCDB file",pos,TCOLOTHER_Comment,iface);

			if (now-__scdbFile.time >= 24*3600 && now-__scdbFile.time <= 32*3600) {
				_tuOtherLog->setColor(color(CLR_WARNING),pos);
				icon("Logs",ICONST_WARNING);
			}
			else if (now-__scdbFile.time > 32*3600) {
				_tuOtherLog->setColor(color(CLR_ALARM),pos);
				icon("Logs",ICONST_ALARM);
			}
			else
				_tuOtherLog->setColor(color(CLR_RUNNING),pos);
		}
		if (__scdb.mapped())
		{
			_tuOtherLog->rmRows("SCDB log",TCOLOTHER_Comment);
			int pos = _tuOtherLog->addRow();
			_tuOtherLog->setItem(__scdb.file,pos,TCOLOTHER_File,iface);
			string st = timeString(__scdb.time);
			_tuOtherLog->setItem(st,pos,TCOLOTHER_Time,iface);
			stringstream s;
			s << boost::format("%d") %__scdb.size;
			_tuOtherLog->setItem(s.str(),pos,TCOLOTHER_Size,iface);
			_tuOtherLog->setItem("SCDB log",pos,TCOLOTHER_Comment,iface);

			if (now-__scdb.time >= 24*3600 && now-__scdb.time <= 32*3600) {
				_tuOtherLog->setColor(color(CLR_WARNING),pos);
				icon("Logs",ICONST_WARNING);
			}
			else if (now-__scdb.time > 32*3600) {
				_tuOtherLog->setColor(color(CLR_ALARM),pos);
				icon("Logs",ICONST_ALARM);
			}
			else
				_tuOtherLog->setColor(color(CLR_RUNNING),pos);
		}
		if (sort_state) _tuOtherLog->sort(sort_state);
		icon("Logs");
	}

	// Finally
	if (eviface->action() != uiEvent::ACT_UPDSTATUSBAR)
	{
		_tuTreeWidget->hresize();
		_tuTreeWidget2->hresize();
		_tuMyLog->hresize();

		map<uistring, tabUtils *>::const_iterator it;
 		for (it=_map_forms.begin(); it != _map_forms.end(); ++it) {
			tabUtils *tabu = it->second;
			tabu->hresize();
			(*tabu)->update();
		}
 		map<uistring, pair<tabUtils *, int> >::const_iterator cit;
		for (cit=_map_ctrl_forms.begin(); cit != _map_ctrl_forms.end(); ++cit) {
			tabUtils *tabu = cit->second.first;
			tabu->hresize();
			(*tabu)->update();
		}
	}
	return true;
}

void ui::visibleRuns(srviface *iface, vector<int> &runs) const
{
	for (int i=0; i<(*_tuProducerRuns)->rowCount(); i++) {
		if ((*_tuProducerRuns)->isRowHidden(i) ||
			_tuProducerRuns->getItem(i,TCOLPR_Serv) != iface->vno())
				continue;
		try {
			long uid = boost::lexical_cast<long>(_tuProducerRuns->getItem(i,TCOLPR_Uid));
			int rid = iface->findRunEvInfoByUid(uid);
			if (rid < 0) {
				log() << "ui::visibleRuns-E- no RT found, uid=" << boost::format("%u") %uid << endl;
				continue;
			}
			runs.push_back(rid);
		}
		catch(boost::bad_lexical_cast &ex) { }
	}
}

void ui::visibleClients(srviface *iface, vector<int> &clients) const
{
	for (int i=0; i<(*_tuActiveClients)->rowCount(); i++) {
		if ((*_tuActiveClients)->isRowHidden(i) ||
				_tuActiveClients->getItem(i,TCOL_Serv) != iface->vno())
				continue;
		try {
			long uid = boost::lexical_cast<long>(_tuActiveClients->getItem(i,TCOL_Id));
			int acid = iface->findAcsByUid(uid);
			if (acid < 0) {
				log() << "ui::visibleClients-E- no AC found, uid=" << boost::format("%u") %uid << endl;
				continue;
			}
			int rid = iface->findRunEvInfoByUid(uid);
			if (rid < 0) {
				log() << "ui::visibleClients-E- no RT found, uid=" << boost::format("%u") %uid << endl;
				continue;
			}
			clients.push_back(acid);
		}
		catch(boost::bad_lexical_cast &ex) { }
	}
}

void ui::slUpdateStatus()
{
	// Clear icons status
	for (int i=0; i<theUI.tabWidget->count(); i++) {
		uistring s(theUI.tabWidget->tabText(i));
		theUI.icon(s.c_str(),ui::ICONST_OK,true);
	}
	// Request data
    setDataRequested();
	__evAskData.raise();
}

void ui::mnuResetHosts()
{
	TreeItem *sel = reinterpret_cast<TreeItem *>((*_tuTreeWidget)->currentItem());
	if (sel) {
		const TreeItem *server = _tuTreeWidget->parent(sel,ITYPE_SERVER);
		srviface *iface = srviface::iface(uistring(server->text(0)).c_str());
		_thread<srviface *> thr;
		thr.run(&srviface::resetHosts,iface);
	}
	else {
		warning myWarning("Please select destination amsprodserver");
		myWarning.setupUi();
		myWarning->show();
		myWarning->exec();
	}
}

void ui::mnuResetFailedHosts()
{
	TreeItem *sel = reinterpret_cast<TreeItem *>((*_tuTreeWidget)->currentItem());
	if (sel) {
		const TreeItem *server = _tuTreeWidget->parent(sel,ITYPE_SERVER);
		srviface *iface = srviface::iface(uistring(server->text(0)).c_str());
		_thread<srviface *> thr;
		thr.run(&srviface::resetFailedHosts,iface);
	}
	else {
		warning myWarning("Please select destination amsprodserver");
		myWarning.setupUi();
		myWarning->show();
		myWarning->exec();
	}
}

void ui::mnuResetFailedRuns()
{
	TreeItem *sel = reinterpret_cast<TreeItem *>((*_tuTreeWidget)->currentItem());
	if (sel) {
		const TreeItem *server = _tuTreeWidget->parent(sel,ITYPE_SERVER);
		srviface *iface = srviface::iface(uistring(server->text(0)).c_str());
		_thread<srviface::prFailedRuns> thr;
		thr.run(&srviface::resetFailedRuns,srviface::prFailedRuns(iface,false));
	}
	else {
		warning myWarning("Please select destination amsprodserver");
		myWarning.setupUi();
		myWarning->show();
		myWarning->exec();
	}
}

void ui::mnuResetFailedRunsDST()
{
	TreeItem *sel = reinterpret_cast<TreeItem *>((*_tuTreeWidget)->currentItem());
	if (sel) {
		const TreeItem *server = _tuTreeWidget->parent(sel,ITYPE_SERVER);
		srviface *iface = srviface::iface(uistring(server->text(0)).c_str());
		_thread<srviface::prFailedRuns> thr;
		thr.run(&srviface::resetFailedRuns,srviface::prFailedRuns(iface,true));
	}
	else {
		warning myWarning("Please select destination amsprodserver");
		myWarning.setupUi();
		myWarning->show();
		myWarning->exec();
	}
}

void ui::mnuResetHistory()
{
	TreeItem *sel = reinterpret_cast<TreeItem *>((*_tuTreeWidget)->currentItem());
	if (sel) {
		const TreeItem *server = _tuTreeWidget->parent(sel,ITYPE_SERVER);
		srviface *iface = srviface::iface(uistring(server->text(0)).c_str());
		_thread<srviface *> thr;
		thr.run(&srviface::resetHistory,iface);
	}
	else {
		warning myWarning("Please select destination amsprodserver");
		myWarning.setupUi();
		myWarning->show();
		myWarning->exec();
	}
}

void ui::mnuDeleteValidatedDST()
{
	TreeItem *sel = reinterpret_cast<TreeItem *>((*_tuTreeWidget)->currentItem());
	if (sel) {
		const TreeItem *server = _tuTreeWidget->parent(sel,ITYPE_SERVER);
		srviface *iface = srviface::iface(uistring(server->text(0)).c_str());
		_thread<srviface *> thr;
		thr.run(&srviface::deleteValidatedDST,iface);
	}
	else {
		warning myWarning("Please select destination amsprodserver");
		myWarning.setupUi();
		myWarning->show();
		myWarning->exec();
	}
}

void ui::mnuDeleteFailedDST()
{
	TreeItem *sel = reinterpret_cast<TreeItem *>((*_tuTreeWidget)->currentItem());
	if (sel) {
		const TreeItem *server = _tuTreeWidget->parent(sel,ITYPE_SERVER);
		srviface *iface = srviface::iface(uistring(server->text(0)).c_str());
		_thread<srviface *> thr;
		thr.run(&srviface::deleteFailedDST,iface);
	}
	else {
		warning myWarning("Please select destination amsprodserver");
		myWarning.setupUi();
		myWarning->show();
		myWarning->exec();
	}
}

void ui::mnuDeleteActiveClients()
{
	TreeItem *sel = reinterpret_cast<TreeItem *>((*_tuTreeWidget)->currentItem());
	if (sel) {
		const TreeItem *server = _tuTreeWidget->parent(sel,ITYPE_SERVER);
		srviface *iface = srviface::iface(uistring(server->text(0)).c_str());
		_thread<srviface *> thr;
		thr.run(&srviface::deleteAClients,iface);
	}
	else {
		warning myWarning("Please select destination amsprodserver");
		myWarning.setupUi();
		myWarning->show();
		myWarning->exec();
	}
}

void ui::mnuDeleteRuns()
{
	TreeItem *sel = reinterpret_cast<TreeItem *>((*_tuTreeWidget)->currentItem());
	if (sel) {
		const TreeItem *server = _tuTreeWidget->parent(sel,ITYPE_SERVER);
		srviface *iface = srviface::iface(uistring(server->text(0)).c_str());
		_thread<srviface *> thr;
		thr.run(&srviface::deleteRuns,iface);
	}
	else {
		warning myWarning("Please select destination amsprodserver");
		myWarning.setupUi();
		myWarning->show();
		myWarning->exec();
	}
}

void ui::mnuFinishFailedRuns()
{
	TreeItem *sel = reinterpret_cast<TreeItem *>((*_tuTreeWidget)->currentItem());
	if (sel) {
		const TreeItem *server = _tuTreeWidget->parent(sel,ITYPE_SERVER);
		srviface *iface = srviface::iface(uistring(server->text(0)).c_str());
		_thread<srviface *> thr;
		thr.run(&srviface::finishFailedRuns,iface);
	}
	else {
		warning myWarning("Please select destination amsprodserver");
		myWarning.setupUi();
		myWarning->show();
		myWarning->exec();
	}
}

void ui::mnuUpdateAFSToken()
{
	TreeItem *sel = reinterpret_cast<TreeItem *>((*_tuTreeWidget)->currentItem());
	if (sel) {
		const TreeItem *server = _tuTreeWidget->parent(sel,ITYPE_SERVER);
		srviface *iface = srviface::iface(uistring(server->text(0)).c_str());
		_thread<srviface *> thr;
		thr.run(&srviface::updateAFSToken,iface);
	}
	else {
		warning myWarning("Please select destination amsprodserver");
		myWarning.setupUi();
		myWarning->show();
		myWarning->exec();
	}
}

void ui::mnuActiveHosts()
{
    setDataRequested();
	__evAskData.raise();
}

void ui::mnuLockUpdate()
{
	if (actLockUpdate->isChecked() == false) {
		setDataRequested();
		__evAskData.raise();
	}
}

void ui::mnuUseFilter()
{
	setDataRequested();
	__evAskData.raise();
}

void ui::mnuZoomIn()
{
	zoom(ZOOM_In,_tuTreeWidget);
	zoom(ZOOM_In,_tuTreeWidget2);
	zoom(ZOOM_In,_tuMyLog);

	QFont &font = const_cast<QFont &>(statusBar->font());
	int sz = font.pointSize();
	font.setPointSize(sz+1);
	statusBar->setFont(font);

	map<uistring, tabUtils *>::const_iterator it;
	for (it=_map_forms.begin(); it != _map_forms.end(); ++it) {
		tabUtils *tabu = it->second;
		tabu->_hrState = 0;
		zoom(ZOOM_In,tabu);
	}
	map<uistring, pair<tabUtils *, int> >::const_iterator cit;
	for (cit=_map_ctrl_forms.begin(); cit != _map_ctrl_forms.end(); ++cit) {
		tabUtils *tabu = cit->second.first;
		tabu->_hrState = 0;
		zoom(ZOOM_In,tabu);
	}
}

void ui::mnuZoomOut()
{
	zoom(ZOOM_Out,_tuTreeWidget);
	zoom(ZOOM_Out,_tuTreeWidget2);
	zoom(ZOOM_Out,_tuMyLog);

	QFont &font = const_cast<QFont &>(statusBar->font());
	int sz = font.pointSize();
	font.setPointSize(sz-1);
	statusBar->setFont(font);

	map<uistring, tabUtils *>::const_iterator it;
	for (it=_map_forms.begin(); it != _map_forms.end(); ++it) {
		tabUtils *tabu = it->second;
		tabu->_hrState = 0;
		zoom(ZOOM_Out,tabu);
	}
	map<uistring, pair<tabUtils *, int> >::const_iterator cit;
	for (cit=_map_ctrl_forms.begin(); cit != _map_ctrl_forms.end(); ++cit) {
		tabUtils *tabu = cit->second.first;
		tabu->_hrState = 0;
		zoom(ZOOM_Out,tabu);
	}
}

void ui::mnuClearMyLogging() { (*_tuMyLog)->clear(); }

void ui::mnuExportDB()
{
	TreeItem *sel = reinterpret_cast<TreeItem *>((*_tuTreeWidget)->currentItem());
	srviface *iface;
	if (sel) {
		const TreeItem *server = _tuTreeWidget->parent(sel,ITYPE_SERVER);
		iface = srviface::iface(uistring(server->text(0)).c_str());
	}
	else {
		warning myWarning("Please select destination amsprodserver");
		myWarning.setupUi();
		myWarning->show();
		myWarning->exec();
		return;
	}

	QStringList files;
	QFileDialog dialog(_mw);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setViewMode(QFileDialog::Detail);
	const char *title = "Export to c++ BDB";
	dialog.setWindowTitle(QApplication::translate(title,title,0,QApplication::UnicodeUTF8));
	if (!dialog.exec()) {
		log() << "ui::mnuExportDB()-E- no export file specified" << endl;
		return;
	}
	files = dialog.selectedFiles();
	string file = files.first().toStdString();
	string senv = file.substr(0,file.find_last_of('/'));
	string sfile = file.substr(file.find_last_of('/')+1);

	log() << "ui::mnuExportDB()-I- export to " << file << endl;
	try {
		iface->bdbPut(senv,sfile,0);
	}
	catch (string &ex) {
		log() << "ui::mnuExportDB()-E-" << ex << endl;
	}
}

void ui::mnuExportDBMerged()
{
	TreeItem *sel = reinterpret_cast<TreeItem *>((*_tuTreeWidget)->currentItem());
	srviface *iface;
	if (sel) {
		const TreeItem *server = _tuTreeWidget->parent(sel,ITYPE_SERVER);
		iface = srviface::iface(uistring(server->text(0)).c_str());
	}
	else {
		warning myWarning("Please select destination amsprodserver");
		myWarning.setupUi();
		myWarning->show();
		myWarning->exec();
		return;
	}

	QStringList files;
	QFileDialog dialog(_mw);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setViewMode(QFileDialog::Detail);
	const char *title = "Export to perl BDB";
	dialog.setWindowTitle(QApplication::translate(title,title,0,QApplication::UnicodeUTF8));
	if (!dialog.exec()) {
		log() << "ui::mnuExportDBMerged()-E- no export file specified" << endl;
		return;
	}
	files = dialog.selectedFiles();
	string file = files.first().toStdString();

	log() << "ui::mnuExportDBMerged()-I- export to " << file << endl;
	try {
		iface->bdbPutPerl(__thePerl,file);
	}
	catch (string &ex) {
		log() << "ui::mnuExportDBMerged()-E-" << ex << endl;
	}
}

void ui::mnuImportDB()
{
	TreeItem *sel = reinterpret_cast<TreeItem *>((*_tuTreeWidget)->currentItem());
	srviface *iface;
	if (sel) {
		const TreeItem *server = _tuTreeWidget->parent(sel,ITYPE_SERVER);
		iface = srviface::iface(uistring(server->text(0)).c_str());
	}
	else {
		warning myWarning("Please select destination amsprodserver");
		myWarning.setupUi();
		myWarning->show();
		myWarning->exec();
		return;
	}

	QStringList files;
	QFileDialog dialog(_mw);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setViewMode(QFileDialog::Detail);
	const char *title = "Import from text file to c++ BDB" ;
	dialog.setWindowTitle(QApplication::translate(title,title,0,QApplication::UnicodeUTF8));
	if (!dialog.exec()) {
		log() << "ui::mnuImportDB()-E- no import file specified" << endl;
		return;
	}
	files = dialog.selectedFiles();
	string file = files.first().toStdString();
	string senv = file.substr(0,file.find_last_of('/'));
	string sfile = file.substr(file.find_last_of('/')+1);

	log() << "ui::mnuImportDB()-I- import from " << file << endl;
	try {
		iface->bdbGetText(senv,sfile,0);
	}
	catch (string &ex) {
		log() << "ui::mnuImportDB()-E-" << ex << endl;
	}
}

void ui::mnuSetFilter()
{
	if (!_shSelFilt.isAttached()) {
		warning myWarning("Error: no shared memory attached");
		myWarning.setupUi();
		myWarning.setTitle("Set filter");
		myWarning->exec();
		return;
	}
    shFilter before, after;
	if (before.attach(_shSelFilt)) before >> before._cash;

 	filter *myFilter = new filter(_shSelFilt.key());
 	myFilter->setupUi();
 	myFilter->setTitle("Set filter");
	(*myFilter)->exec();

	if (after.attach(before)) after >> after._cash;
	if (_shSelFilt.inuse() && after != before) {
		setDataRequested();
		__evAskData.raise();
	}
}

void ui::clkTreeWidget(QTreeWidgetItem *item, int column)
{
	treeclick(reinterpret_cast<TreeItem *>(item),column);
};

void ui::clkTreeWidget2(QTreeWidgetItem *item, int column)
{
	TreeItem *node = reinterpret_cast<TreeItem *>(item);
    if (node->type() == ITYPE_RUN)
    {
    	string path = _tuTreeWidget2->parent_path(node,ITYPE_HOST);

    	tabUtils *tabu = _map_ctrl_forms[path].first;
    	tabu->filterRows(false);
 		tabu->filterRows(QString(node->iface()->vno()),0,true);
		if (tabu->_hrState == 0)
			tabu->_hrState = 1;
 		tabu->hresize();
		(*tabu)->update();

		stackedWidget2->setCurrentIndex(_map_ctrl_forms[path].second);

		int row = (*tabu)->currentRow();
		bool sel = tabu->editable() &&
				   (row >= 0 && tabu->getItem(row,0) == node->iface()->vno());
		if (sel && (strstr(path.c_str(),"/Nominal Client") || strstr(path.c_str(),"/Nominal Killer")))
			theUI.pbtReplace->setEnabled(false);
		else
			theUI.pbtReplace->setEnabled(sel);
		theUI.pbtAdd->setEnabled(sel);
		theUI.pbtRemove->setEnabled(sel);
		theUI.pbtNew->setEnabled(tabu->editable());
		theUI.pbtReplaceAll->setEnabled(tabu->editable());
		theUI.pbtMoveUp->setEnabled(sel);
		theUI.pbtMoveDown->setEnabled(sel);
    }
    else
    {
    	stackedWidget2->setCurrentIndex(0);

		theUI.pbtReplace->setEnabled(false);
		theUI.pbtAdd->setEnabled(false);
		theUI.pbtRemove->setEnabled(false);
		theUI.pbtNew->setEnabled(false);
		theUI.pbtReplaceAll->setEnabled(false);
		theUI.pbtMoveUp->setEnabled(false);
		theUI.pbtMoveDown->setEnabled(false);
    }
};

void ui::tabChanged(int index)
{
	for (int i=0; i<tabWidget->count(); i++)
	{
		uistring s(tabWidget->tabText(i));
		icon(s.c_str());
	}
	uistring s(tabWidget->tabText(index));
	if (s == "Control")	stackedWidget->setCurrentIndex(1);
	  else				stackedWidget->setCurrentIndex(0);
}

void ui::clkbtReplace(bool checked)
{
	__mxProcData.lock();

	tabUtils *tabu = getVisibleCtrlTab();
	if (tabu && tabu->editable()) {
//!!!!! investigate memory leaks
// for (int i=0; i<1000; i++)
		tabu->operation(DPS::Client::Update,(*tabu)->currentRow());
	}
	__mxProcData.unlock();
}

void ui::clkbtAdd(bool checked)
{
	__mxProcData.lock();

	tabUtils *tabu = getVisibleCtrlTab();
	if (tabu && tabu->editable()) {
		tabu->operation(DPS::Client::Create,(*tabu)->currentRow());
    }
    __mxProcData.unlock();
}

void ui::clkbtRemove(bool checked)
{
	__mxProcData.lock();

	tabUtils *tabu = getVisibleCtrlTab();
	if (tabu && tabu->editable()) {
		tabu->operation(DPS::Client::Delete,(*tabu)->currentRow());
    }
    __mxProcData.unlock();
}

void ui::clkbtNew(bool checked)
{
	tabUtils *tabu = getVisibleCtrlTab();
	srviface *iface = _tuTreeWidget2->getServerSelected();
	if (tabu && tabu->editable() && iface) {
		int row = tabu->addRow();
		tabu->setItem(iface->vno(),row,0,iface);
		(*tabu)->setCurrentCell(row,0);
	}
}

void ui::clkbtReplaceAll(bool checked)
{
	__mxProcData.lock();

	tabUtils *tabu = getVisibleCtrlTab();
	const char *name = getVisibleCtrlTabName(tabu);
	if (tabu && tabu->editable() && name) {
		if (strstr(name,"/Nominal Client") || strstr(name,"/Nominal Killer") || strstr(name,"/Nominal Host"))
			tabu->operation(DPS::Client::Update);
		else
			for (int i=0; i<(*tabu)->rowCount(); i++)
				if (!(*tabu)->isRowHidden(i))
					tabu->operation(DPS::Client::Update,i);
	}
	__mxProcData.unlock();
}

void ui::clkbtMoveUp(bool checked)
{
	__mxProcData.lock();

	tabUtils *tabu = getVisibleCtrlTab();
	if (tabu && tabu->editable()) {
		int cr = (*tabu)->currentRow();
		if (cr >= 1) {
			int cr2 = cr;
			for (cr-=1; cr>=0; cr--)
				if (!(*tabu)->isRowHidden(cr))
					break;
			if (cr >= 0) {
				vector<uistring> col;
				for (int i=0; i<(*tabu)->columnCount(); i++)
					col.push_back(tabu->getItem(cr2,i));
				srviface *iface = tabu->iface(cr2,0);
				(*tabu)->removeRow(cr2);
				(*tabu)->insertRow(cr);
				for (int i=0; i<(*tabu)->columnCount(); i++)
					tabu->setItem(col[i],cr,i,iface);
				(*tabu)->selectRow(cr);
			}
		}
    }
	__mxProcData.unlock();
}

void ui::clkbtMoveDown(bool checked)
{
	__mxProcData.lock();

	tabUtils *tabu = getVisibleCtrlTab();
	if (tabu && tabu->editable()) {
		int cr = (*tabu)->currentRow();
		if (cr < (*tabu)->rowCount()-1) {
			int cr2 = cr;
			for (cr+=1; cr<(*tabu)->rowCount(); cr++)
				if (!(*tabu)->isRowHidden(cr))
					break;
			if (cr < (*tabu)->rowCount()) {
				vector<uistring> col;
				for (int i=0; i<(*tabu)->columnCount(); i++)
					col.push_back(tabu->getItem(cr,i));
				srviface *iface = tabu->iface(cr,0);
				(*tabu)->removeRow(cr);
				(*tabu)->insertRow(cr2);
				for (int i=0; i<(*tabu)->columnCount(); i++)
					tabu->setItem(col[i],cr2,i,iface);
				(*tabu)->selectRow(cr);
				QModelIndex modelIndex = (*tabu)->indexAt(QPoint(0,(*tabu)->rowViewportPosition(cr)));
				(*tabu)->scrollTo(modelIndex);
			}
		}
    }
	__mxProcData.unlock();
}

//_____________________________________________________________________
// class mylogUtils

bool mylogUtils::eventFilter(QObject *obj, QEvent *event)
{
	if (qtabUtils::eventFilter(obj,event))
		return _qtw->eventFilter(obj,event);
	return true;
}

//_____________________________________________________________________
// class warning public

void warning::setupUi()
{
	Ui_Dialog::setupUi(_qtw);
	const char *title = "Warning";
	setTitle(title);
	_qtw->setModal(true);
	dlgWarning->setText(uistring(_message).q_str());
	QObject::connect(pbtWarningOk,SIGNAL(clicked(bool)),this, SLOT(ok(bool)));
}

void warning::setTitle(const char *title)
{
	_qtw->setWindowTitle(QApplication::translate(title,title,0,QApplication::UnicodeUTF8));
}

void warning::ok(bool checked) { _qtw->close(); }

//_____________________________________________________________________
// class viewer public

void viewer::setupUi()
{
	Ui_Form::setupUi(_qtw);
	plainTextEdit->installEventFilter(this);
	QObject::connect(pbtUp,SIGNAL(clicked(bool)),this, SLOT(clkbtUp(bool)));
	QObject::connect(pbtDown,SIGNAL(clicked(bool)),this, SLOT(clkbtDown(bool)));
	QObject::connect(pbtUpd,SIGNAL(clicked(bool)),this, SLOT(clkbtUpd(bool)));
}

void viewer::getFile(const char *file)
{
	if (file) _file = file;
	  else    file = _file.c_str();
	string use_file = file;
	_contents.clear();

	CharStringToken st(file,":");
	if (st.tokens() > 2 && strlen(st.token(1)) == 0) {
		CharStringToken st2(st.token(0),"@");
		uistring cmd = srviface::shos() + st2.token(0) + " ";
		if (st2.tokens() > 1 && string(st2.token(1)) == "lsf")
			cmd += string("bpeek ") + st.token(2);
		if (st2.tokens() > 1 && string(st2.token(1)) == "condor")
			cmd += string("condor_tail -maxbytes 100000000 ") + st.token(2);
		else
			cmd += string("cat ") + st.tostr(":",2);
	    qprocess proc;
	    if (!proc.run(cmd)) {
	    	uistring s;
			while (proc.getline(s))
				_contents.push_back(s);
			setTitle(use_file.c_str());
			return;
	    }
		use_file = st.tostr(":",2);
		log()<<"viewer::getFile-E- exec error on: "<<proc.lastcmd()<<endl;
	}
	fstream fs(use_file.c_str(),fstream::in);
	if (fs.is_open()) {
		uistring s;
		while (!std::getline(fs,s).eof())
			_contents.push_back(s);
		fs.close();
	}
	else
		log()<<"viewer::getFile-E- can't open file: "<<use_file<<endl;
	setTitle(use_file.c_str());
}

void viewer::setFile(const char *file) { if (file) _file = file; }

void viewer::setTitle(const char *title)
{
	_qtw->setWindowTitle(QApplication::translate(title,title,0,QApplication::UnicodeUTF8));
}

void viewer::clkbtUp(bool checked)
{
	uistring pattern = lnePattern->text();
	if (pattern.size()) {
		if (chkCaseSens->isChecked())
			plainTextEdit->find(pattern.q_str(),QTextDocument::FindBackward | QTextDocument::FindCaseSensitively);
		else
			plainTextEdit->find(pattern.q_str(),QTextDocument::FindBackward);
	}
}

void viewer::clkbtDown(bool checked)
{
	uistring pattern = lnePattern->text();
	if (pattern.size()) {
		if (chkCaseSens->isChecked())
			plainTextEdit->find(pattern.q_str(),QTextDocument::FindCaseSensitively);
		else
			plainTextEdit->find(pattern.q_str());
	}
}

void viewer::clkbtUpd(bool checked)
{
	_thread<viewer *> thr;
	thr.run(&thrViewer,this);
	pbtUpd->setEnabled(false);
}

bool viewer::eventFilter(QObject *obj, QEvent *event)
{
	QKeyEvent *kevent;
	int key;

	if (event->type() == QEvent::KeyPress) {
		kevent = dynamic_cast<QKeyEvent *>(event);
		key = kevent->key();
		switch (key) {
		case Qt::Key_Control:
			_ctrl_pressed = true;
			break;
		}
		if (_ctrl_pressed) {
			switch (key) {
			case Qt::Key_Plus:
			case Qt::Key_Equal:
				ui::zoom(ui::ZOOM_In,this);
				return true;
			case Qt::Key_Minus:
				ui::zoom(ui::ZOOM_Out,this);
				return true;
			}
		}
	}
	else if (event->type() == QEvent::KeyRelease)
	{
		kevent = dynamic_cast<QKeyEvent *>(event);
		key = kevent->key();
		switch (key) {
		case Qt::Key_Control:
			_ctrl_pressed = false;
			break;
		}
	}
	return false;
}

bool viewer::event(QEvent *ev)
{
	plainTextEdit->clear();
	for (std::list<uistring>::iterator it=_contents.begin(); it != _contents.end(); ++it)
		plainTextEdit->appendPlainText(it->q_str());
	_contents.clear();
	pbtUpd->setEnabled(true);
	(*this)->update();
	return true;
}

//_____________________________________________________________________
// class filter public

filter::filter(const char *key): Ui_FilterDialog()
{
	_qtw = new QDialog();
	_key = key;
}

void filter::setupUi()
{
    shFilter shSelFilt;

	if (!shSelFilt.attach(_key.c_str()))
		return;
	shSelFilt >> shSelFilt._cash;

	Ui_FilterDialog::setupUi(_qtw);

	CharStringToken st(shSelFilt._cash._servers,",");
	for (int i=0; i<st.tokens(); i++)
		pcServer->addItem(QString(st.token(i)));
	pcServer->setCurrentIndex(shSelFilt._cash._csServer);
	pcHostName->setText(QString(shSelFilt._cash._csHostName.c_str()));
	pcRunStatus->setCurrentIndex(shSelFilt._cash._rStatus);
	pcRunId->setText(QString(shSelFilt._cash._rRun.c_str()));
	pcUid->setText(QString(shSelFilt._cash._rUid.c_str()));
	pcRunDataSet->setText(QString(shSelFilt._cash._rDataSet.c_str()));
	pcPriority->setText(QString(shSelFilt._cash._rPrior.c_str()));
	pcDataMC->setText(QString(shSelFilt._cash._rDataMC.c_str()));
	pcClientStatus->setCurrentIndex(shSelFilt._cash._acStatus);
	pcThreads->setText(QString(shSelFilt._cash._acThreads.c_str()));
	pcProcessID->setText(QString(shSelFilt._cash._acProcessID.c_str()));
	shSelFilt.detach();

	QObject::connect(pbApply,SIGNAL(clicked(bool)),this, SLOT(clkbtApply(bool)));
	QObject::connect(pbReset,SIGNAL(clicked(bool)),this, SLOT(clkbtReset(bool)));
	QObject::connect(pbCancel,SIGNAL(clicked(bool)),this, SLOT(clkbtCancel(bool)));
}

void filter::setTitle(const char *title)
{
	_qtw->setWindowTitle(QApplication::translate(title,title,0,QApplication::UnicodeUTF8));
}

void filter::clkbtApply(bool checked)
{
    shFilter shSelFilt;

	if (!shSelFilt.attach(_key.c_str()))
		return;
	shSelFilt >> shSelFilt._cash;
	shSelFilt._cash._csServer = pcServer->currentIndex();
	shSelFilt._cash._csHostName = uistring(pcHostName->text());
	shSelFilt._cash._rStatus = pcRunStatus->currentIndex();
	shSelFilt._cash._rRun = uistring(pcRunId->text());
	shSelFilt._cash._rUid = uistring(pcUid->text());
	shSelFilt._cash._rDataSet = uistring(pcRunDataSet->text());
	shSelFilt._cash._rPrior = uistring(pcPriority->text());
	shSelFilt._cash._rDataMC = uistring(pcDataMC->text());
	shSelFilt._cash._acStatus = pcClientStatus->currentIndex();
	shSelFilt._cash._acThreads = uistring(pcThreads->text());
	shSelFilt._cash._acProcessID = uistring(pcProcessID->text());
	shSelFilt << shSelFilt._cash;
	shSelFilt.detach();
}

void filter::clkbtReset(bool checked)
{
    shFilter shSelFilt;

	if (!shSelFilt.attach(_key.c_str()))
		return;
	shSelFilt >> shSelFilt._cash;
	uistring dummy("");
	pcServer->setCurrentIndex(0);
	pcHostName->setText(dummy.q_str());
	pcRunStatus->setCurrentIndex(0);
	pcRunId->setText(dummy.q_str());
	pcUid->setText(dummy.q_str());
	pcRunDataSet->setText(dummy.q_str());
	pcPriority->setText(dummy.q_str());
	pcDataMC->setText(dummy.q_str());
	pcClientStatus->setCurrentIndex(0);
	pcThreads->setText(dummy.q_str());
	pcProcessID->setText(dummy.q_str());
	shSelFilt._cash._csServer = 0;
	shSelFilt._cash._csHostName = dummy;
	shSelFilt._cash._rStatus = 0;
	shSelFilt._cash._rRun = dummy;
	shSelFilt._cash._rUid = dummy;
	shSelFilt._cash._rDataSet = dummy;
	shSelFilt._cash._rPrior = dummy;
	shSelFilt._cash._rDataMC = dummy;
	shSelFilt._cash._acStatus = 0;
	shSelFilt._cash._acThreads = dummy;
	shSelFilt._cash._acProcessID = dummy;
	shSelFilt << shSelFilt._cash;
	shSelFilt.detach();
}

void filter::clkbtCancel(bool checked) { _qtw->close(); }

//_____________________________________________________________________
// class editColumn

editColumn::editColumn(const char *key): Ui_editColumnDialog()
{
	_qtw = new QDialog();
	_key = key;
}

void editColumn::setupUi()
{
	Ui_editColumnDialog::setupUi(_qtw);
	pcField->setFocus();
	QObject::connect(pbtnOK,SIGNAL(clicked(bool)),this, SLOT(clkbtOK(bool)));
	QObject::connect(pbtnCancel,SIGNAL(clicked(bool)),this, SLOT(clkbtCancel(bool)));
}

void editColumn::setTitle(const char *title)
{
	_qtw->setWindowTitle(QApplication::translate(title,title,0,QApplication::UnicodeUTF8));
}

void editColumn::clkbtOK(bool checked)
{
	shCedit shCedit;

	if (!shCedit.attach(_key.c_str()))
		return;
	shCedit >> shCedit._cash;
	shCedit._cash._data = uistring(pcField->text());
	shCedit._cash._upd++;
	shCedit << shCedit._cash;
	shCedit.detach();
	_qtw->close();
}

void editColumn::clkbtCancel(bool checked) { _qtw->close(); }

//_____________________________________________________________________
// class selFile

selFile::selFile(const char *key): Ui_selFileDialog()
{
	_qtw = new QDialog();
	_key = key;
}

void selFile::setupUi()
{
	Ui_selFileDialog::setupUi(_qtw);

	shSfile shSelFile;
	if (!shSelFile.attach(_key.c_str()))
		return;
	shSelFile >> shSelFile._cash;
	QStringList filelst;
	for (unsigned i=0; i<shSelFile._cash._data.size(); i++)
		filelst << uistring(shSelFile._cash._data[i]).q_str();
    pcbStr->insertItems(0,filelst);
    pcbStr->setCurrentIndex(shSelFile._cash._selected);

	_qtw->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	_qtw->setStyleSheet("border: 1px solid black;");
	pbtnOK->setStyleSheet("border: 1px solid gray; background-color: white;");
	pcbStr->setStyleSheet("border: 1px solid lightgray;");
	QObject::connect(pbtnOK,SIGNAL(clicked(bool)),this, SLOT(clkbtOK(bool)));
}

void selFile::setTitle(const char *title)
{
	_qtw->setWindowTitle(QApplication::translate(title,title,0,QApplication::UnicodeUTF8));
}

void selFile::clkbtOK(bool)
{
	shSfile shSelFile;
	if (!shSelFile.attach(_key.c_str()))
		return;
	shSelFile >> shSelFile._cash;
	shSelFile._cash._selected = pcbStr->currentIndex();
	shSelFile << shSelFile._cash;
	_qtw->close();
}

//_____________________________________________________________________
// class myQMainWindow

bool qtabUtils::eventFilter(QObject *obj, QEvent *event)
{
	bool ctrl_pressed = (QApplication::keyboardModifiers() & Qt::ControlModifier);
	bool alt_pressed = (QApplication::keyboardModifiers() & Qt::AltModifier);

	if (event->type() == QEvent::KeyPress) {
		QKeyEvent *kev = dynamic_cast<QKeyEvent *>(event);
		if (ctrl_pressed || alt_pressed) {
			QKeyEvent *newEvent = new QKeyEvent(kev->type(),kev->key(),kev->modifiers(),kev->text(),kev->isAutoRepeat(),kev->count());
			QApplication::postEvent(theUI._mw,newEvent);
			return false;
		}
	}
	else if (event->type() == QEvent::KeyRelease) {
		QKeyEvent *kev = dynamic_cast<QKeyEvent *>(event);
		int key = kev->key();
		switch (key) {
		case Qt::Key_Control:
		case Qt::Key_Alt:
			return false;
		}
	}
	else if (event->type() == QEvent::ContextMenu) {
		QContextMenuEvent *cev = dynamic_cast<QContextMenuEvent *>(event);
		QContextMenuEvent *newEvent = new QContextMenuEvent(cev->reason(),cev->pos(),cev->globalPos(),cev->modifiers());
		QApplication::postEvent(theUI._mw,newEvent);
		return false;
	}
	return true;
}

bool myQMainWindow::event(QEvent *event)
{
	_ctrl_pressed = (QApplication::keyboardModifiers() & Qt::ControlModifier);
	_alt_pressed = (QApplication::keyboardModifiers() & Qt::AltModifier);

	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *kevent = dynamic_cast<QKeyEvent *>(event);
		int key = kevent->key();
		switch (key) {
		case Qt::Key_Control:
		case Qt::Key_Alt:
			return QMainWindow::event(event);
		}
		if (_ctrl_pressed) {
			int tabid = theUI.tabWidget->currentIndex();
			if (tabid < 0) tabid = 0;
			int ntabs = theUI.tabWidget->count();

			switch (key) {
			case Qt::Key_Plus:
			case Qt::Key_Equal:
				emit theUI.mnuZoomIn();
				break;
			case Qt::Key_Minus:
				emit theUI.mnuZoomOut();
				break;
			case Qt::Key_A:
				emit theUI.clkbtAdd(false);
				break;
			case Qt::Key_F:
				emit theUI.mnuSetFilter();
				break;
			case Qt::Key_R:
				emit theUI.clkbtRemove(false);
				break;
			case Qt::Key_U:
				emit theUI.clkbtReplace(false);
				break;
			case Qt::Key_X:
				theUI.kickev(static_cast<uistring *>(0));
				break;
			case Qt::Key_Left:
				theUI.tabWidget->setCurrentIndex((tabid+ntabs-1) % ntabs);
				break;
			case Qt::Key_Right:
				theUI.tabWidget->setCurrentIndex((tabid+1) % ntabs);
				break;
			}
		}
		else if (_alt_pressed) {
			switch (key) {
			case Qt::Key_H:
				emit theUI.mnuResetHosts();
				break;
			case Qt::Key_G:
				emit theUI.mnuResetFailedHosts();
				break;
			case Qt::Key_Q:
				emit theUI.mnuQuit();
				break;
			case Qt::Key_U:
				emit theUI.slUpdateStatus();
				break;
			case Qt::Key_V:
				emit theUI.mnuDeleteValidatedDST();
				break;
			}
		}
	}
	else if (event->type() == QEvent::ShortcutOverride) {
		QKeyEvent *kevent = dynamic_cast<QKeyEvent *>(event);
		int key = kevent->key();
		switch (key) {
		case Qt::Key_Up:
		case Qt::Key_Down:
		case Qt::Key_PageUp:
		case Qt::Key_PageDown:
			_caller = theUI.getVisibleTab();
			break;
		}
	}
	else if (event->type() == QEvent::KeyRelease)
	{
		QKeyEvent *kevent = dynamic_cast<QKeyEvent *>(event);
		int key = kevent->key();
		switch (key) {
		case Qt::Key_Control:
		case Qt::Key_Alt:
			break;
		default:
			if (_caller) {
				tabUtils *tabu = static_cast<tabUtils *>(_caller);
				if (tabu) {
					QItemSelectionModel *selmod = (*tabu)->selectionModel();
					if (selmod->hasSelection()) {
						QModelIndexList sellist = selmod->selectedRows();
						int row = sellist.first().row();
						emit tabu->clkSelection(row,0);
					}
				}
				_caller = 0;
			}
			break;
		}
	}
	else if (event->type() == QEvent::ContextMenu)
	{
		tabUtils *tabu = theUI.getVisibleCtrlTab();
		if (tabu && tabu->editable()) {
			QContextMenuEvent *cev = dynamic_cast<QContextMenuEvent *>(event);
			QHeaderView *hHdr = (*tabu)->horizontalHeader();
			// recalculate proper position
			QPoint cpos = cev->pos();
			cpos.setX(cpos.x()-(*tabu)->verticalHeader()->contentsRect().right());
			int colid = hHdr->logicalIndexAt(cpos);
			if (colid>=0 && hHdr->contentsRect().contains(cpos)) {
				// right position found
				uistring colnm = (*tabu)->horizontalHeaderItem(colid)->data(Qt::DisplayRole).toString();
				if (!theUI._shCtrEdit.isAttached()) {
					warning myWarning("Error: no shared memory attached");
					myWarning.setupUi();
					myWarning.setTitle(colnm.c_str());
					myWarning->move(cev->globalPos());
					myWarning->exec();
					return false;
				}
				if (colid == tabu->_sort.column) {
					warning myWarning("Please make this column unsorted");
					myWarning.setupUi();
					myWarning.setTitle(colnm.c_str());
					myWarning->move(cev->globalPos());
					myWarning->exec();
					return false;
				}
				shCedit before, after;
				if (before.attach(theUI._shCtrEdit))
					before >> before._cash;
				editColumn *column = new editColumn(before.key());
				column->setupUi();
				column->setTitle(colnm.c_str());
				(*column)->move(cev->globalPos());
				(*column)->exec();
				if (after.attach(before))
					after >> after._cash;
				// change contents of table column
				if (after != before) {
					__mxProcData.lock();
					for (int i=0; i<(*tabu)->rowCount(); i++)
						if (!(*tabu)->isRowHidden(i)) {
							TableItem *item = reinterpret_cast<TableItem *>((*tabu)->item(i,colid));
							if (item)
								item->setText(QString(after._cash._data.c_str()));
						}
					__mxProcData.unlock();
				}
			}
		}
		return false;
	}
	return QMainWindow::event(event);
}
