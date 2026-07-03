#include "uiwrapper_dbvw.h"
#include "../dbServer/dbserver.hxx"
#include "../dbServer/bdbRC.hxx"
#include "../dbServer/bdbTraits.hxx"
#include "ManMapper.hxx"

namespace idl {
	const char *_strClientType[] = {
		"Generic", "Server", "Consumer", "Producer", "Monitor", "Killer", "DBServer", "ODBServer", "Standalone" };
	senum<DPS::Client::ClientType> enumClientType(_strClientType,sizeof(_strClientType)/sizeof(char *),DPS::Client::Generic);
	const char *_strClientStatusType[] = {
		"Permanent", "OneRunOnly" };
	senum<DPS::Client::ClientStatusType> enumClientStatusType(_strClientStatusType,sizeof(_strClientStatusType)/sizeof(char *),DPS::Client::OneRunOnly);
	const char *_strClientStatus[] = {
		"Lost", "Unknown", "Submitted", "Registered", "Active", "TimeOut", "Killed" };
	senum<DPS::Client::ClientStatus> enumClientStatus(_strClientStatus,sizeof(_strClientStatus)/sizeof(char *),DPS::Client::Unknown);
	const char *_strClientExiting[] = {
		"NOP", "CInExit", "SInExit", "CInAbort", "SInAbort", "SInKill" };
	senum<DPS::Client::ClientExiting> enumClientExiting(_strClientExiting,sizeof(_strClientExiting)/sizeof(char *),DPS::Client::NOP);
	const char *_strRecordChange[] = {
		"Delete", "Update", "Create", "ClearCreate" };
	senum<DPS::Producer::RecordChange> enumRecordChange(_strRecordChange,sizeof(_strRecordChange)/sizeof(char *),DPS::Producer::Update);
	const char *_strRunStatus[] = {
		"ToBeRerun", "Failed", "Processing", "Finished", "Unknown", "Allocated", "Foreign", "Canceled" };
	senum<DPS::Producer::RunStatus> enumRunStatus(_strRunStatus,sizeof(_strRunStatus)/sizeof(char *),DPS::Producer::Unknown);
	const char *_strRunMode[] = {
		"RILO", "LILO", "RIRO", "LIRO" };
	senum<DPS::Producer::RunMode> enumRunMode(_strRunMode,sizeof(_strRunMode)/sizeof(char *),DPS::Producer::LILO);
	const char *_strDSTStatus[] = {
		"Success", "InProgress", "Failure", "Validated" };
	senum<DPS::Producer::DSTStatus> enumDSTStatus(_strDSTStatus,sizeof(_strDSTStatus)/sizeof(char *),DPS::Producer::Failure);
	const char *_strDSTType[] = {
			"Ntuple", "EventTag", "RootFile", "RawFile" };
	senum<DPS::Producer::DSTType> enumDSTType(_strDSTType,sizeof(_strDSTType)/sizeof(char *),DPS::Producer::RootFile);
	const char *_strHostStatus[] = {
		"NoResponse", "InActive", "LastClientFailed", "OK" };
	senum<DPS::Client::HostStatus> enumHostStatus(_strHostStatus,sizeof(_strHostStatus)/sizeof(char *),DPS::Client::OK);
}
using namespace idl;

//_____________________________________________________________________

treeUtils::treeUtils(QTree *widget) throw() {
	_qtw = widget;
    _qtw->setSelectionBehavior(QAbstractItemView::SelectRows);
    _qtw->setSelectionMode(QAbstractItemView::SingleSelection);
    _qtw->setCurrentItem(0);
    QObject::connect(_qtw,SIGNAL(itemSelectionChanged()),this, SLOT(clkSelectionChanged()));
}

void treeUtils::init(TreeItem *item) const {
}

void treeUtils::hresize() const {
	_qtw->resizeColumnToContents(0);
}

void treeUtils::vresize() const {
}

TreeItem *treeUtils::setItem(const string &text, int column, TreeItem *item) const {
	item->setText(column,text.c_str());
	return item;
}

void treeUtils::clkSelectionChanged() {
	QList<QTreeWidgetItem *> l = _qtw->selectedItems();
	if (l.size()) theUI.clkTreeWidget2(l.at(0),0);
}

//_____________________________________________________________________
// class tabUtils

tabUtils::tabUtils(QTable *widget) throw()
{
	_qtw = widget;
    _qtw->setSelectionBehavior(QAbstractItemView::SelectRows);
    _qtw->verticalHeader()->setDefaultAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    for (int i=0,len=_qtw->columnCount(); i<len; i++) {
    	uistring key = _qtw->horizontalHeaderItem(i)->text();
    	_columns[key] = i;
    }
    _sort.column = -1;
    _hrState = 0;
    hresize();
    vresize();
    QObject::connect(_qtw->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(clktbHeader(int)));
    QObject::connect(_qtw,SIGNAL(cellClicked(int, int)),this, SLOT(clkSelection(int, int)));
    QObject::connect(_qtw,SIGNAL(itemSelectionChanged()),this, SLOT(clkSelectionChanged()));
};

