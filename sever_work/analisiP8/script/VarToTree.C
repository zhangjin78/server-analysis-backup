
TTree *tVar=NULL;
//--header
bool isreal;
int TriggerPeriod;
unsigned int run;
unsigned int event;
//--Particle
int ntrack;
//--Track
float tk_pos[9][3];	//reconstructed tracker position
float tk_dir[9][3];	//reconstructed tracker direction
float tk_ql2[9][2];	//tracker XQ & YQ
float tk_ql[9];		//selection on tk_ql[0] tk_ql[8]:L1 L9
int tk_qls[9];		//Q status for each layer
int tk_lid[9];		//tk ladder id
float tk_q[2];		//1:inner, Z-0.45< q_inn < Z+0.45; 0: all
float tk_qrms[2];		//1:inner rms, q_inn_rms<0.55; 0: all
float tk_qin[2][3];
float tk_qrmn[2][3];
float tk_qln[2][9][3]; //[version:0=YJ,1=HL][ilay][pattern:0=X,1=Y,2=X+Y]
float tk_chis1[3][3][7][3];////Algo+PGCIEMAT+Span+XYXY
float tk_rig[3]; //StoreTree, store only 3 patterns rigidity
//--TOF
float tof_betah;
float tof_betahmc;
int tof_hsumh;
int tof_hsumhu;
int tof_qs;
float tof_ql[4];
int tof_barid[4];
//------secondary tracks cuts
int betah2hb[2];
float betah2q;
float betah2r;
//--L1L9Particle
float tk_l1q;
float tk_l9q;
int tk_l1qs;
int tk_l9qs;
int ibetahs;
int itrdtracks;
int tof_nhits;
int tof_hsumhus;
float betahs;
float tk_l1qvs;
float tk_l9qvs;
float tk_pos1s[9][3];
float tk_dir1s[3];
float tk_l1qxy[2];
float tk_l9qxy[2];
int tk_exqls[2];
float tk_exql[2][3];
float tk_exqvn[2][2];//YJ+HL, unbiased Tracker Q Correct by unbiased Beta track
float tk_exqln[2][2][3];//YJ+LU, unbiased Tracker Q Correct by Inner Tracker Track
//--MC
float mcweight[3];
float mmom;
float mch;
int mpar;
int mtof_pass;
float mevcoo[3];
float mevdir[3];
float mevcoo1[21][3]; //2019.10.13
float mevmom1[21]; //2021.09.06
//MC truth of secondary, 0: maximum momentum secondary, 1: maximum charge secondary //2019.10.30
float mpare[2];	//momentum
int mparp[2];	//particle id
float mparc[2];	//charge
//TrMCCluster with maximum momentum for each tracker layer //2019.11.06
float mtrmom[9];	//maximum momentum
int mtrpar[9];	//particle id
float mtrcoo[9][3];	//step start point
float mtrcoo1[9][3];	//mid point 
int mtrpri[9];	//is primary
int mtrz[9];	//integral charge in binary, retrieve by mtrz[il]&0x3f, see TrMCCluster::getZ()
//--Selection for ChargeTemp
bool selFrag;
bool selLTOFodd;
bool selTOFS0;
bool selTOFS2;
bool selInner;
bool selUTOF;
bool selLTOF;
bool selL9;
bool FragAboveL1;
	