void tabUtils::init(TableItem *item) const
{
	item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void tabUtils::hresize()
{
	MapCtrlForms::const_iterator cit;
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

int tabUtils::addRow() const
{
	int lastrow = _qtw->rowCount();
	_qtw->insertRow(lastrow);
	return lastrow;
}

int tabUtils::hasSelection() const
{
	int result = 0;
	QList<QTableWidgetSelectionRange> sl = _qtw->selectedRanges();
	for (int i=0,l=sl.length(); i<l; i++)
		result += sl[i].rowCount();
	return result;
}

void tabUtils::rmRow(int row) const
{
	if (row < 0)
		while (_qtw->rowCount())
			_qtw->removeRow(_qtw->rowCount()-1);
	else
		_qtw->removeRow(row);
}

void tabUtils::clktbHeader(int logid)
{
	if (logid != _sort.column) {
		_sort.column = logid;
	    _sort.order = SORT_Ascend;
	}
	if (_sort.order != SORT_No) {
		_qtw->setSortingEnabled(true);
		_qtw->sortByColumn(logid,static_cast<Qt::SortOrder>(_sort.order));
		_sort.order = (_sort.order+1) % (static_cast<int>(SORT_No)+1);
	}
	else {
		_qtw->setSortingEnabled(false);
		_sort.column = -1;
		for (MapCtrlForms::iterator it=theUI._map_ctrl_forms.begin();
				it != theUI._map_ctrl_forms.end(); ++it) {
			tabUtils *tab = it->second.first;
			if (tab == this) {
				tab->rmRow();
				tab->dbin();
				break;
			}
		}
	}
}

void tabUtils::clkSelection(int row, int column)
{
	clkSelectionChanged();
}

void tabUtils::clkSelectionChanged()
{
	if (hasSelection()>0) {
		theUI.pbtRemove->setEnabled(true);
		theUI.pbtMoveUp->setEnabled(true);
		theUI.pbtMoveDown->setEnabled(true);
	}
	else {
		theUI.pbtRemove->setEnabled(false);
		theUI.pbtMoveUp->setEnabled(false);
		theUI.pbtMoveDown->setEnabled(false);
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

void sNC::dbin(bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
    try {
    	bdbRC db(usedb);
   		db.connect2(DBO_NSL);
   		nc_dt db_ncs(db);
   		for (int i=0,len=db_ncs.length(); i<len; i++,db_ncs++) {
   			const DPS::Producer::NominalClient &Nc = *db_ncs;
			int pos = addRow();
			setItem(Nc.uid,pos,"Uid");
			setItem(enumClientType.key(Nc.Type),pos,"Type");
			setItem(Nc.MaxClients,pos,"MaxClients");
			setItem(Nc.CPUNeeded,pos,"CPU");
			setItem(Nc.MemoryNeeded,pos,"Memory");
			setItem(string(Nc.WholeScriptPath),pos,"ScriptPath");
			setItem(string(Nc.LogPath),pos,"LogPath");
			setItem(string(Nc.SubmitCommand),pos,"Submit");
			setItem(string(Nc.HostName),pos,"HostName");
			setItem(Nc.LogInTheEnd,pos,"LoginTheEnd");
   		}
		theUI.kickev(uiEvent::ACT_TABCHANGED,this);
    }
	catch (string &ex) {
		cout<<"sNC::dbin-E- " << ex << endl;
	}
}

void sNC::dbout(bdbRC *db) {
	int rows = _qtw->rowCount();
	if (rows < 0) return;
	bdbRC *usedb(db?db:theDB);
	try {
    	bdbRC db(usedb);
   		db.connect2(DBO_NSL);
   		nc_dt db_ncs(db);
   		db_ncs.erase();
		for (int row=0; row<rows; row++) {
   			DPS::Producer::NominalClient Nc;
			Nc.uid = getItem<unsigned long>(row,"Uid");
			Nc.Type = enumClientType.value(getItem<string>(row,"Type").c_str());
			Nc.MaxClients = getItem<short>(row,"MaxClients");
			Nc.CPUNeeded = getItem<float>(row,"CPU");
			Nc.MemoryNeeded = getItem<short>(row,"Memory");
			Nc.WholeScriptPath = getItem<string>(row,"ScriptPath").c_str();
			Nc.LogPath = getItem<string>(row,"LogPath").c_str();
			Nc.SubmitCommand = getItem<string>(row,"Submit").c_str();
			Nc.HostName = getItem<string>(row,"HostName").c_str();
			Nc.LogInTheEnd = getItem<short>(row,"LoginTheEnd");
   			db_ncs.insert(Nc);
		}
	}
	catch (string &ex) {
		cout<<"sNC::dbout-E- " << ex << endl;
	}
}

void sNC::dbin_imex(ofstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
    try {
    	bdbRC db(usedb);
   		db.connect2(DBO_NSL);
   		nc_dt db_ncs(db);
		fs << "<<" << DBO_NSL << ">>" << endl;
   		for (int i=0,len=db_ncs.length(); i<len; i++,db_ncs++) {
   			const DPS::Producer::NominalClient &Nc = *db_ncs;
			fs << "<::uid>" << Nc.uid <<
				"<::Type>" << Nc.Type <<
				"<::MaxClients>" << Nc.MaxClients <<
				"<::CPUNeeded>" << Nc.CPUNeeded <<
				"<::MemoryNeeded>" << Nc.MemoryNeeded <<
				"<::WholeScriptPath>" << (const char*)Nc.WholeScriptPath <<
				"<::LogPath>" << (const char*)Nc.LogPath <<
				"<::SubmitCommand>" << (const char*)Nc.SubmitCommand <<
				"<::HostName>" << (const char*)Nc.HostName <<
				"<::LogInTheEnd>" << Nc.LogInTheEnd << endl;
   		}
		fs << endl;
    }
	catch (string &ex) {
		cout<<"sNC::dbin_imex-E- " << ex << endl;
	}
}

void sNC::dbout_imex(ifstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
	try {
		bdbRC db(usedb);
   		db.connect2(DBO_NSL);
   		nc_dt db_ncs(db);
   		db_ncs.erase();

		string s;
		DPS::Producer::NominalClient Nc;
		streampos last = fs.tellg();
		while (!std::getline(fs,s).eof()) {
			if (s.find("<<") == 0) {
				fs.seekg(last);
				return;
			} else {
				CharStringToken st(s);
				st.strtok2("<::");
				for (int i=0; i<st.tokens(); i++) {
					string ss = st.token(i);
					string ssv;
					if (ss.find('>') != string::npos)
						ssv = ss.substr(ss.find('>')+1);
					// insert parsing below
					if (ss.find("uid>") == 0)
						Nc.uid = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("Type>") == 0)
						Nc.Type = (DPS::Producer::ClientType)boost::lexical_cast<int>(ssv);
					else if (ss.find("MaxClients>") == 0)
						Nc.MaxClients = boost::lexical_cast<short>(ssv);
					else if (ss.find("CPUNeeded>") == 0)
						Nc.CPUNeeded = boost::lexical_cast<float>(ssv);
					else if (ss.find("MemoryNeeded>") == 0)
						Nc.MemoryNeeded = boost::lexical_cast<short>(ssv);
					else if (ss.find("WholeScriptPath>") == 0)
						Nc.WholeScriptPath = ssv.c_str();
					else if (ss.find("LogPath>") == 0)
						Nc.LogPath = ssv.c_str();
					else if (ss.find("SubmitCommand>") == 0)
						Nc.SubmitCommand = ssv.c_str();
					else if (ss.find("HostName>") == 0)
						Nc.HostName = ssv.c_str();
					else if (ss.find("LogInTheEnd>") == 0)
						Nc.LogInTheEnd = boost::lexical_cast<short>(ssv);
				}
				// insert into db
				if (st.tokens()) db_ncs.insert(Nc);
			}
			streampos last = fs.tellg();
		}
	}
	catch (string &ex) {
		cout<<"sNC::dbout_imex-E- " << ex << endl;
	}
    catch(boost::bad_lexical_cast &ex) {
    	cout<<"sNC::dbout_imex-E- boost::bad_lexical_cast ex"<<endl;
    }
}

void sNH::dbin(bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
    try {
    	bdbRC db(usedb);
		db.connect2(DBO_NHL);
		nh_dt db_nhs(db);
		for (int i=0,len=db_nhs.length(); i<len; i++,db_nhs++) {
			const DPS::Producer::NominalHost &Nh = *db_nhs;
			int pos = addRow();
			setItem(uistring(Nh.HostName),pos,"HostName");
			setItem(uistring(Nh.Interface),pos,"Interface");
			setItem(uistring(Nh.OS),pos,"OS");
			setItem(Nh.CPUNumber,pos,"CPUNumb");
			setItem(Nh.Memory,pos,"Memory");
			setItem(Nh.Clock,pos,"Clock");
			setItem(0,pos,"ClientsAllowed");
			setItem("NotFound",pos,"Status");
   		}
		theUI.kickev(uiEvent::ACT_TABCHANGED,this);
    }
	catch (string &ex) {
		cout<<"sNH::dbin-E- " << ex << endl;
	}
}

void sNH::dbout(bdbRC *db) {
	int rows = _qtw->rowCount();
	if (rows < 0) return;
	bdbRC *usedb(db?db:theDB);
	try {
    	bdbRC db(usedb);
		db.connect2(DBO_NHL);
		nh_dt db_nhs(db);
		db_nhs.erase();
		for (int row=0; row<rows; row++) {
			DPS::Producer::NominalHost Nh;
			Nh.HostName = getItem<string>(row,"HostName").c_str();
			Nh.Interface = getItem<string>(row,"Interface").c_str();
			Nh.OS = getItem<string>(row,"OS").c_str();
			Nh.CPUNumber = getItem<short>(row,"CPUNumb");
			Nh.Memory = getItem<short>(row,"Memory");
			Nh.Clock = getItem<long>(row,"Clock");
			db_nhs.insert(Nh);
		}
	}
	catch (string &ex) {
		cout<<"sNH::dbout-E- " << ex << endl;
	}
}

void sNH::dbin_imex(ofstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
    try {
    	bdbRC db(usedb);
		db.connect2(DBO_NHL);
		nh_dt db_nhs(db);
		fs << "<<" << DBO_NHL << ">>" << endl;
		for (int i=0,len=db_nhs.length(); i<len; i++,db_nhs++) {
			const DPS::Producer::NominalHost &Nh = *db_nhs;
			fs << "<::HostName>" << (const char*)Nh.HostName <<
				"<::Interface>" << (const char*)Nh.Interface <<
				"<::OS>" << (const char*)Nh.OS <<
				"<::CPUNumber>" << Nh.CPUNumber <<
				"<::Memory>" << Nh.Memory <<
				"<::Clock>" << Nh.Clock << endl;
   		}
		fs << endl;
    }
	catch (string &ex) {
		cout<<"sNH::dbin_imex-E- " << ex << endl;
	}
}

void sNH::dbout_imex(ifstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
	try {
		bdbRC db(usedb);
		db.connect2(DBO_NHL);
		nh_dt db_nhs(db);
		db_nhs.erase();

		string s;
		DPS::Producer::NominalHost Nh;
		streampos last = fs.tellg();
		while (!std::getline(fs,s).eof()) {
			if (s.find("<<") == 0) {
				fs.seekg(last);
				return;
			} else {
				CharStringToken st(s);
				st.strtok2("<::");
				for (int i=0; i<st.tokens(); i++) {
					string ss = st.token(i);
					string ssv;
					if (ss.find('>') != string::npos)
						ssv = ss.substr(ss.find('>')+1);
					// insert parsing below
					if (ss.find("HostName>") == 0)
						Nh.HostName = ssv.c_str();
					else if (ss.find("Interface>") == 0)
						Nh.Interface = ssv.c_str();
					else if (ss.find("OS>") == 0)
						Nh.OS = ssv.c_str();
					else if (ss.find("CPUNumber>") == 0)
						Nh.CPUNumber = boost::lexical_cast<short>(ssv);
					else if (ss.find("Memory>") == 0)
						Nh.Memory = boost::lexical_cast<short>(ssv);
					else if (ss.find("Clock>") == 0)
						Nh.Clock = boost::lexical_cast<long>(ssv);
				}
				// insert into db
				if (st.tokens()) db_nhs.insert(Nh);
			}
			streampos last = fs.tellg();
		}
	}
	catch (string &ex) {
		cout<<"sNH::dbout_imex-E- " << ex << endl;
	}
    catch(boost::bad_lexical_cast &ex) {
    	cout<<"sNH::dbout_imex-E- boost::bad_lexical_cast ex"<<endl;
    }
}

void sNK::dbin(bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
    try {
    	bdbRC db(usedb);
		db.connect2(DBO_NKL);
		nc_dt db_nks(db);
		for (int i=0,len=db_nks.length(); i<len; i++,db_nks++) {
			const DPS::Producer::NominalClient &Nc = *db_nks;
			int pos = addRow();
			setItem(Nc.uid,pos,"Uid");
			setItem(enumClientType.key(Nc.Type),pos,"Type");
			setItem(Nc.MaxClients,pos,"MaxClients");
			setItem(Nc.CPUNeeded,pos,"CPU");
			setItem(Nc.MemoryNeeded,pos,"Memory");
			setItem(string(Nc.WholeScriptPath),pos,"ScriptPath");
			setItem(string(Nc.LogPath),pos,"LogPath");
			setItem(string(Nc.SubmitCommand),pos,"Submit");
			setItem(string(Nc.HostName),pos,"HostName");
			setItem(Nc.LogInTheEnd,pos,"LoginTheEnd");
   		}
		theUI.kickev(uiEvent::ACT_TABCHANGED,this);
    }
	catch (string &ex) {
		cout<<"sNK::dbin-E- " << ex << endl;
	}
}

void sNK::dbout(bdbRC *db) {
	int rows = _qtw->rowCount();
	if (rows < 0) return;
	bdbRC *usedb(db?db:theDB);
	try {
    	bdbRC db(usedb);
		db.connect2(DBO_NKL);
		nc_dt db_nks(db);
		db_nks.erase();
		for (int row=0; row<rows; row++) {
			DPS::Producer::NominalClient Nc;
			Nc.uid = getItem<unsigned long>(row,"Uid");
			Nc.Type = enumClientType.value(getItem<string>(row,"Type").c_str());
			Nc.MaxClients = getItem<short>(row,"MaxClients");
			Nc.CPUNeeded = getItem<float>(row,"CPU");
			Nc.MemoryNeeded = getItem<short>(row,"Memory");
			Nc.WholeScriptPath = getItem<string>(row,"ScriptPath").c_str();
			Nc.LogPath = getItem<string>(row,"LogPath").c_str();
			Nc.SubmitCommand = getItem<string>(row,"Submit").c_str();
			Nc.HostName = getItem<string>(row,"HostName").c_str();
			Nc.LogInTheEnd = getItem<short>(row,"LoginTheEnd");
			db_nks.insert(Nc);
		}
	}
	catch (string &ex) {
		cout<<"sNK::dbout-E- " << ex << endl;
	}
}

void sNK::dbin_imex(ofstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
    try {
    	bdbRC db(usedb);
		db.connect2(DBO_NKL);
		nc_dt db_nks(db);
		fs << "<<" << DBO_NKL << ">>" << endl;
		for (int i=0,len=db_nks.length(); i<len; i++,db_nks++) {
			const DPS::Producer::NominalClient &Nc = *db_nks;
			fs << "<::uid>" << Nc.uid <<
				"<::Type>" << Nc.Type <<
				"<::MaxClients>" << Nc.MaxClients <<
				"<::CPUNeeded>" << Nc.CPUNeeded <<
				"<::MemoryNeeded>" << Nc.MemoryNeeded <<
				"<::WholeScriptPath>" << (const char*)Nc.WholeScriptPath <<
				"<::LogPath>" << (const char*)Nc.LogPath <<
				"<::SubmitCommand>" << (const char*)Nc.SubmitCommand <<
				"<::HostName>" << (const char*)Nc.HostName <<
				"<::LogInTheEnd>" << Nc.LogInTheEnd << endl;
   		}
		fs << endl;
    }
	catch (string &ex) {
		cout<<"sNK::dbin_imex-E- " << ex << endl;
	}
}

void sNK::dbout_imex(ifstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
	try {
		bdbRC db(usedb);
		db.connect2(DBO_NKL);
		nc_dt db_nks(db);
		db_nks.erase();

		string s;
		DPS::Producer::NominalClient Nc;
		streampos last = fs.tellg();
		while (!std::getline(fs,s).eof()) {
			if (s.find("<<") == 0) {
				fs.seekg(last);
				return;
			} else {
				CharStringToken st(s);
				st.strtok2("<::");
				for (int i=0; i<st.tokens(); i++) {
					string ss = st.token(i);
					string ssv;
					if (ss.find('>') != string::npos)
						ssv = ss.substr(ss.find('>')+1);
					// insert parsing below
					if (ss.find("uid>") == 0)
						Nc.uid = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("Type>") == 0)
							Nc.Type = (DPS::Producer::ClientType)boost::lexical_cast<int>(ssv);
					else if (ss.find("MaxClients>") == 0)
							Nc.MaxClients = boost::lexical_cast<short>(ssv);
					else if (ss.find("CPUNeeded>") == 0)
							Nc.CPUNeeded = boost::lexical_cast<float>(ssv);
					else if (ss.find("MemoryNeeded>") == 0)
							Nc.MemoryNeeded = boost::lexical_cast<short>(ssv);
					else if (ss.find("WholeScriptPath>") == 0)
							Nc.WholeScriptPath = ssv.c_str();
					else if (ss.find("LogPath>") == 0)
							Nc.LogPath = ssv.c_str();
					else if (ss.find("SubmitCommand>") == 0)
							Nc.SubmitCommand = ssv.c_str();
					else if (ss.find("HostName>") == 0)
							Nc.HostName = ssv.c_str();
					else if (ss.find("LogInTheEnd>") == 0)
							Nc.LogInTheEnd  = boost::lexical_cast<short>(ssv);
				}
				// insert into db
				if (st.tokens()) db_nks.insert(Nc);
			}
			streampos last = fs.tellg();
		}
	}
	catch (string &ex) {
		cout<<"sNK::dbout_imex-E- " << ex << endl;
	}
    catch(boost::bad_lexical_cast &ex) {
    	cout<<"sNK::dbout_imex-E- boost::bad_lexical_cast ex"<<endl;
    }
}

void sSE::dbin(bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
    try {
    	bdbRC db(usedb);
		db.connect2(DBO_ENV);
		env_dt db_env(db);
		for (int i=0,len=db_env.length(); i<len; i++,db_env++) {
			uistring st = (const char *)*db_env;
			if (st.tokens("=")) {
				int pos = addRow();
				setItem(st.token(0),pos,"EnvironmentVar");
				if (st.tokens("=") > 1)
					setItem(st.token(1),pos,"Path");
			}
		}
		theUI.kickev(uiEvent::ACT_TABCHANGED,this);
    }
	catch (string &ex) {
		cout<<"sSE::dbin-E- " << ex << endl;
	}
}

void sSE::dbout(bdbRC *db) {
	int rows = _qtw->rowCount();
	if (rows < 0) return;
	bdbRC *usedb(db?db:theDB);
	try {
    	bdbRC db(usedb);
		db.connect2(DBO_ENV);
		env_dt db_env(db);
		db_env.erase();
		for (int row=0; row<rows; row++) {
			string s = getItem<string>(row,"EnvironmentVar") + "=" + getItem<string>(row,"Path");
			CORBA::String_var svar = s.c_str();
			db_env.insert(svar);
		}
	}
	catch (string &ex) {
		cout<<"sSE::dbout-E- " << ex << endl;
	}
}

void sSE::dbin_imex(ofstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
    try {
    	bdbRC db(usedb);
		db.connect2(DBO_ENV);
		env_dt db_env(db);
		fs << "<<" << DBO_ENV << ">>" << endl;
		for (int i=0,len=db_env.length(); i<len; i++,db_env++) {
			uistring st = (const char *)*db_env;
			fs << "<::>" << st << endl;
   		}
		fs << endl;
    }
	catch (string &ex) {
		cout<<"sSE::dbin_imex-E- " << ex << endl;
	}
}

void sSE::dbout_imex(ifstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
	try {
		bdbRC db(usedb);
		db.connect2(DBO_ENV);
		env_dt db_env(db);
		db_env.erase();

		string s;
		CORBA::String_var svar;
		streampos last = fs.tellg();
		while (!std::getline(fs,s).eof()) {
			if (s.find("<<") == 0) {
				fs.seekg(last);
				return;
			} else {
				CharStringToken st(s);
				st.strtok2("<::");
				for (int i=0; i<st.tokens(); i++) {
					string ss = st.token(i);
					string ssv;
					if (ss.find('>') != string::npos)
						ssv = ss.substr(ss.find('>')+1);
					// insert parsing below
					svar = ssv.c_str();
				}
				// insert into db
				if (st.tokens()) db_env.insert(svar);
			}
			streampos last = fs.tellg();
		}
	}
	catch (string &ex) {
		cout<<"sSE::dbout_imex-E- " << ex << endl;
	}
    catch(boost::bad_lexical_cast &ex) {
    	cout<<"sSE::dbout_imex-E- boost::bad_lexical_cast ex"<<endl;
    }
}

void pNC::dbin(bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
	try {
    	bdbRC db(usedb);
   		db.connect2(DBO_NCL);
   		nc_dt db_ncp(db);
   		for (int i=0,len=db_ncp.length(); i<len; i++,db_ncp++) {
   			const DPS::Producer::NominalClient &Nc = *db_ncp;
			int pos = addRow();
			setItem(Nc.uid,pos,"Uid");
			setItem(enumClientType.key(Nc.Type),pos,"Type");
			setItem(Nc.MaxClients,pos,"MaxClients");
			setItem(Nc.CPUNeeded,pos,"CPU");
			setItem(Nc.MemoryNeeded,pos,"Memory");
			setItem(string(Nc.WholeScriptPath),pos,"ScriptPath");
			setItem(string(Nc.LogPath),pos,"LogPath");
			setItem(string(Nc.SubmitCommand),pos,"Submit");
			setItem(string(Nc.HostName),pos,"HostName");
			setItem(Nc.LogInTheEnd,pos,"LoginTheEnd");
   		}
   		theUI.kickev(uiEvent::ACT_TABCHANGED,this);
	}
	catch (string &ex) {
		cout<<"pNC::dbin-E- " << ex << endl;
	}
}

void pNC::dbout(bdbRC *db) {
	int rows = _qtw->rowCount();
	if (rows < 0) return;
	bdbRC *usedb(db?db:theDB);
	try {
		bdbRC db(usedb);
		db.connect2(DBO_NCL);
		nc_dt db_ncp(db);
		db_ncp.erase();
		for (int row=0; row<rows; row++) {
			DPS::Producer::NominalClient Nc;
			Nc.uid = getItem<unsigned long>(row,"Uid");
			Nc.Type = enumClientType.value(getItem<string>(row,"Type").c_str());
			Nc.MaxClients = getItem<short>(row,"MaxClients");
			Nc.CPUNeeded = getItem<float>(row,"CPU");
			Nc.MemoryNeeded = getItem<short>(row,"Memory");
			Nc.WholeScriptPath = getItem<string>(row,"ScriptPath").c_str();
			Nc.LogPath = getItem<string>(row,"LogPath").c_str();
			Nc.SubmitCommand = getItem<string>(row,"Submit").c_str();
			Nc.HostName = getItem<string>(row,"HostName").c_str();
			Nc.LogInTheEnd = getItem<short>(row,"LoginTheEnd");
			db_ncp.insert(Nc);
		}
	}
	catch (string &ex) {
		cout<<"pNC::dbout-E- " << ex << endl;
	}
}

void pNC::dbin_imex(ofstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
    try {
    	bdbRC db(usedb);
   		db.connect2(DBO_NCL);
   		nc_dt db_ncp(db);
		fs << "<<" << DBO_NCL << ">>" << endl;
   		for (int i=0,len=db_ncp.length(); i<len; i++,db_ncp++) {
   			const DPS::Producer::NominalClient &Nc = *db_ncp;
			fs << "<::uid>" << Nc.uid <<
				"<::Type>" << Nc.Type <<
				"<::MaxClients>" << Nc.MaxClients <<
				"<::CPUNeeded>" << Nc.CPUNeeded <<
				"<::MemoryNeeded>" << Nc.MemoryNeeded <<
				"<::WholeScriptPath>" << (const char*)Nc.WholeScriptPath <<
				"<::LogPath>" << (const char*)Nc.LogPath <<
				"<::SubmitCommand>" << (const char*)Nc.SubmitCommand <<
				"<::HostName>" << (const char*)Nc.HostName <<
				"<::LogInTheEnd>" << Nc.LogInTheEnd << endl;
   		}
		fs << endl;
    }
	catch (string &ex) {
		cout<<"pNC::dbin_imex-E- " << ex << endl;
	}
}

void pNC::dbout_imex(ifstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
	try {
		bdbRC db(usedb);
		db.connect2(DBO_NCL);
		nc_dt db_ncp(db);
		db_ncp.erase();

		string s;
		DPS::Producer::NominalClient Nc;
		streampos last = fs.tellg();
		while (!std::getline(fs,s).eof()) {
			if (s.find("<<") == 0) {
				fs.seekg(last);
				return;
			} else {
				CharStringToken st(s);
				st.strtok2("<::");
				for (int i=0; i<st.tokens(); i++) {
					string ss = st.token(i);
					string ssv;
					if (ss.find('>') != string::npos)
						ssv = ss.substr(ss.find('>')+1);
					// insert parsing below
					if (ss.find("uid>") == 0)
						Nc.uid = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("Type>") == 0)
						Nc.Type = (DPS::Producer::ClientType)boost::lexical_cast<int>(ssv);
					else if (ss.find("MaxClients>") == 0)
						Nc.MaxClients = boost::lexical_cast<short>(ssv);
					else if (ss.find("CPUNeeded>") == 0)
						Nc.CPUNeeded = boost::lexical_cast<float>(ssv);
					else if (ss.find("MemoryNeeded>") == 0)
						Nc.MemoryNeeded = boost::lexical_cast<short>(ssv);
					else if (ss.find("WholeScriptPath>") == 0)
						Nc.WholeScriptPath = ssv.c_str();
					else if (ss.find("LogPath>") == 0)
						Nc.LogPath = ssv.c_str();
					else if (ss.find("SubmitCommand>") == 0)
						Nc.SubmitCommand = ssv.c_str();
					else if (ss.find("HostName>") == 0)
						Nc.HostName = ssv.c_str();
					else if (ss.find("LogInTheEnd>") == 0)
						Nc.LogInTheEnd = boost::lexical_cast<short>(ssv);
				}
				// insert into db
				if (st.tokens()) db_ncp.insert(Nc);
			}
			streampos last = fs.tellg();
		}
	}
	catch (string &ex) {
		cout<<"pNC::dbout_imex-E- " << ex << endl;
	}
    catch(boost::bad_lexical_cast &ex) {
    	cout<<"pNC::dbout_imex-E- boost::bad_lexical_cast ex"<<endl;
    }
}

void pAC::dbin(bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
	try {
		bdbRC db(usedb);
		db.connect2(DBO_ACL);
		ac_dt db_aclp(db);
		for (int i=0,len=db_aclp.length(); i<len; i++,db_aclp++) {
			const DPS::Client::ActiveClient &Ac = *db_aclp;
			int pos = addRow();
			setItem(Ac.id.uid,pos,"Uid");
			setItem(string(Ac.id.HostName),pos,"HostName");
			setItem(Ac.id.pid,pos,"Pid");
			setItem(Ac.id.threads,pos,"Threads");
			setItem(Ac.id.Mips,pos,"Mips");
			setItem(Ac.TimeOut,pos,"TimeOut");
			setItem(enumClientStatus.key(Ac.Status),pos,"Status");
			setItem(enumClientStatusType.key(Ac.StatusType),pos,"Type");
			setItem(enumClientExiting.key(Ac.id.Status),pos,"ExitStatus");
			setItem(string(timeString(static_cast<time_t>(Ac.LastUpdate))),pos,"LastUpd");
		}
		theUI.kickev(uiEvent::ACT_TABCHANGED,this);
	}
	catch (string &ex) {
		cout<<"pAC::dbin-E- " << ex << endl;
	}
}

void pAC::dbout(bdbRC *db) {
	int rows = _qtw->rowCount();
	if (rows < 0) return;
	bdbRC *usedb(db?db:theDB);
	try {
		bdbRC db(usedb);
		db.connect2(DBO_ACL);
		ac_dt db_aclp(db);
		db_aclp.erase();
		for (int row=0; row<rows; row++) {
			DPS::Client::ActiveClient Ac;
			Ac.id.uid = getItem<unsigned long>(row,"Uid");
			Ac.id.HostName = getItem<string>(row,"HostName").c_str();
			Ac.id.pid = getItem<unsigned long>(row,"Pid");
			Ac.id.threads = getItem<unsigned long>(row,"Threads");
			Ac.id.Mips = getItem<float>(row,"Mips");
			Ac.TimeOut = getItem<time_t>(row,"TimeOut");
			Ac.Status = enumClientStatus.value(getItem<string>(row,"Status").c_str());
			Ac.StatusType = enumClientStatusType.value(getItem<string>(row,"Type").c_str());
			Ac.id.Status = enumClientExiting.value(getItem<string>(row,"ExitStatus").c_str());
			Ac.LastUpdate = ui::stringTime(getItem<string>(row,"LastUpd"));
			db_aclp.insert(Ac);
		}
	}
	catch (string &ex) {
		cout<<"pAC::dbout-E- " << ex << endl;
	}
}

void pAC::dbin_imex(ofstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
    try {
    	bdbRC db(usedb);
		db.connect2(DBO_ACL);
		ac_dt db_aclp(db);
		fs << "<<" << DBO_ACL << ">>" << endl;
		for (int i=0,len=db_aclp.length(); i<len; i++,db_aclp++) {
			const DPS::Client::ActiveClient &Ac = *db_aclp;
			fs << "<::id::HostName>" << (const char*)Ac.id.HostName <<
				"<::id::uid>" << Ac.id.uid <<
				"<::id::pid>" << Ac.id.pid <<
				"<::id::ppid>" << Ac.id.ppid <<
				"<::id::threads>" << Ac.id.threads <<
				"<::id::threads_change>" << Ac.id.threads_change <<
				"<::id::Type>" << Ac.id.Type <<
				"<::id::StatusType>" << Ac.id.StatusType <<
				"<::id::Status>" << Ac.id.Status <<
				"<::id::Interface>" << (const char*)Ac.id.Interface <<
				"<::id::Mips>" << Ac.id.Mips
				<< "<::id::coid>" << Ac.id.coid;	// CID::coid field added
			for (unsigned j=0; j<Ac.ars.length(); j++) {
				const DPS::Client::ActiveRef &arf = Ac.ars[j];
				fs << "<::ars::IOR>" << (const char*)arf.IOR <<
					"<::ars::Interface>" << (const char*)arf.Interface <<
					"<::ars::Type>" << arf.Type <<
					"<::ars::uid>" << arf.uid;
			}
			fs << "<::LastUpdate>"<< Ac.LastUpdate <<
					"<::Start>"<< Ac.Start <<
					"<::TimeOut>"<< Ac.TimeOut <<
					"<::Status>"<< Ac.Status <<
					"<::StatusType>"<< Ac.StatusType << endl;
   		}
		fs << endl;
    }
	catch (string &ex) {
		cout<<"pAC::dbin_imex-E- " << ex << endl;
	}
}

void pAC::dbout_imex(ifstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
	try {
		bdbRC db(usedb);
		db.connect2(DBO_ACL);
		ac_dt db_aclp(db);
		db_aclp.erase();

		string s;
		DPS::Client::ActiveClient Ac;
		DPS::Client::ActiveRef arf;
		int ars_len = 0;
		streampos last = fs.tellg();
		while (!std::getline(fs,s).eof()) {
			if (s.find("<<") == 0) {
				fs.seekg(last);
				return;
			} else {
				CharStringToken st(s);
				st.strtok2("<::");
				for (int i=0; i<st.tokens(); i++) {
					string ss = st.token(i);
					string ssv;
					if (ss.find('>') != string::npos)
						ssv = ss.substr(ss.find('>')+1);
					// insert parsing below
					if (ss.find("id::HostName>") == 0)
							Ac.id.HostName = ssv.c_str();
					else if (ss.find("id::uid>") == 0)
							Ac.id.uid = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("id::pid>") == 0)
							Ac.id.pid = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("id::ppid>") == 0)
							Ac.id.ppid = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("id::threads>") == 0)
							Ac.id.threads = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("id::threads_change>") == 0)
							Ac.id.threads_change = boost::lexical_cast<long>(ssv);
					else if (ss.find("id::Type>") == 0)
							Ac.id.Type = (DPS::Producer::ClientType)boost::lexical_cast<int>(ssv);
					else if (ss.find("id::StatusType>") == 0)
							Ac.id.StatusType = (DPS::Producer::ClientStatusType)boost::lexical_cast<int>(ssv);
					else if (ss.find("id::Status>") == 0)
							Ac.id.Status = (DPS::Producer::ClientExiting)boost::lexical_cast<int>(ssv);
					else if (ss.find("id::Interface>") == 0)
							Ac.id.Interface = ssv.c_str();
					else if (ss.find("id::Mips>") == 0)
							Ac.id.Mips = boost::lexical_cast<float>(ssv);
					else if (ss.find("id::coid>") == 0)
							Ac.id.coid = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("ars::IOR>") == 0)
							arf.IOR = ssv.c_str();
					else if (ss.find("ars::Interface>") == 0)
							arf.Interface = ssv.c_str();
					else if (ss.find("ars::Type>") == 0)
							arf.Type = (DPS::Producer::ClientType)boost::lexical_cast<int>(ssv);
					else if (ss.find("ars::uid>") == 0) {
							arf.uid = boost::lexical_cast<unsigned long>(ssv);
							Ac.ars.length(ars_len+1);
							DPS::Client::ActiveRef_var arfv = new DPS::Client::ActiveRef(arf);
							Ac.ars[ars_len++] = arfv;
					}
					else if (ss.find("LastUpdate>") == 0)
							Ac.LastUpdate = boost::lexical_cast<time_t>(ssv);
					else if (ss.find("Start>") == 0)
							Ac.Start = boost::lexical_cast<time_t>(ssv);
					else if (ss.find("TimeOut>") == 0)
							Ac.TimeOut = boost::lexical_cast<time_t>(ssv);
					else if (ss.find("Status>") == 0)
							Ac.Status = (DPS::Producer::ClientStatus)boost::lexical_cast<int>(ssv);
					else if (ss.find("StatusType>") == 0)
							Ac.StatusType = (DPS::Producer::ClientStatusType)boost::lexical_cast<int>(ssv);
				}
				// insert into db
				if (st.tokens()) db_aclp.insert(Ac);
			}
			streampos last = fs.tellg();
		}
	}
	catch (string &ex) {
		cout<<"pAC::dbout_imex-E- " << ex << endl;
	}
    catch(boost::bad_lexical_cast &ex) {
    	cout<<"pAC::dbout_imex-E- boost::bad_lexical_cast ex"<<endl;
    }
}