bool BookVarTree(TObjArray &hman1, SelEvent &ev, TFile &outfile, int QVer)
{
    string sDir = Form("L1L2Sur%d", QVer);
    outfile.mkdir(Form("%s", sDir.c_str()));

    for(int ir = 1; ir < 2; ir ++){
        for(it=viQSel.begin(); it!=viQSel.end(); it++){
            if (!ev.isreal && *it!=ev.mch) continue;

            histn = Form("%s/q%d/%s/TreeVar", sDir.c_str(), *it, cSpan[ir]);
            outfile.mkdir(histn.c_str());
            outfile.cd(histn.c_str());

            histn = Form("Tree_r%d_q%d", ir, *it);
            tVar = new TTree(histn.c_str(), histn.c_str());

            //--header
            tVar->Branch("isreal", &isreal, "isreal/O");
            tVar->Branch("TriggerPeriod", &TriggerPeriod, "TriggerPeriod/I");
            tVar->Branch("run", &run, "run/i");
            tVar->Branch("event", &event, "event/i");

            //--Particle
            tVar->Branch("ntrack", &ntrack, "ntrack/I");

            //--Track
            tVar->Branch("tk_pos",        tk_pos,         "tk_pos[9][3]/F");
            tVar->Branch("tk_dir",        tk_dir,         "tk_dir[9][3]/F");
            tVar->Branch("tk_chis1",      tk_chis1,       "tk_chis1[3][3][7][3]/F");
            tVar->Branch("tk_q",          tk_q,           "tk_q[2]/F");//All+Inner
            tVar->Branch("tk_qrms",       tk_qrms,        "tk_qrms[2]/F");
            tVar->Branch("tk_ql",         tk_ql,          "tk_ql[9]/F");//Temp Only Layer1+2
            tVar->Branch("tk_qls",        tk_qls,         "tk_qls[9]/I");
            tVar->Branch("tk_lid",        tk_lid,         "tk_lid[9]/I");
            tVar->Branch("tk_ql2",        tk_ql2,         "tk_ql2[9][2]/F");
            tVar->Branch("tk_rig",        tk_rig,         "tk_rig[3]/F");
            tVar->Branch("tk_qin",        tk_qin,         "tk_qin[2][3]/F");
            tVar->Branch("tk_qrmn",       tk_qrmn,        "tk_qrmn[2][3]/F");
            tVar->Branch("tk_qln",        tk_qln,         "tk_qln[2][9][3]/F");
            
            //--TOF
            tVar->Branch("tof_betah",   &tof_betah,   "tof_betah/F");
            tVar->Branch("tof_betahmc", &tof_betahmc, "tof_betahmc/F");
            tVar->Branch("tof_hsumh",   &tof_hsumh,   "tof_hsumh/I"); //BetaH
            tVar->Branch("tof_hsumhu",  &tof_hsumhu,  "tof_hsumhu/I");
            tVar->Branch("tof_ql",       tof_ql,      "tof_ql[4]/F");
            tVar->Branch("tof_qs"  ,    &tof_qs,      "tof_qs/I");//Q-Status
            tVar->Branch("tof_barid",    tof_barid,   "tof_barid[4]/I");
            
            //--secondary tracks cuts
            tVar->Branch("betah2hb",    betah2hb,   "betah2hb[2]/I");
            tVar->Branch("betah2q",    &betah2q,    "betah2q/F");
            tVar->Branch("betah2r",    &betah2r,    "betah2r/F");
            
            //--L1L9Particle
            tVar->Branch("tk_l1q",     &tk_l1q,     "tk_l1q/F");
            tVar->Branch("tk_l9q",     &tk_l9q,     "tk_l9q/F");
            tVar->Branch("tk_l1qxy",   tk_l1qxy,    "tk_l1qxy[2]/F");
            tVar->Branch("tk_l9qxy",   tk_l9qxy,    "tk_l9qxy[2]/F");
            tVar->Branch("tk_l1qs",    &tk_l1qs,    "tk_l1qs/I");
            tVar->Branch("tk_l9qs",    &tk_l9qs,    "tk_l9qs/I");
            tVar->Branch("ibetahs",    &ibetahs,    "ibetahs/I");
            tVar->Branch("itrdtracks", &itrdtracks, "itrdtracks/I");
            tVar->Branch("tof_nhits",  &tof_nhits,  "tof_nhits/I");
            tVar->Branch("tof_hsumhus",&tof_hsumhus,"tof_hsumhus/I");
            tVar->Branch("betahs",     &betahs,     "betahs/F");
            tVar->Branch("tk_pos1s",    tk_pos1s,   "tk_pos1s[9][3]/F");
            tVar->Branch("tk_dir1s",    tk_dir1s,   "tk_dir1s[3]/F");
            tVar->Branch("tk_l1qvs",   &tk_l1qvs,   "tk_l1qvs/F");
            tVar->Branch("tk_l9qvs",   &tk_l9qvs,   "tk_l9qvs/F");
            tVar->Branch("tk_exql",     tk_exql,    "tk_exql[2][3]/F");
            tVar->Branch("tk_exqls",    tk_exqls,   "tk_exqls[2]/I");//TK-QL Status
            tVar->Branch("tk_exqln",    tk_exqln,   "tk_exqln[2][2][3]/F");
            tVar->Branch("tk_exqvn",    tk_exqvn,   "tk_exqvn[2][2]/F");
            
            //--MC
            tVar->Branch("mcweight",    mcweight,   "mcweight[3]/F");
            tVar->Branch("mpar",     &mpar,     "mpar/I");
            tVar->Branch("mmom",     &mmom,     "mmom/F");
            tVar->Branch("mch",      &mch,      "mch/F");
            tVar->Branch("mtof_pass",&mtof_pass,"mtof_pass/I");
            tVar->Branch("mevcoo",    mevcoo,   "mevcoo[3]/F");
            tVar->Branch("mevdir",    mevdir,   "mevdir[3]/F");
            tVar->Branch("mevcoo1",    mevcoo1,   "mevcoo1[21][3]/F");
            tVar->Branch("mevmom1",    mevmom1,   "mevmom1[21]/F");
            tVar->Branch("mpare",     mpare,    "mpare[2]/F");
            tVar->Branch("mparp",     mparp,    "mparp[2]/I");
            tVar->Branch("mparc",     mparc,    "mparc[2]/F");
            tVar->Branch("mtrmom",    mtrmom,   "mtrmom[9]/F");
            tVar->Branch("mtrpar",    mtrpar,   "mtrpar[9]/I");
            tVar->Branch("mtrcoo",    mtrcoo,   "mtrcoo[9][3]/F");
            tVar->Branch("mtrcoo1",   mtrcoo1,  "mtrcoo1[9][3]/F");
            tVar->Branch("mtrpri",    mtrpri,   "mtrpri[9]/I");
            tVar->Branch("mtrz",      mtrz,     "mtrz[9]/I");

            //-- Selectin 
            tVar->Branch("selFrag", &selFrag, "selFrag/O");
            tVar->Branch("selLTOFodd", &selLTOFodd, "selLTOFodd/O");
            tVar->Branch("selTOFS0", &selTOFS0, "selTOFS0/O");
            tVar->Branch("selTOFS2", &selTOFS2, "selTOFS2/O");
            tVar->Branch("selInner", &selInner, "selInner/O");
            tVar->Branch("selUTOF", &selUTOF, "selUTOF/O");
            tVar->Branch("selLTOF", &selLTOF, "selLTOF/O");
            tVar->Branch("selL9", &selL9, "selL9/O");
            tVar->Branch("FragAboveL1", &FragAboveL1, "FragAboveL1/O");
        } // it
    } //ir

    return true;
}