void pNH::dbin(bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
    try {
    	bdbRC db(usedb);
		db.connect2(DBO_AHLP);
		ah_dt db_ahp(db);
		for (int i=0,len=db_ahp.length(); i<len; i++,db_ahp++) {
			const DPS::Producer::ActiveHost &Ah = *db_ahp;
			int pos = addRow();
			setItem(string(Ah.HostName),pos,"HostName");
			setItem(string(Ah.Interface),pos,"Interface");
			setItem(enumHostStatus.key(Ah.Status),pos,"Status");
			setItem(Ah.ClientsRunning,pos,"CliRunning");
			setItem(Ah.ClientsAllowed,pos,"CliAllowed");
			setItem(Ah.ClientsProcessed,pos,"CliProcessed");
			setItem(Ah.ClientsFailed,pos,"CliFailed");
			setItem(Ah.ClientsKilled,pos,"CliKilled");
			if (Ah.LastFailed)
				setItem(string(timeString(static_cast<time_t>(Ah.LastFailed))),pos,"LastFailed");
			else
				setItem(string(""),pos,"LastFailed");
			if (Ah.LastUpdate)
				setItem(string(timeString(static_cast<time_t>(Ah.LastUpdate))),pos,"LastUpdate");
			else
				setItem(string(""),pos,"LastUpdate");
			setItem(Ah.Clock,pos,"Clock");
   		}
   		theUI.kickev(uiEvent::ACT_TABCHANGED,this);
	}
	catch (string &ex) {
		cout<<"pNH::dbin-E- " << ex << endl;
	}
}

void pNH::dbout(bdbRC *db) {
	int rows = _qtw->rowCount();
	if (rows < 0) return;
	bdbRC *usedb(db?db:theDB);
	try {
    	bdbRC db(usedb);
		db.connect2(DBO_AHLP);
		ah_dt db_ahp(db);
		db_ahp.erase();
		for (int row=0; row<rows; row++) {
			DPS::Producer::ActiveHost Ah;
			Ah.HostName = getItem<string>(row,"HostName").c_str();
			Ah.Interface = getItem<string>(row,"Interface").c_str();
			Ah.Status = enumHostStatus.value(getItem<string>(row,"Status").c_str());
			Ah.ClientsRunning = getItem<short>(row,"CliRunning");
			Ah.ClientsAllowed = getItem<short>(row,"CliAllowed");
			Ah.ClientsProcessed = getItem<long>(row,"CliProcessed");
			Ah.ClientsFailed = getItem<long>(row,"CliFailed");
			Ah.ClientsKilled = getItem<long>(row,"CliKilled");
			Ah.LastFailed = ui::stringTime(getItem<string>(row,"LastFailed"));
			Ah.LastUpdate = ui::stringTime(getItem<string>(row,"LastUpdate"));
			Ah.Clock = getItem<long>(row,"Clock");
			db_ahp.insert(Ah);
		}
	}
	catch (string &ex) {
		cout<<"pNH::dbout-E- " << ex << endl;
	}
}

void pNH::dbin_imex(ofstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
    try {
    	bdbRC db(usedb);
		db.connect2(DBO_AHLP);
		ah_dt db_ahp(db);
		fs << "<<" << DBO_AHLP << ">>" << endl;
		for (int i=0,len=db_ahp.length(); i<len; i++,db_ahp++) {
			const DPS::Producer::ActiveHost &Ah = *db_ahp;
			fs << "<::HostName>" << (const char*)Ah.HostName <<
				"<::Interface>" << (const char*)Ah.Interface <<
				"<::Status>" << Ah.Status <<
				"<::ClientsRunning>" << Ah.ClientsRunning <<
				"<::ClientsAllowed>" << Ah.ClientsAllowed <<
				"<::ClientsProcessed>" << Ah.ClientsProcessed <<
				"<::ClientsFailed>" << Ah.ClientsFailed <<
				"<::ClientsKilled>" << Ah.ClientsKilled <<
				"<::LastFailed>" << Ah.LastFailed <<
				"<::LastUpdate>" << Ah.LastUpdate <<
				"<::Clock>" << Ah.Clock << endl;
   		}
		fs << endl;
    }
	catch (string &ex) {
		cout<<"pNH::dbin_imex-E- " << ex << endl;
	}
}