bool SelVarTree(TObjArray &hman1, SelEvent &ev, TFile &outfile, int ich, int QVer, const double ww[3])
{
    string sDir = Form("L1L2Sur%d", QVer);

    //--header
    isreal = ev.isreal;
    TriggerPeriod = ev.TriggerPeriod();
    run = ev.run;
    event = ev.event;

    //--Particle
    ntrack = ev.ntrack;
    
    //--Track
    for(int il = 0; il < 9; il++){
        for(int ix = 0; ix < 3; ix++){
            tk_pos[il][ix] = ev.tk_pos[il][ix];
            tk_dir[il][ix] = ev.tk_dir[il][ix];
        }
        for(int ix = 0; ix < 2; ix++) tk_ql2[il][ix] = ev.tk_ql2[il][ix];
        tk_ql[il] = ev.tk_ql[il];
        tk_qls[il] = ev.tk_qls[il];
        tk_lid[il] = ev.tk_lid[il];
    }
    for(int isp = 0; isp < 2; isp++){
        tk_q[isp] = ev.tk_q[isp];
        tk_qrms[isp] = ev.tk_qrms[isp];
        for(int ix = 0; ix < 3; ix++){
            tk_qin[isp][ix] = ev.tk_qin[isp][ix];
            tk_qrmn[isp][ix] = ev.tk_qrmn[isp][ix];
        }
        for(int il = 0; il < 9; il++)
            for(int ix = 0; ix < 3; ix++) tk_qln[isp][il][ix] = ev.tk_qln[isp][il][ix];
    }
    for(int ial = 0; ial < 3; ial++){
        for(int ipg = 0; ipg < 3; ipg++)
            for(int isp = 0; isp < 7; isp++)
                for(int ix = 0; ix < 3; ix++) tk_chis1[ial][ipg][isp][ix] = ev.tk_chis1[ial][ipg][isp][ix];
    }
    for(int isp = 0; isp < 3; isp++) tk_rig[isp] = ev.GetRigidity(isp);

    //--TOF
    tof_betah = ev.tof_betah;
    tof_betahmc = ev.tof_betahmc;
    tof_hsumh = ev.tof_hsumh;
    tof_hsumhu = ev.tof_hsumhu;
    tof_qs = ev.tof_qs;
    for(int il = 0; il < 4; il++){
        tof_ql[il] = ev.tof_ql[il];
        tof_barid[il] = ev.tof_barid[il];
    }
    for(int i = 0; i < 2; i++) betah2hb[i] = ev.betah2hb[i];
    betah2q = ev.betah2q;
    betah2r = ev.betah2r;

    //--L1L9Particle
    tk_l1q = ev.tk_l1q;
    tk_l9q = ev.tk_l9q;
    tk_l1qs = ev.tk_l1qs;
    tk_l9qs = ev.tk_l9qs;
    ibetahs = ev.ibetahs;
    itrdtracks = ev.itrdtracks;
    tof_nhits = ev.tof_nhits;
    tof_hsumhus = ev.tof_hsumhus;
    betahs = ev.betahs;
    tk_l1qvs = ev.tk_l1qvs;
    tk_l9qvs = ev.tk_l9qvs;
    for(int ix = 0; ix < 3; ix++){
        tk_dir1s[ix] = ev.tk_dir1s[ix];
        for(int il = 0; il < 9; il++) tk_pos1s[il][ix] = ev.tk_pos1s[il][ix];
    }
    for(int iel = 0; iel < 2; iel++){
        tk_l1qxy[iel] = ev.tk_l1qxy[iel];
        tk_l9qxy[iel] = ev.tk_l9qxy[iel];
        tk_exqls[iel] = ev.tk_exqls[iel];
        for(int ix = 0; ix < 3; ix++) tk_exql[iel][ix] = ev.tk_exql[iel][ix];
        for(int ia = 0;ia <2; ia++){
            tk_exqvn[iel][ia] = tk_exqvn[iel][ia];
            for(int ix = 0; ix < 3; ix++) tk_exqln[iel][ia][ix] = ev.tk_exqln[iel][ia][ix];
        }
    }

    //--MC
    mmom = ev.mmom;
    mch = ev.mch;
    mpar = ev.mpar;
    mtof_pass = ev.mtof_pass;
    for(int i = 0; i < 3; i++){
        mcweight[i] = ww0[i];
        mevcoo[i] = ev.mevcoo[i];
        mevdir[i] = ev.mevdir[i];
    }
    for(int iml = 0; iml < 21; iml++){
        mevmom1[iml] = ev.mevmom1[iml];
        for(int ix = 0; ix < 3; ix++) mevcoo1[iml][ix] = ev.mevcoo1[iml][ix];
    }
    for(int i = 0; i < 2; i++){
        mpare[i] = ev.mpare[i];
        mparp[i] = ev.mparp[i];
        mparc[i] = ev.mparc[i];
    }
    for(int il = 0; il < 9; il++){
        mtrmom[il] = ev.mtrmom[il];
        mtrpar[il] = ev.mtrpar[il];
        mtrpri[il] = ev.mtrpri[il];
        mtrz[il] = ev.mtrz[il];
        for(int ix = 0; ix < 3; ix++){
            mtrcoo[il][ix] = ev.mtrcoo[il][ix];
            mtrcoo1[il][ix] = ev.mtrcoo1[il][ix];
        }
    }

    //--Selection for charge Temp
    selFrag = (ev.ntrack==1 || !ev.Select_Tk2ndRecon());
    selLTOFodd = true;
    if(ich%2==1 || ich>20){
        if(ich%2==1) selLTOFodd = selLTOFodd && (fabs(ev.GetTOFUDQ(1)-ich)<0.5);
        else selLTOFodd = selLTOFodd && ev.GetTOFUDQ(1)>ich-0.5 && ev.GetTOFUDQ(1)<ich+1.5;
        selLTOFodd = selLTOFodd && (fabs(ev.GetTofLQ(2)-ev.GetTofLQ(3))<fabs(ich-ev.GetQSelRange(0, ich, 1, QVer)));
        selLTOFodd = selLTOFodd && ev.IsGoodTofQUD(0) && ev.IsGoodTofQUD(1);
    }
    selTOFS2 = (fabs(ev.GetTofLQ(2)-ev.GetTofLQ(2+1))<fabs(ich-ev.GetQSelRange(0, ich, 1, QVer)));
    selTOFS0 = (fabs(ev.GetTofLQ(0)-ev.GetTofLQ(0+1))<fabs(ich-ev.GetQSelRange(0, ich, 1, QVer)));
    selInner = (fabs(ev.GetSubDetQ(0,QVer)-ich)<0.5);
    selUTOF = (fabs(ev.GetTOFUDQ(0)-ich)<0.5);
    selLTOF = true;
    if(ich%2==1)  selLTOF = selLTOF && (fabs(ev.GetTOFUDQ(1)-ich)<0.5);
    else selLTOF = selLTOF && ev.GetTOFUDQ(1)>ich-0.5 && ev.GetTOFUDQ(1)<ich+1.5;
    selL9 = (fabs(ev.GetSubDetQ(5,QVer)-ich)<0.5);

    //-- Selection for MCtruth
    FragAboveL1 = !ev.PrimaryUpToTrLayer(1);


    //Fill Tree
    for(int ir = 1; ir < 2; ir ++){
        histn = Form("%s/q%d/%s/TreeVar", sDir.c_str(), ich, cSpan[ir]);
        histn += Form("/Tree_r%d_q%d", ir, ich);
        tVar = static_cast<TTree*>(outfile.Get(histn.c_str()));
        tVar->Fill();
    } //ir
    return true;
}