void pNH::dbout_imex(ifstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
	try {
		bdbRC db(usedb);
		db.connect2(DBO_AHLP);
		ah_dt db_ahp(db);
		db_ahp.erase();

		string s;
		DPS::Producer::ActiveHost Ah;
		streampos last = fs.tellg();
		while (!std::getline(fs,s).eof()) {
			if (s.find("<<") == 0) {
				fs.seekg(last);
				return;
			} else {
				CharStringToken st(s);
				st.strtok2("<::");
				for (int i=0; i<st.tokens(); i++) {
					string ss = st.token(i);
					string ssv;
					if (ss.find('>') != string::npos)
						ssv = ss.substr(ss.find('>')+1);
					// insert parsing below
					if (ss.find("HostName>") == 0)
							Ah.HostName = ssv.c_str();
					else if (ss.find("Interface>") == 0)
							Ah.Interface = ssv.c_str();
					else if (ss.find("Status>") == 0)
							Ah.Status = (DPS::Producer::HostStatus)boost::lexical_cast<int>(ssv);
					else if (ss.find("ClientsRunning>") == 0)
							Ah.ClientsRunning = boost::lexical_cast<short>(ssv);
					else if (ss.find("ClientsAllowed>") == 0)
							Ah.ClientsAllowed = boost::lexical_cast<short>(ssv);
					else if (ss.find("ClientsProcessed>") == 0)
							Ah.ClientsProcessed = boost::lexical_cast<long>(ssv);
					else if (ss.find("ClientsFailed>") == 0)
							Ah.ClientsFailed = boost::lexical_cast<long>(ssv);
					else if (ss.find("ClientsKilled>") == 0)
							Ah.ClientsKilled = boost::lexical_cast<long>(ssv);
					else if (ss.find("LastFailed>") == 0)
							Ah.LastFailed = boost::lexical_cast<time_t>(ssv);
					else if (ss.find("LastUpdate>") == 0)
							Ah.LastUpdate = boost::lexical_cast<time_t>(ssv);
					else if (ss.find(":Clock>") == 0)
							Ah.Clock = boost::lexical_cast<long>(ssv);
				}
				// insert into db
				if (st.tokens()) db_ahp.insert(Ah);
			}
			streampos last = fs.tellg();
		}
	}
	catch (string &ex) {
		cout<<"pNH::dbout_imex-E- " << ex << endl;
	}
    catch(boost::bad_lexical_cast &ex) {
    	cout<<"pNH::dbout_imex-E- boost::bad_lexical_cast ex"<<endl;
    }
}

void pNNT::dbin(bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
    try {
    	bdbRC db(usedb);
		db.connect2(DBO_DSTI);
		dsti_dt db_dstip(db);
   		for (int i=0,len=db_dstip.length(); i<len; i++,db_dstip++) {
    			const DPS::Producer::DSTInfo &Dsti = *db_dstip;
    			int pos = addRow();
    			setItem(Dsti.uid,pos,"Uid");
    			setItem(string(Dsti.HostName),pos,"HostName");
    			setItem(string(Dsti.OutputDirPath),pos,"OutputDirPath");
    			setItem(enumRunMode.key(Dsti.Mode),pos,"RunMode");
    			setItem(Dsti.UpdateFreq,pos,"UpdFreq");
    			setItem(enumDSTType.key(Dsti.type),pos,"DstType");
		}
		theUI.kickev(uiEvent::ACT_TABCHANGED,this);
    }
	catch (string &ex) {
		cout<<"pNNT::dbin-E- " << ex << endl;
	}
}

void pNNT::dbout(bdbRC *db) {
	int rows = _qtw->rowCount();
	if (rows < 0) return;
	bdbRC *usedb(db?db:theDB);
	try {
    	bdbRC db(usedb);
		db.connect2(DBO_DSTI);
		dsti_dt db_dstip(db);
		db_dstip.erase();
		for (int row=0; row<rows; row++) {
			DPS::Producer::DSTInfo Dsti;
			Dsti.uid = getItem<unsigned long>(row,"Uid");
			Dsti.HostName = getItem<string>(row,"HostName").c_str();
			Dsti.OutputDirPath = getItem<string>(row,"OutputDirPath").c_str();
			Dsti.Mode =enumRunMode.value(getItem<string>(row,"RunMode").c_str());
			Dsti.UpdateFreq = getItem<long>(row,"UpdFreq");
			Dsti.type = enumDSTType.value(getItem<string>(row,"DstType").c_str());
			db_dstip.insert(Dsti);
		}
	}
	catch (string &ex) {
		cout<<"pNNT::dbout-E- " << ex << endl;
	}
}

void pNNT::dbin_imex(ofstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
    try {
    	bdbRC db(usedb);
		db.connect2(DBO_DSTI);
		dsti_dt db_dstip(db);
		fs << "<<" << DBO_DSTI << ">>" << endl;
   		for (int i=0,len=db_dstip.length(); i<len; i++,db_dstip++) {
   			const DPS::Producer::DSTInfo &Dsti = *db_dstip;
			fs << "<::type>" << Dsti.type <<
				"<::uid>" << Dsti.uid <<
				"<::HostName>" << Dsti.HostName <<
				"<::OutputDirPath>" << Dsti.OutputDirPath <<
				"<::Mode>" << Dsti.Mode <<
				"<::UpdateFreq>" << Dsti.UpdateFreq <<
				"<::DieHard>" << Dsti.DieHard <<
				"<::FreeSpace>" << Dsti.FreeSpace <<
				"<::TotalSpace>" << Dsti.TotalSpace << endl;
   		}
		fs << endl;
    }
	catch (string &ex) {
		cout<<"pNNT::dbin_imex-E- " << ex << endl;
	}
}

void pNNT::dbout_imex(ifstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
	try {
		bdbRC db(usedb);
		db.connect2(DBO_DSTI);
		dsti_dt db_dstip(db);
		db_dstip.erase();

		string s;
		DPS::Producer::DSTInfo Dsti;
		streampos last = fs.tellg();
		while (!std::getline(fs,s).eof()) {
			if (s.find("<<") == 0) {
				fs.seekg(last);
				return;
			} else {
				CharStringToken st(s);
				st.strtok2("<::");
				for (int i=0; i<st.tokens(); i++) {
					string ss = st.token(i);
					string ssv;
					if (ss.find('>') != string::npos)
						ssv = ss.substr(ss.find('>')+1);
					// insert parsing below
					if (ss.find("type>") == 0)
						Dsti.type = (DPS::Producer::DSTType)boost::lexical_cast<int>(ssv);
					else if (ss.find("uid>") == 0)
						Dsti.uid = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("HostName>") == 0)
						Dsti.HostName = ssv.c_str();
					else if (ss.find("OutputDirPath>") == 0)
						Dsti.OutputDirPath = ssv.c_str();
					else if (ss.find("Mode>") == 0)
						Dsti.Mode = (DPS::Producer::RunMode)boost::lexical_cast<int>(ssv);
					else if (ss.find("UpdateFreq>") == 0)
						Dsti.UpdateFreq = boost::lexical_cast<long>(ssv);
					else if (ss.find("DieHard>") == 0)
						Dsti.DieHard = boost::lexical_cast<long>(ssv);
					else if (ss.find("FreeSpace>") == 0)
						Dsti.FreeSpace = boost::lexical_cast<long>(ssv);
					else if (ss.find("TotalSpace>") == 0)
						Dsti.TotalSpace = boost::lexical_cast<long>(ssv);
				}
				// insert into db
				if (st.tokens()) db_dstip.insert(Dsti);
			}
			streampos last = fs.tellg();
		}
	}
	catch (string &ex) {
		cout<<"pNNT::dbout_imex-E- " << ex << endl;
	}
    catch(boost::bad_lexical_cast &ex) {
    	cout<<"pNNT::dbout_imex-E- boost::bad_lexical_cast ex"<<endl;
    }
}

void pPNT::dbin(bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
    try {
    	bdbRC db(usedb);
		db.connect2(DBO_DSTS);
		dsts_dt db_dstp(db);
		for (int i=0,len=db_dstp.length(); i<len; i++,db_dstp++) {
			const DPS::Producer::DST &Dst = *db_dstp;
			int pos = addRow();
			setItem(Dst.Run,pos,"Run");
			setItem(string(timeString(static_cast<time_t>(Dst.Insert))),pos,"Time");
			setItem(Dst.FirstEvent,pos,"FirstEvent");
			setItem(Dst.LastEvent,pos,"LastEvent");
			setItem(string(Dst.Name),pos,"Path");
			setItem(Dst.size,pos,"Size");
			setItem(enumDSTStatus.key(Dst.Status),pos,"Status");
			setItem(enumDSTType.key(Dst.Type),pos,"Type");
		}
		theUI.kickev(uiEvent::ACT_TABCHANGED,this);
    }
	catch (string &ex) {
		cout<<"pPNT::dbin-E- " << ex << endl;
	}
}

void pPNT::dbout(bdbRC *db) {
	int rows = _qtw->rowCount();
	if (rows < 0) return;
	bdbRC *usedb(db?db:theDB);
	try {
    	bdbRC db(usedb);
		db.connect2(DBO_DSTS);
		dsts_dt db_dstp(db);
		db_dstp.erase();
		for (int row=0; row<rows; row++) {
			DPS::Producer::DST Dst;
			Dst.Run = getItem<long>(row,"Run");
			Dst.Insert = getItem<time_t>(row,"Time");
			Dst.FirstEvent = getItem<long>(row,"FirstEvent");
			Dst.LastEvent = getItem<long>(row,"LastEvent");
			Dst.Name = getItem<string>(row,"Path").c_str();
			Dst.size = getItem<long>(row,"Size");
			Dst.Status = enumDSTStatus.value(getItem<string>(row,"Status").c_str());
			Dst.Type = enumDSTType.value(getItem<string>(row,"Type").c_str());
			db_dstp.insert(Dst);
		}
	}
	catch (string &ex) {
		cout<<"pPNT::dbout-E- " << ex << endl;
	}
}

void pPNT::dbin_imex(ofstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
    try {
    	bdbRC db(usedb);
		db.connect2(DBO_DSTS);
		dsts_dt db_dstp(db);
		fs << "<<" << DBO_DSTS << ">>" << endl;
		for (int i=0,len=db_dstp.length(); i<len; i++,db_dstp++) {
			const DPS::Producer::DST &Dst = *db_dstp;
			fs << "<::Insert>" << Dst.Insert <<
				"<::Begin>" << Dst.Begin <<
				"<::End>" << Dst.End <<
				"<::Run>" << Dst.Run <<
				"<::FirstEvent>" << Dst.FirstEvent <<
				"<::LastEvent>" << Dst.LastEvent <<
				"<::EventNumber>" << Dst.EventNumber <<
				"<::ErrorNumber>" << Dst.ErrorNumber <<
				"<::Status>" << Dst.Status <<
				"<::Type>" << Dst.Type <<
				"<::size>" << Dst.size <<
				"<::Name>" << (const char*)Dst.Name <<
				"<::Version>" << (const char*)Dst.Version <<
				"<::crc>" << Dst.crc <<
				"<::rndm1>" << Dst.rndm1 <<
				"<::rndm2>" << Dst.rndm2 <<
				"<::FreeSpace>" << Dst.FreeSpace <<
				"<::TotalSpace>" << Dst.TotalSpace << endl;
   		}
		fs << endl;
    }
	catch (string &ex) {
		cout<<"pPNT::dbin_imex-E- " << ex << endl;
	}
}

void pPNT::dbout_imex(ifstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
	try {
		bdbRC db(usedb);
		db.connect2(DBO_DSTS);
		dsts_dt db_dstp(db);
		db_dstp.erase();

		string s;
		DPS::Producer::DST Dst;
		streampos last = fs.tellg();
		while (!std::getline(fs,s).eof()) {
			if (s.find("<<") == 0) {
				fs.seekg(last);
				return;
			} else {
				CharStringToken st(s);
				st.strtok2("<::");
				for (int i=0; i<st.tokens(); i++) {
					string ss = st.token(i);
					string ssv;
					if (ss.find('>') != string::npos)
						ssv = ss.substr(ss.find('>')+1);
					// insert parsing below
					if (ss.find("Insert>") == 0)
						Dst.Insert = boost::lexical_cast<time_t>(ssv);
					else if (ss.find("Begin>") == 0)
						Dst.Begin = boost::lexical_cast<time_t>(ssv);
					else if (ss.find("End>") == 0)
						Dst.End = boost::lexical_cast<time_t>(ssv);
					else if (ss.find("Run>") == 0)
						Dst.Run = boost::lexical_cast<long>(ssv);
					else if (ss.find("FirstEvent>") == 0)
						Dst.FirstEvent = boost::lexical_cast<long>(ssv);
					else if (ss.find("LastEvent>") == 0)
						Dst.LastEvent = boost::lexical_cast<long>(ssv);
					else if (ss.find("EventNumber>") == 0)
						Dst.EventNumber = boost::lexical_cast<long>(ssv);
					else if (ss.find("ErrorNumber>") == 0)
						Dst.ErrorNumber = boost::lexical_cast<long>(ssv);
					else if (ss.find("Status>") == 0)
						Dst.Status = (DPS::Producer::DSTStatus)boost::lexical_cast<int>(ssv);
					else if (ss.find("Type>") == 0)
						Dst.Type = (DPS::Producer::DSTType)boost::lexical_cast<int>(ssv);
					else if (ss.find("size>") == 0)
						Dst.size = boost::lexical_cast<long>(ssv);
					else if (ss.find("Name>") == 0)
						Dst.Name = ssv.c_str();
					else if (ss.find("Version>") == 0)
						Dst.Version = ssv.c_str();
					else if (ss.find("crc>") == 0)
						Dst.crc = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("rndm1>") == 0)
						Dst.rndm1 = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("rndm2>") == 0)
						Dst.rndm2 = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("FreeSpace>") == 0)
						Dst.FreeSpace = boost::lexical_cast<long>(ssv);
					else if (ss.find("TotalSpace>") == 0)
						Dst.TotalSpace = boost::lexical_cast<long>(ssv);
				}
				// insert into db
				if (st.tokens()) db_dstp.insert(Dst);
			}
			streampos last = fs.tellg();
		}
	}
	catch (string &ex) {
		cout<<"pPNT::dbout_imex-E- " << ex << endl;
	}
    catch(boost::bad_lexical_cast &ex) {
    	cout<<"pPNT::dbout_imex-E- boost::bad_lexical_cast ex"<<endl;
    }
}

void pRT::dbin(bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
    try {
    	bdbRC db(usedb);
		db.connect2(DBO_RTB);
		rtb_dt db_rtp(db);
		for (int i=0,len=db_rtp.length(); i<len; i++,db_rtp++) {
			const DPS::Producer::RunEvInfo &Rei = *db_rtp;
			int pos = addRow();
			setItem(Rei.uid,pos,"Uid");
			setItem(Rei.Run,pos,"Run");
			setItem(Rei.FirstEvent,pos,"FirstEvent");
			setItem(Rei.LastEvent,pos,"LastEvent");
			setItem(Rei.TFEvent,pos,"TEvent");
			setItem(Rei.TLEvent,pos,"TLEvent");
			setItem(Rei.Priority,pos,"Priority");
			setItem(string(Rei.FilePath),pos,"FilePath");
			setItem(enumRunStatus.key(Rei.Status),pos,"Status");
			setItem(enumRunStatus.key(Rei.History),pos,"History");
			setItem(Rei.CounterFail,pos,"Fails");
			if (Rei.SubmitTime)
				setItem(string(timeString(static_cast<time_t>(Rei.SubmitTime))),pos,"SubmTime");
			else
				setItem(string(""),pos,"SubmTime");
			setItem(Rei.cinfo.TimeSpent,pos,"TimeSpent");
			setItem(Rei.cuid,pos,"CUid");
			setItem(Rei.DataMC,pos,"DataMC");
			setItem(string(Rei.cinfo.HostName),pos,"HostName");
		}
		theUI.kickev(uiEvent::ACT_TABCHANGED,this);
    }
	catch (string &ex) {
		cout<<"pRT::dbin-E- " << ex << endl;
	}
}

void pRT::dbout(bdbRC *db) {
	int rows = _qtw->rowCount();
	if (rows < 0) return;
	bdbRC *usedb(db?db:theDB);
	try {
    	bdbRC db(usedb);
		db.connect2(DBO_RTB);
		rtb_dt db_rtp(db);
		db_rtp.erase();
		for (int row=0; row<rows; row++) {
			DPS::Producer::RunEvInfo Rei;
			Rei.uid = getItem<unsigned long>(row,"Uid");
			Rei.Run = getItem<unsigned long>(row,"Run");
			Rei.FirstEvent = getItem<long>(row,"FirstEvent");
			Rei.LastEvent = getItem<long>(row,"LastEvent");
			Rei.TFEvent = getItem<time_t>(row,"TEvent");
			Rei.TLEvent = getItem<time_t>(row,"TLEvent");
			Rei.Priority = getItem<long>(row,"Priority");
			Rei.FilePath = getItem<string>(row,"FilePath").c_str();
			Rei.Status = enumRunStatus.value(getItem<string>(row,"Status").c_str());
			Rei.History = enumRunStatus.value(getItem<string>(row,"History").c_str());
			Rei.CounterFail = getItem<unsigned long>(row,"Fails");
			Rei.SubmitTime = getItem<time_t>(row,"SubmTime");
			Rei.cinfo.TimeSpent = getItem<float>(row,"TimeSpent");
			Rei.cuid = getItem<unsigned long>(row,"CUid");
			Rei.DataMC = getItem<long>(row,"DataMC");
			Rei.cinfo.HostName = getItem<string>(row,"HostName").c_str();
			db_rtp.insert(Rei);
		}
	}
	catch (string &ex) {
		cout<<"pRT::dbout-E- " << ex << endl;
	}
}

void pRT::dbin_imex(ofstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
	try {
		bdbRC db(usedb);
		db.connect2(DBO_RTB);
		rtb_dt db_rtp(db);
		fs << "<<" << DBO_RTB << ">>" << endl;
		for (int i=0,len=db_rtp.length(); i<len; i++,db_rtp++) {
			const DPS::Producer::RunEvInfo &Rei = *db_rtp;
			fs << "<::uid>" << Rei.uid <<
				"<::Run>" << Rei.Run <<
				"<::FirstEvent>" << Rei.FirstEvent <<
				"<::LastEvent>" << Rei.LastEvent <<
				"<::TFEvent>" << Rei.TFEvent <<
				"<::TLEvent>" << Rei.TLEvent <<
				"<::Priority>" << Rei.Priority <<
				"<::FilePath>" << (const char*)Rei.FilePath <<
				"<::Status>" << Rei.Status <<
				"<::History>" << Rei.History <<
				"<::CounterFail>" << Rei.CounterFail <<
				"<::SubmitTime>" << Rei.SubmitTime <<
				"<::cuid>" << Rei.cuid <<
				"<::rndm1>" << Rei.rndm1 <<
				"<::rndm2>" << Rei.rndm2 <<
				"<::DataMC>" << Rei.DataMC <<
				"<::cinfo::Run>" << Rei.cinfo.Run <<
				"<::cinfo::EventsProcessed>" << Rei.cinfo.EventsProcessed <<
				"<::cinfo::LastEventProcessed>" << Rei.cinfo.LastEventProcessed <<
				"<::cinfo::CriticalErrorsFound>" << Rei.cinfo.CriticalErrorsFound <<
				"<::cinfo::ErrorsFound>" << Rei.cinfo.ErrorsFound <<
				"<::cinfo::CPUTimeSpent>" << Rei.cinfo.CPUTimeSpent <<
				"<::cinfo::TimeSpent>" << Rei.cinfo.TimeSpent <<
				"<::cinfo::Mips>" << Rei.cinfo.Mips <<
				"<::cinfo::CPUMipsTimeSpent>" << Rei.cinfo.CPUMipsTimeSpent <<
				"<::cinfo::Status>" << Rei.cinfo.Status <<
				"<::cinfo::HostName>" << (const char*)Rei.cinfo.HostName << endl;
		}
		fs << endl;
	}
	catch (string &ex) {
		cout<<"pRT::dbin_imex-E- " << ex << endl;
	}
}

void pRT::dbout_imex(ifstream &fs, bdbRC *db) {
	bdbRC *usedb(db?db:theDB);
	try {
		bdbRC db(usedb);
		db.connect2(DBO_RTB);
		rtb_dt db_rtp(db);
		db_rtp.erase();

		string s;
		DPS::Producer::RunEvInfo Rei;
		streampos last = fs.tellg();
		while (!std::getline(fs,s).eof()) {
			if (s.find("<<") == 0) {
				fs.seekg(last);
				return;
			} else {
				CharStringToken st(s);
				st.strtok2("<::");
				for (int i=0; i<st.tokens(); i++) {
					string ss = st.token(i);
					string ssv;
					if (ss.find('>') != string::npos)
						ssv = ss.substr(ss.find('>')+1);
					// insert parsing below
					if (ss.find("uid>") == 0)
						Rei.uid = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("Run>") == 0)
						Rei.Run = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("FirstEvent>") == 0)
						Rei.FirstEvent = boost::lexical_cast<long>(ssv);
					else if (ss.find("LastEvent>") == 0)
						Rei.LastEvent = boost::lexical_cast<long>(ssv);
					else if (ss.find("TFEvent>") == 0)
						Rei.TFEvent = boost::lexical_cast<time_t>(ssv);
					else if (ss.find("TLEvent>") == 0)
						Rei.TLEvent = boost::lexical_cast<time_t>(ssv);
					else if (ss.find("Priority>") == 0)
						Rei.Priority = boost::lexical_cast<long>(ssv);
					else if (ss.find("FilePath>") == 0)
						Rei.FilePath = ssv.c_str();
					else if (ss.find("Status>") == 0)
						Rei.Status = (DPS::Producer::RunStatus)boost::lexical_cast<int>(ssv);
					else if (ss.find("History>") == 0)
						Rei.History = (DPS::Producer::RunStatus)boost::lexical_cast<int>(ssv);
					else if (ss.find("CounterFail>") == 0)
						Rei.CounterFail = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("SubmitTime>") == 0)
						Rei.SubmitTime = boost::lexical_cast<time_t>(ssv);
					else if (ss.find("cuid>") == 0)
						Rei.cuid = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("rndm1>") == 0)
						Rei.rndm1 = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("rndm2>") == 0)
						Rei.rndm2 = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("DataMC>") == 0)
						Rei.DataMC = boost::lexical_cast<long>(ssv);
					else if (ss.find("cinfo::Run>") == 0)
						Rei.cinfo.Run = boost::lexical_cast<unsigned long>(ssv);
					else if (ss.find("cinfo::EventsProcessed>") == 0)
						Rei.cinfo.EventsProcessed = boost::lexical_cast<long>(ssv);
					else if (ss.find("cinfo::LastEventProcessed>") == 0)
						Rei.cinfo.LastEventProcessed = boost::lexical_cast<long>(ssv);
					else if (ss.find("cinfo::CriticalErrorsFound>") == 0)
						Rei.cinfo.CriticalErrorsFound = boost::lexical_cast<long>(ssv);
					else if (ss.find("cinfo::ErrorsFound>") == 0)
						Rei.cinfo.ErrorsFound = boost::lexical_cast<long>(ssv);
					else if (ss.find("cinfo::CPUTimeSpent>") == 0)
						Rei.cinfo.CPUTimeSpent = boost::lexical_cast<float>(ssv);
					else if (ss.find("cinfo::TimeSpent>") == 0)
						Rei.cinfo.TimeSpent = boost::lexical_cast<float>(ssv);
					else if (ss.find("cinfo::Mips>") == 0)
						Rei.cinfo.Mips = boost::lexical_cast<float>(ssv);
					else if (ss.find("cinfo::CPUMipsTimeSpent>") == 0)
						Rei.cinfo.CPUMipsTimeSpent = boost::lexical_cast<float>(ssv);
					else if (ss.find("cinfo::Status>") == 0)
						Rei.cinfo.Status = (DPS::Producer::RunStatus)boost::lexical_cast<int>(ssv);
					else if (ss.find("cinfo::HostName>") == 0)
						Rei.cinfo.HostName = ssv.c_str();
				}
				// insert into db
				if (st.tokens()) db_rtp.insert(Rei);
			}
			streampos last = fs.tellg();
		}
	}
	catch (string &ex) {
		cout<<"pRT::dbout_imex-E- " << ex << endl;
	}
    catch(boost::bad_lexical_cast &ex) {
    	cout<<"pRT::dbout_imex-E- boost::bad_lexical_cast ex"<<endl;
    }
}

//_____________________________________________________________________

ui::ui(): Ui_MainWindow()
{
}

void ui::setupUi(QMainWindow *MainWindow)
{
	Ui_MainWindow::setupUi(MainWindow);
	_mw = dynamic_cast<myQMainWindow *>(MainWindow);

	uistring title("AMS BDB quick Viewer (");
	title += common::user() + "@" + uistring(QHostInfo::localHostName()) + ":)";
	MainWindow->setWindowTitle(title.q_str());

	_tuTreeWidget2 = new treeUtils(treeWidget2);
	TreeItem *dbfl = _tuTreeWidget2->setItem("root",TRCOL_Name,new TreeItem(_tuTreeWidget2));
	treeWidget2->addTopLevelItem(dbfl);
	dbfl->setExpanded(true);

	TreeItem *serv = _tuTreeWidget2->setItem("Server",TRCOL_Name,new TreeItem(_tuTreeWidget2));
	dbfl->insertChild(0,serv);
	TreeItem *form = _tuTreeWidget2->setItem("Nominal Client",TRCOL_Name,new TreeItem(_tuTreeWidget2));
	serv->insertChild(0,form);
	form = _tuTreeWidget2->setItem("Nominal Host",TRCOL_Name,new TreeItem(_tuTreeWidget2));
	serv->insertChild(0,form);
	form = _tuTreeWidget2->setItem("Nominal Killer",TRCOL_Name,new TreeItem(_tuTreeWidget2));
	serv->insertChild(0,form);
	form = _tuTreeWidget2->setItem("Set Environment",TRCOL_Name,new TreeItem(_tuTreeWidget2));
	serv->insertChild(0,form);

	TreeItem *prod = _tuTreeWidget2->setItem("Producer",TRCOL_Name,new TreeItem(_tuTreeWidget2));
	dbfl->insertChild(0,prod);
	form = _tuTreeWidget2->setItem("Nominal Client",TRCOL_Name,new TreeItem(_tuTreeWidget2));
	prod->insertChild(0,form);
	form = _tuTreeWidget2->setItem("Active Client",TRCOL_Name,new TreeItem(_tuTreeWidget2));
	prod->insertChild(0,form);
	form = _tuTreeWidget2->setItem("Nominal Host",TRCOL_Name,new TreeItem(_tuTreeWidget2));
	prod->insertChild(0,form);
	form = _tuTreeWidget2->setItem("Nominal NTuple",TRCOL_Name,new TreeItem(_tuTreeWidget2));
	prod->insertChild(0,form);
	form = _tuTreeWidget2->setItem("Produced NTuple",TRCOL_Name,new TreeItem(_tuTreeWidget2));
	prod->insertChild(0,form);
	form = _tuTreeWidget2->setItem("Run Table",TRCOL_Name,new TreeItem(_tuTreeWidget2));
	prod->insertChild(0,form);

	_map_ctrl_forms["Server/Nominal Client"] = pair<tabUtils *, MCFids>(new sNC(qtw_sNC),MCFids(1,DBO_NSL));
	_map_ctrl_forms["Server/Nominal Host"] = pair<tabUtils *, MCFids>(new sNH(qtw_sNH),MCFids(2,DBO_NHL));
	_map_ctrl_forms["Server/Nominal Killer"] = pair<tabUtils *, MCFids>(new sNK(qtw_sNK),MCFids(3,DBO_NKL));
	_map_ctrl_forms["Server/Set Environment"] = pair<tabUtils *, MCFids>(new sSE(qtw_sSE),MCFids(4,DBO_ENV));
	_map_ctrl_forms["Producer/Nominal Client"] = pair<tabUtils *, MCFids>(new pNC(qtw_pNC),MCFids(5,DBO_NCL));
	_map_ctrl_forms["Producer/Active Client"] = pair<tabUtils *, MCFids>(new pAC(qtw_pAC),MCFids(6,DBO_ACL));
	_map_ctrl_forms["Producer/Nominal Host"] = pair<tabUtils *, MCFids>(new pNH(qtw_pNH),MCFids(7,DBO_AHLP));
	_map_ctrl_forms["Producer/Nominal NTuple"] = pair<tabUtils *, MCFids>(new pNNT(qtw_pNNT),MCFids(8,DBO_DSTI));
	_map_ctrl_forms["Producer/Produced NTuple"] = pair<tabUtils *, MCFids>(new pPNT(qtw_pPNT),MCFids(9,DBO_DSTS));
	_map_ctrl_forms["Producer/Run Table"] = pair<tabUtils *, MCFids>(new pRT(qtw_pRT),MCFids(10,DBO_RTB));

	for (MapCtrlForms::iterator it=_map_ctrl_forms.begin(); it != _map_ctrl_forms.end(); ++it)
		(*it->second.first)->setColumnHidden(0,true);

	QObject::connect(actOpen, SIGNAL(triggered()), this, SLOT(mnuOpen()));
	QObject::connect(actNew, SIGNAL(triggered()), this, SLOT(mnuNew()));
	QObject::connect(actNew2, SIGNAL(triggered()), this, SLOT(mnuNew2()));
	QObject::connect(actRecover, SIGNAL(triggered()), this, SLOT(mnuRecover()));
	QObject::connect(actUpdateStatus, SIGNAL(triggered()), this, SLOT(mnuUpdate()));
	QObject::connect(actCommitStatus,SIGNAL(triggered()),this, SLOT(mnuCommit()));
	QObject::connect(actQuit, SIGNAL(triggered()), this, SLOT(mnuQuit()));
	QObject::connect(actZoomIn, SIGNAL(triggered()), this, SLOT(mnuZoomIn()));
	QObject::connect(actZoomOut, SIGNAL(triggered()), this, SLOT(mnuZoomOut()));
	QObject::connect(actExportDB, SIGNAL(triggered()), this, SLOT(mnuExportDB()));
	QObject::connect(actImportDB, SIGNAL(triggered()), this, SLOT(mnuImportDB()));

	QObject::connect(theUI.pbtRemove,SIGNAL(clicked(bool)),this, SLOT(clkbtRemove(bool)));
	QObject::connect(theUI.pbtNew,SIGNAL(clicked(bool)),this, SLOT(clkbtNew(bool)));
	QObject::connect(theUI.pbtMoveUp,SIGNAL(clicked(bool)),this, SLOT(clkbtMoveUp(bool)));
	QObject::connect(theUI.pbtMoveDown,SIGNAL(clicked(bool)),this, SLOT(clkbtMoveDown(bool)));

	actUpdateStatus->setEnabled(false);
	actCommitStatus->setEnabled(false);
	actUseFilter->setEnabled(false);
	actExportDB->setEnabled(false);
	actSetFilter->setEnabled(false);
}

void ui::kickev(uiEvent::actType type, void *option)
{
	uiEvent *pEv = new uiEvent(type,QEvent::User,option);
	QApplication::postEvent(reinterpret_cast<QObject *>(this),pEv);
}

tabUtils *ui::getVisibleCtrlTab() const
{
	MapCtrlForms::const_iterator cit;
	for (cit=_map_ctrl_forms.begin(); cit != _map_ctrl_forms.end(); ++cit) {
		tabUtils *tabu = cit->second.first;
		if ((*tabu)->isVisible())
			return tabu;
	}
	TreeItem *current = dynamic_cast<TreeItem *>(treeWidget2->currentItem());
	if (current && current->isSelected()) {
		uistring path;
		while (current) {
			uistring s = current->text(0);
			if (s != "root")
				path = s + uistring(path.length()?"/":"") + path;
			current = dynamic_cast<TreeItem *>(current->parent());
		}
		if ((cit=_map_ctrl_forms.find(path)) != _map_ctrl_forms.end())
			return cit->second.first;
	}
	return 0;
}

time_t ui::stringTime(const string t)
{
	time_t result = 0;
	CharStringToken st(t," \t");
	if (st.tokens()<2)
		return result;
	CharStringToken st2(st.token(0),".");
	CharStringToken st3(st.token(1),":");
	try {
		struct tm tm;
		if (st2.tokens()>2) {
			tm.tm_year = boost::lexical_cast<int>(st2.token(0))-1900;
			tm.tm_mon = boost::lexical_cast<int>(st2.token(1))-1;
			tm.tm_mday = boost::lexical_cast<int>(st2.token(2));
		}
		if (st3.tokens()>2) {
			tm.tm_hour = boost::lexical_cast<int>(st3.token(0));
			tm.tm_min = boost::lexical_cast<int>(st3.token(1));
			tm.tm_sec = boost::lexical_cast<int>(st3.token(2));
		}
		result = mktime(&tm);
	}
	catch(string &ex) {
	}
	return result;
}

void ui::mnuOpen()
{
	myQFileDialog dialog;
	const char *ptitle = "Open c++ BDB";
	dialog.setWindowTitle(QApplication::translate(ptitle,ptitle,0,QApplication::UnicodeUTF8));
	if (!dialog.exec()) {
		cout<<"ui::mnuOpen-E- failed"<<endl;
		return;
	}
	QStringList files = dialog.selectedFiles();
	uistring file = files.first().toStdString();
	readDB(file.c_str());
}

void ui::mnuNew() {
	QStringList files;
	QFileDialog dialog(_mw);
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setFileMode(QFileDialog::AnyFile);
	const char *ptitle = "New c++ BDB";
	dialog.setWindowTitle(QApplication::translate(ptitle,ptitle,0,QApplication::UnicodeUTF8));
	if (!dialog.exec()) {
		cout<<"ui::mnuNew-E- failed"<<endl;
		return;
	}
	files = dialog.selectedFiles();
	uistring file = files.first().toStdString();
	createDB(file.c_str());
}

void ui::mnuNew2() {
	QStringList files;
	QFileDialog dialog(_mw);
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setFileMode(QFileDialog::Directory);
	const char *ptitle = "New c++ BDB v.2";
	dialog.setWindowTitle(QApplication::translate(ptitle,ptitle,0,QApplication::UnicodeUTF8));
	if (!dialog.exec()) {
		cout<<"ui::mnuNew2-E- failed"<<endl;
		return;
	}
	files = dialog.selectedFiles();
	uistring file = files.first().toStdString();
	createDB(file.c_str());
}

void ui::mnuRecover() {
	QStringList files;
	QFileDialog dialog(_mw);
	dialog.setFileMode(QFileDialog::DirectoryOnly);
	dialog.setViewMode(QFileDialog::Detail);
	const char *ptitle = "Recover c++ BDB";
	dialog.setWindowTitle(QApplication::translate(ptitle,ptitle,0,QApplication::UnicodeUTF8));
	if (!dialog.exec()) {
		cout<<"ui::mnuRecover-E- failed"<<endl;
		return;
	}
	files = dialog.selectedFiles();
	uistring file = files.first().toStdString();
	recoverDB(file.c_str());
}

void ui::mnuUpdate() {
	tabUtils *tabu = getVisibleCtrlTab();
	if (tabu) {
		tabu->rmRow();
		tabu->dbin();
	}
}

void ui::mnuCommit() {
	tabUtils *tabu = getVisibleCtrlTab();
/*!!!!! version 2 tuning
double total=0;
for(int i=0; i<1000; i++) {
  timespec start_time = hrtime(CLOCK_REALTIME);*/
	if (tabu) tabu->dbout();
/*!!!!! version 2 tuning
  timespec end_time = hrtime(CLOCK_REALTIME);
  total += static_cast<double>((end_time.tv_sec-start_time.tv_sec)*1e9 + (end_time.tv_nsec-start_time.tv_nsec))/1e6;
}
cout<<"average time = "<<boost::format("%.6d") %(total/1000)<<" msec"<<endl;*/
	mnuUpdate();
}

void ui::mnuQuit() { exit(0); };

void ui::mnuZoomIn() {
	zoom(ZOOM_In,_tuTreeWidget2);
	MapCtrlForms::const_iterator cit;
	for (cit=_map_ctrl_forms.begin(); cit != _map_ctrl_forms.end(); ++cit) {
		tabUtils *tabu = cit->second.first;
		tabu->_hrState = 0;
		zoom(ZOOM_In,tabu);
	}
}

void ui::mnuZoomOut() {
	zoom(ZOOM_Out,_tuTreeWidget2);
	MapCtrlForms::const_iterator cit;
	for (cit=_map_ctrl_forms.begin(); cit != _map_ctrl_forms.end(); ++cit) {
		tabUtils *tabu = cit->second.first;
		tabu->_hrState = 0;
		zoom(ZOOM_Out,tabu);
	}
}

void ui::mnuExportDB()
{
	QStringList files;
	QFileDialog dialog(_mw);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setViewMode(QFileDialog::Detail);
	const char *ptitle = "Export BDB";
	dialog.setWindowTitle(QApplication::translate(ptitle,ptitle,0,QApplication::UnicodeUTF8));
	if (!dialog.exec()) {
		cout<<"ui::mnuExportDB-E- failed"<<endl;
		return;
	}
	files = dialog.selectedFiles();
	uistring file = files.first().toStdString();

	ofstream fsexp;
	fsexp.open(file.c_str(),fstream::out|fstream::trunc);
	if (!fsexp.is_open()) {
    	cout<<"ui::mnuExportDB-E- "<<file<<" open error"<<endl;
    	return;
	}
	MapCtrlForms::const_iterator cit;
	for (cit=_map_ctrl_forms.begin(); cit!=_map_ctrl_forms.end(); ++cit)
		cit->second.first->dbin_imex(fsexp);
	fsexp.close();

	for (MapCtrlForms::iterator it=_map_ctrl_forms.begin(); it!=_map_ctrl_forms.end(); ++it) {
		uistring sform = it->first;
		try {
			tabUtils *tab = _map_ctrl_forms[sform.c_str()].first;
			tab->rmRow();
	    	tab->dbin();
			tab->hresize();
			(*tab)->update();
		}
		catch (string &ex) {
			cout<<"ui::mnuExportD-E- "<<sform<<", "<<ex<<endl;
		}
	}
}

void ui::mnuImportDB()
{
	QStringList files;
	QFileDialog dialog(_mw);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setViewMode(QFileDialog::Detail);
	const char *ptitle = "Import BDB";
	dialog.setWindowTitle(QApplication::translate(ptitle,ptitle,0,QApplication::UnicodeUTF8));
	if (!dialog.exec()) {
		cout<<"ui::mnuImportDB-E- failed"<<endl;
		return;
	}
	files = dialog.selectedFiles();
	uistring file = files.first().toStdString();

	ifstream fsimp(file.c_str(),fstream::in);
	if (!fsimp.is_open()) {
		cout<<"ui::mnuImportDB-E- "<<file<<" open error"<<endl;
		return;
	}
	string s;
	dbTables type;
	MapCtrlForms::const_iterator cit = _map_ctrl_forms.end();
	while (!std::getline(fsimp,s).eof()) {
		if (s.find("<<") == 0) {	// new bdb table found
			type = (dbTables)boost::lexical_cast<int>(s.substr(2,s.find(">>")-2));
			for (cit=_map_ctrl_forms.begin(); cit!=_map_ctrl_forms.end(); ++cit)
				if (type == cit->second.second.dbtyp) {
					cit->second.first->dbout_imex(fsimp);
					break;
				}
		}
	}
	fsimp.close();
}

void ui::clkTreeWidget2(QTreeWidgetItem *item, int column)
{
	if (!item->childCount()) {
		uistring skey;
		for (QTreeWidgetItem *pitem=item; pitem->parent(); pitem = pitem->parent()) {
			if (skey.length()) skey = "/" + skey;
			skey = uistring(pitem->text(0)) + skey;
		}
		stackedWidget2->setCurrentIndex(_map_ctrl_forms[skey.c_str()].second.stwid);

		theUI.actUpdateStatus->setEnabled(true);
		theUI.actCommitStatus->setEnabled(true);
		theUI.pbtNew->setEnabled(true);
		tabUtils *tab = _map_ctrl_forms[skey.c_str()].first;
		emit tab->clkSelectionChanged();
	}
	else {
		stackedWidget2->setCurrentIndex(0);

		theUI.actUpdateStatus->setEnabled(false);
		theUI.actCommitStatus->setEnabled(false);
		theUI.pbtRemove->setEnabled(false);
		theUI.pbtNew->setEnabled(false);
		theUI.pbtMoveUp->setEnabled(false);
		theUI.pbtMoveDown->setEnabled(false);
	}
}

struct range_top {
	bool operator()(const QTableWidgetSelectionRange &ita, const QTableWidgetSelectionRange &itb) {
		return ita.topRow() < itb.topRow();
	}
} range_top;
struct range_top_r {
	bool operator()(const QTableWidgetSelectionRange &ita, const QTableWidgetSelectionRange &itb) {
		return itb.topRow() < ita.topRow();
	}
} range_top_r;

void ui::clkbtNew(bool checked)
{
	tabUtils *tab = getVisibleCtrlTab();
	if (tab) tab->addRow();
}

void ui::clkbtRemove(bool checked)
{
	tabUtils *tab = getVisibleCtrlTab();
	if (!tab) return;
	QList<QTableWidgetSelectionRange> sl=(*tab)->selectedRanges();
	qSort(sl.begin(),sl.end(),range_top_r);
	if (sl.count()<=0) return;

	for (int i=0,l=sl.count(); i<l; i++) {
		QTableWidgetSelectionRange sr=sl[i];
		for (int j=sr.bottomRow(); j>=sr.topRow(); j--)
			(*tab)->removeRow(j);
	}
}

void ui::clkbtMoveUp(bool checked)
{
	tabUtils *tab = getVisibleCtrlTab();
	if (!tab) return;
	QList<QTableWidgetSelectionRange> sl=(*tab)->selectedRanges();
	qSort(sl.begin(),sl.end(),range_top);
	if (sl.count()<=0 || !sl[0].topRow()) return;

	for (int i=0,l=sl.count(); i<l; i++) {
		QTableWidgetSelectionRange sr=sl[i];
		for (int j=sr.topRow(); j<=sr.bottomRow(); j++) {
			(*tab)->insertRow(j+1);
			for (int k=0; k<(*tab)->columnCount(); k++)
				(*tab)->setItem(j+1,k,(*tab)->takeItem(j-1,k));
			(*tab)->removeRow(j-1);
		}
	}
}

void ui::clkbtMoveDown(bool checked)
{
	tabUtils *tab = getVisibleCtrlTab();
	if (!tab) return;
	QList<QTableWidgetSelectionRange> sl=(*tab)->selectedRanges();
	qSort(sl.begin(),sl.end(),range_top_r);
	if (sl.count()<=0 || sl[0].bottomRow()==(*tab)->rowCount()-1) return;

	for (int i=0,l=sl.count(); i<l; i++) {
		QTableWidgetSelectionRange sr=sl[i];
		for (int j=sr.bottomRow(); j>=sr.topRow(); j--) {
			(*tab)->insertRow(j);
			for (int k=0; k<(*tab)->columnCount(); k++)
				(*tab)->setItem(j,k,(*tab)->takeItem(j+2,k));
			(*tab)->removeRow(j+2);
		}
	}
}

//_____________________________________________________________________
// class myQMainWindow

bool qtabUtils::eventFilter(QObject *obj, QEvent *event)
{
	bool ctrl_pressed = (QApplication::keyboardModifiers() & Qt::ControlModifier);
	bool alt_pressed = (QApplication::keyboardModifiers() & Qt::AltModifier);
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
			switch (key) {
			case Qt::Key_Plus:
			case Qt::Key_Equal:
				emit theUI.mnuZoomIn();
				break;
			case Qt::Key_Minus:
				emit theUI.mnuZoomOut();
				break;
			case Qt::Key_R:
				emit theUI.clkbtRemove(false);
				break;
			case Qt::Key_N:
				emit theUI.clkbtNew(false);
				break;
			}
		}
		else if (_alt_pressed) {
			switch (key) {
			case Qt::Key_U:
				emit theUI.mnuUpdate();
				break;
			case Qt::Key_C:
				emit theUI.mnuCommit();
				break;
			case Qt::Key_Q:
				emit theUI.mnuQuit();
				break;
			}
		}
	}
	else if (event->type() == QEvent::KeyRelease)
	{
	}
	else if (event->type() == QEvent::ContextMenu)
	{
	}
	return QMainWindow::event(event);
}

//_____________________________________________________________________
// class myQFileDialog

myQFileDialog::myQFileDialog(): QFileDialog() {
	setOption(QFileDialog::DontUseNativeDialog,true);
	setViewMode(QFileDialog::Detail);
	connect(this,SIGNAL(directoryEntered(const QString &)),this,SLOT(sig_dirEntered(const QString &)));
    connect(this,SIGNAL(currentChanged(const QString &)),this,SLOT(sig_curChanged(const QString &)));
	setFileMode(QFileDialog::Directory);
    emit directoryEntered(directory().absolutePath());
}
myQFileDialog::~myQFileDialog() {
}
void myQFileDialog::sig_dirEntered(const QString &dir) {
	setFileMode(QFileDialog::Directory);
}
void myQFileDialog::sig_curChanged(const QString &path) {
	boost::filesystem::path ipath(uistring(path).c_str());
	if (boost::filesystem::is_directory(ipath)) setFileMode(QFileDialog::Directory);
	  else setFileMode(QFileDialog::ExistingFile);
}
