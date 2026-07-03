//----Author Q.Yan qyan@cern.ch: interface to Millepede
#include <TH1.h>
#include <TH2F.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TLine.h>
#include <TSystem.h>
#include <TStyle.h>
#include <iostream>
#include <iomanip>
#include  "TMath.h"
#include "TMinuit.h"
#include <sstream>
#include "TString.h"
#include <fstream>
#include <iostream>
#include "TFile.h"
#include "TMath.h"
#include "TTree.h"
#include "TString.h"
#include "HistoMan.h"
#include "TProfile2D.h"
#include "TVirtualFitter.h"
#include "root_RVSP.h" //betahr
#include "TDatime.h"
#include "TkGeomN.h"
#include "MilleBinary.h"
#include "GblFitter.h"
#include "readfile3.C"
//#include "/afs/cern.ch/work/q/qyan/work/analysis/PreSelect/bacground2/bin.C"
#include "QEvent2.h"
#include "QAnalysis.h"

#define USERTI7
#define USETTK

#if defined (AFTERALIGN2) || defined (AFTERALIGN3)
#define AFTERALIGN
#endif

using namespace qanalysisconst;

class QAnalysisN: public QAnalysis{
 public:
    static const int SCharge=1;
//    static const int MCharge=8;
    static const int MCharge=2;
    HistoMan hman1;
 public:
    unsigned int timebe[2];
    static int sev[30][10];
    static double cput[30][10];
    char *runlogname;
    std::map<int, int > ladnev;
    std::map<int, int > ladnevx;
    std::map<unsigned int, int > runlist;
    std::map<int, map<int,int> > beamlist;
    gbl::MilleBinary* milleFile;
    TkTrackN *tkfinal;
    vector<pair<AMSPoint,AMSDir> >beampd;
 public:
    QAnalysisN(TChain *ch):QAnalysis(ch){timebe[0]=timebe[1]=0;runlogname=0;milleFile=0;tkfinal=0;}
    int BookHistos(const char *ofile);
    int SelectEvent(QEvent *ev0=0,int pos=0);
    double GetdTheta(float a[3],float b[3],int ixy);
    int FillExpoTime(const char *runlogdir);
    int Save(){hman1.Save();hman1.Clear();return 0;}
 public:
    float GetMean(vector<float> signal,float &rms);
};

double QAnalysisN::cput[30][10]={{0}};
int    QAnalysisN::sev[30][10]={{0}};

//----
float QAnalysisN::GetMean(vector<float> signal,float &rms){
  int   n    = 0;
  float mean = 0;
  rms  = 0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    mean += signal.at(ii);
    rms  += pow(signal.at(ii),2);
    n++;
  }
  mean /= n;
  rms  /= n;
  rms = sqrt(rms - mean*mean);
  return mean;
}

double QAnalysisN::GetdTheta(float a[3],float b[3],int ixy){
  double cosab=0;
  double a1[3],b1[3];
  for(int ic=0;ic<3;ic++){a1[ic]=a[ic];b1[ic]=b[ic];}
  double sign=1;
  if(ixy<=1){
    cosab=fabs(a1[ixy]*b1[ixy]+a1[2]*b1[2])/sqrt(a1[ixy]*a1[ixy]+a1[2]*a1[2])/sqrt(b1[ixy]*b1[ixy]+b1[2]*b1[2]);
    if(a1[ixy]/a1[2]<b1[ixy]/b1[2]){sign=-1;}
 }
  else {
    cosab=fabs(a1[0]*b1[0]+a1[1]*b1[1]+a1[2]*b1[2]);
    double as=sqrt(a1[0]*a1[0]+a1[1]*a1[1]+a1[2]*a1[2]);
    double bs=sqrt(b1[0]*b1[0]+b1[1]*b1[1]+b1[2]*b1[2]);
    cosab=cosab/as/bs;
  }
  return sign*sqrt(2*(1.-cosab));
}

//const int ntbrun=81+7+16+14+41+11+13;
const int ntbrun=81+41+14+11+7+16+13;
int tbrun[]={
//---pr400.416.job
1281355854,1281360821,1281361614,1281364582,1281365312,1281368961,1281372836,1281373519,1281375482,1281379100,1281379631,1281437027,1281437618,1281442014,1281445825,1281451114,1281451533,1281455126,1281455529,1281460368,1281460742,1281461240,1281461521,1281461849,1281462458,1281464767,1281465188,1281469026,1281469400,1281473429,1281473836,1281476221,1281477924,1281478326,1281481749,1281482091,1281485405,1281485879,1281489554,1281489918,1281492821,1281493193,1281496321,1281496667,1281499451,1281499868,1281502753,1281503164,1281506356,1281506765,1281510094,1281510462,1281513970,1281514335,1281518667,1281519126,1281522043,1281522407,1281526336,1281526755,1281530403,1281530823,1281533702,1281534066,1281538126,1281538537,1281542241,1281542655,1281546270,1281546656,1281550125,1281550445,1281553865,1281703951,1281704189,1281704862,1281705258,1281705735,1281706130,1281706488,1281706805,
//---pr400.280.job
1281574137,1281574443,1281578465,1281578732,1281582266,1281582536,1281587475,1281589010,1281592643,1281593145,1281596254,1281596574,1281600076,1281600591,1281604007,1281606521,1281609038,1281609357,1281610088,1281612601,1281612923,1281617233,1281617510,1281619430,1281619804,1281640859,1281642092,1281644856,1281645332,1281648749,1281649023,1281652569,1281652942,1281656377,1281656736,1281660336,1281660744,1281664344,1281664708,1281668323,1281668702,
//---pr400.80.job
1281554781,1281558146,1281558525,1281560441,1281562495,1281563603,1281563954,1281567509,1281567823,1281571449,1281571767,1281694130,1281697946,1281698270,
//---pr400.60feb.job
1281673346,1281673672,1281677606,1281677976,1281681492,1281681800,1281685396,1281685815,1281689414,1281689739,1281693158,
//---pr400.60.job
1281323720,1281325499,1281327461,1281329420,1281334027,1281337137,1281340985,
//---pr400.10.job
1281346173,1281346723,1281347420,1281347968,1281348569,1281349222,1281350113,1281350720,1281351471,1281352359,1281354296,1281701582,1281701726,1281702230,1281702338,1281702882,
//--pr400.60feb_back.job
1281713527,1281733073,1281733392,1281739047,1281739510,1281743506,1281744017,1281749048,1281749534,1281751130,1281751649,1281755462,1281755834,
};

const int ntbbeam=1015;
int tbbeam[][2]={
{0,0},{0,1},{0,3},{0,4},{0,5},{0,6},{0,7},{0,8},{0,9},{0,10},{0,11},{0,12},{0,13},{0,14},{1,15},{1,16},{1,17},{2,17},{2,18},{2,19},{2,20},{2,21},{2,22},{2,23},{2,24},{2,25},{2,26},{3,26},{4,26},{4,27},{4,28},{4,29},{4,30},{4,31},{4,32},{4,33},{4,34},{4,35},{5,36},{5,37},{5,38},{5,39},{5,40},{5,41},{5,42},{5,43},{5,44},{5,45},{5,46},{5,47},{5,48},{5,49},{6,50},{7,50},{7,51},{7,52},{7,53},{7,54},{7,55},{7,56},{8,57},{8,58},{8,59},{8,60},{8,61},{8,62},{8,63},{8,64},{8,65},{8,66},{8,67},{8,68},{8,69},{9,70},{10,70},{10,71},{10,72},{10,73},{11,75},{12,75},{12,76},{12,77},{12,78},{12,79},{12,80},{12,81},{12,82},{12,83},{13,84},{13,85},{13,86},{13,87},{13,88},{13,89},{14,89},{14,90},{14,91},{14,92},{14,93},{14,94},{14,95},{14,96},{14,97},{14,98},{15,98},{16,98},{16,99},{16,100},{16,101},{16,102},{16,103},{16,104},{16,105},{16,106},{16,107},{16,108},{16,109},{17,110},{18,110},{18,111},{18,112},{18,113},{18,114},{18,115},{18,116},{19,116},{20,116},{20,117},{21,118},{22,119},{23,120},{23,121},{24,122},{24,123},{24,124},{24,125},{24,126},{24,129},{25,129},{26,129},{26,130},{26,131},{26,132},{26,133},{26,134},{26,135},{26,136},{26,137},{26,138},{26,139},{26,140},{26,141},{27,141},{28,141},{28,142},{28,143},{28,144},{28,145},{28,146},{28,147},{28,148},{28,149},{28,150},{28,151},{28,152},{28,153},{29,153},{30,153},{30,154},{30,155},{30,156},{30,157},{30,158},{30,159},{30,160},{31,161},{31,162},{31,163},{31,164},{31,165},{31,166},{32,166},{33,166},{33,167},{33,168},{33,169},{33,170},{33,171},{33,172},{33,173},{33,174},{33,175},{33,176},{33,177},{34,177},{35,177},{35,178},{35,179},{35,180},{35,181},{35,182},{35,183},{35,184},{35,185},{35,186},{35,187},{35,188},{35,189},{35,190},{36,190},{37,190},{37,191},{37,192},{37,193},{37,194},{37,195},{37,196},{37,197},{37,198},{37,199},{37,200},{37,201},{37,202},{38,202},{39,202},{39,203},{39,204},{39,205},{39,206},{39,207},{39,208},{39,209},{39,210},{39,211},{39,212},{40,212},{41,212},{41,213},{41,214},{41,215},{41,216},{41,217},{41,218},{41,219},{41,220},{41,221},{41,222},{41,223},{42,223},{43,223},{43,224},{43,225},{43,226},{43,227},{43,228},{43,229},{43,230},{43,231},{43,232},{43,233},{43,234},{43,235},{43,236},{44,236},{45,236},{45,237},{45,238},{45,239},{45,240},{45,241},{45,242},{45,243},{45,244},{45,245},{45,246},{45,247},{45,248},{45,249},{46,249},{47,249},{47,250},{47,251},{47,252},{47,253},{47,254},{47,255},{47,256},{47,257},{47,258},{47,259},{47,260},{48,261},{49,261},{49,262},{49,263},{49,264},{49,265},{49,266},{49,267},{49,268},{49,269},{49,270},{49,271},{49,272},{49,273},{49,274},{50,274},{51,275},{51,276},{51,277},{51,278},{51,279},{51,280},{51,281},{51,282},{51,283},{51,284},{51,285},{51,286},{51,287},{51,288},{52,288},{53,288},{53,289},{53,290},{53,291},{53,292},{53,293},{53,294},{53,295},{53,296},{53,297},{53,298},{53,299},{53,300},{54,301},{55,301},{55,302},{55,303},{55,304},{55,305},{55,306},{55,307},{55,308},{55,309},{55,310},{55,311},{56,311},{57,311},{57,312},{57,313},{57,314},{57,315},{57,316},{57,317},{57,318},{57,319},{57,320},{57,321},{57,322},{57,323},{58,324},{59,324},{59,325},{59,326},{59,327},{59,328},{59,329},{59,330},{59,331},{59,332},{59,333},{59,334},{59,335},{59,336},{59,337},{60,338},{61,338},{61,339},{61,340},{61,341},{61,342},{61,343},{61,344},{61,345},{61,346},{61,347},{61,348},{62,348},{63,348},{63,349},{63,350},{63,351},{63,352},{63,353},{63,354},{63,355},{63,356},{63,357},{63,358},{63,359},{63,360},{64,360},{65,360},{65,361},{65,362},{65,363},{65,364},{65,365},{65,366},{65,367},{65,368},{65,369},{65,370},{65,371},{65,372},{65,373},{65,374},{66,375},{67,376},{67,377},{67,378},{67,379},{67,380},{67,381},{67,382},{67,383},{67,384},{67,385},{67,386},{67,387},{67,388},{67,389},{68,389},{69,389},{69,390},{69,391},{69,392},{69,393},{69,394},{69,395},{69,396},{69,397},{69,398},{69,399},{69,400},{69,401},{69,402},{69,403},{70,403},{71,403},{71,404},{71,405},{71,406},{71,407},{71,408},{71,409},{71,410},{71,411},{71,412},{71,413},{71,414},{71,415},{72,415},{73,127},{73,128},{74,128},{75,344},{75,386},{76,2},{76,344},{77,2},{77,74},{78,74},{79,294},{80,294},{81,416},{82,416},{82,417},{82,418},{82,419},{82,420},{82,421},{82,422},{82,423},{82,424},{82,425},{82,426},{82,427},{82,428},{82,430},{82,431},{82,432},{82,433},{82,434},{82,435},{82,436},{83,436},{84,436},{84,437},{84,438},{84,439},{84,440},{84,441},{84,442},{84,443},{84,444},{84,445},{84,446},{84,447},{84,448},{84,450},{85,452},{86,452},{86,453},{86,455},{86,457},{86,458},{86,459},{86,460},{86,461},{86,462},{86,463},{86,464},{86,465},{86,466},{86,467},{87,468},{88,468},{88,469},{88,470},{88,471},{88,472},{88,475},{88,476},{88,477},{88,478},{88,479},{88,480},{88,481},{88,482},{88,483},{89,483},{90,483},{90,484},{90,485},{90,486},{90,487},{90,488},{90,489},{90,490},{90,491},{90,492},{90,493},{90,494},{90,495},{90,496},{90,497},{91,498},{92,498},{92,499},{92,500},{92,501},{92,502},{92,503},{92,504},{92,505},{92,506},{92,507},{92,508},{92,509},{92,510},{93,512},{94,511},{94,513},{94,514},{94,515},{94,516},{94,517},{94,518},{94,519},{94,520},{94,521},{95,521},{96,521},{96,522},{96,523},{96,524},{96,525},{96,526},{96,527},{96,528},{96,529},{96,530},{96,531},{97,532},{98,532},{98,533},{98,534},{99,535},{99,536},{99,537},{99,538},{99,539},{99,540},{99,541},{99,542},{99,543},{99,544},{99,545},{99,546},{99,547},{100,547},{101,547},{101,548},{101,549},{101,550},{101,551},{101,552},{101,553},{101,558},{101,559},{101,560},{101,561},{101,562},{101,563},{101,564},{102,564},{103,564},{103,565},{103,566},{103,567},{103,568},{103,569},{103,570},{103,571},{104,571},{105,571},{106,571},{107,571},{107,572},{107,573},{107,574},{107,575},{107,576},{107,577},{107,578},{107,579},{107,580},{107,581},{107,582},{107,583},{107,584},{107,585},{108,585},{109,586},{109,588},{109,589},{109,590},{109,591},{109,592},{109,593},{109,594},{109,595},{109,596},{109,597},{109,598},{110,598},{111,599},{111,600},{111,601},{111,602},{111,603},{111,604},{111,605},{111,606},{111,607},{111,608},{111,609},{111,610},{111,611},{111,612},{111,613},{112,614},{113,614},{113,616},{113,617},{113,618},{113,619},{113,620},{113,621},{113,622},{113,623},{113,624},{113,625},{113,627},{113,628},{114,628},{115,628},{115,629},{115,630},{115,631},{115,632},{115,633},{115,634},{115,635},{115,636},{115,637},{115,638},{115,639},{115,640},{115,641},{115,642},{115,643},{116,643},{117,643},{117,644},{117,645},{117,646},{117,647},{117,648},{117,649},{117,650},{117,651},{117,652},{117,653},{117,654},{117,655},{117,656},{117,657},{117,658},{117,659},{117,660},{118,661},{119,661},{119,662},{119,663},{119,664},{119,665},{119,666},{119,667},{119,668},{119,669},{119,670},{119,671},{119,672},{119,673},{119,674},{119,675},{119,676},{119,677},{120,677},{121,677},{121,678},{121,679},{121,680},{121,681},{121,682},{121,683},{121,684},{121,685},{121,686},{121,687},{121,688},{121,689},{121,690},{121,691},{121,692},{121,693},{121,694},{121,695},{122,696},{122,697},{122,698},{122,699},{122,700},{122,701},{122,702},{122,703},{122,704},{122,705},{122,706},{123,706},{124,707},{124,708},{124,709},{124,710},{124,711},{124,712},{124,713},{125,714},{125,716},{126,736},{127,736},{128,737},{128,738},{128,739},{128,740},{128,741},{128,742},{128,743},{128,744},{128,745},{128,746},{128,747},{128,748},{128,749},{130,751},{130,752},{130,753},{130,754},{130,755},{130,756},{130,757},{130,758},{130,759},{130,760},{130,761},{130,762},{130,763},{130,764},{130,765},{130,766},{131,766},{132,766},{132,767},{132,768},{132,769},{132,770},{132,771},{132,772},{132,773},{132,774},{132,775},{133,696},{133,710},{133,711},{133,712},{133,713},{133,714},{133,715},{133,717},{133,718},{133,719},{133,720},{133,721},{133,722},{133,723},{134,724},{135,724},{135,725},{135,726},{135,727},{135,728},{135,729},{135,730},{135,731},{135,732},{135,733},{135,734},{135,735},{136,776},{137,776},{137,777},{137,778},{137,779},{137,780},{137,781},{137,782},{137,783},{137,784},{137,785},{137,786},{137,787},{137,788},{137,789},{137,790},{138,791},{139,791},{139,792},{139,793},{139,794},{139,795},{139,796},{139,797},{139,798},{139,799},{140,800},{141,800},{141,801},{141,802},{141,803},{141,804},{141,805},{141,806},{141,807},{141,808},{141,809},{141,810},{141,811},{141,812},{142,813},{143,813},{143,814},{143,815},{143,816},{143,817},{143,818},{143,820},{143,821},{143,822},{143,823},{143,824},{143,825},{143,826},{143,827},{144,828},{145,828},{145,829},{145,830},{145,831},{145,832},{145,833},{145,834},{145,835},{146,819},{147,838},{147,839},{147,840},{147,841},{148,841},{148,842},{148,843},{148,844},{148,845},{148,846},{148,847},{148,848},{149,848},{149,849},{149,850},{149,851},{149,852},{149,853},{149,854},{149,855},{150,855},{150,856},{150,857},{150,858},{150,859},{150,860},{150,861},{151,862},{151,865},{151,866},{151,867},{151,868},{151,869},{151,870},{151,871},{151,872},{152,873},{152,874},{152,875},{152,876},{152,877},{152,878},{152,879},{152,880},{152,881},{152,882},{152,883},{152,884},{152,885},{153,886},{153,887},{153,889},{153,890},{153,891},{153,892},{153,893},{153,894},{153,895},{154,896},{154,897},{155,896},{155,897},{156,898},{156,899},{157,898},{157,899},{158,900},{158,901},{159,900},{159,901},{160,902},{160,903},{161,902},{161,903},{162,904},{162,905},{163,904},{163,905},{164,896},{164,897},{165,897},{166,896},{166,897},{167,898},{167,899},{168,898},{168,899},{169,900},{169,901},
};

//------BookHisto
int QAnalysisN::BookHistos(const char *ofile){

  hman1.Clear();
  hman1.Enable();
  hman1.Setname(ofile);

//---
  char histn[1000];
//--normal bin
/*  double hebin[1000];
  const int bin=getamsbin(hebin,1)-1;*/
  for(int ixy=0;ixy<2;ixy++){
    sprintf(histn,"chisoxy%d",ixy);
    hman1.Add(new TH1D(histn,histn,500,0,100));
    sprintf(histn,"chiso2xy%d",ixy);
    hman1.Add(new TH1D(histn,histn,500,0,100));
  }
  for(int iud=0;iud<2;iud++){
    sprintf(histn,"tofqud%d",iud);
    hman1.Add(new TH1D(histn,histn,100,0,5));
  }
  hman1.Add(new TH1D("tofbetah","tofbetah",200,-2,2));
  hman1.Add(new TH1D("tkinq","tkinq",100,0,5));
  hman1.Add(new TH1D("tkl1q","tkl1q",100,0,5));
  hman1.Add(new TH1D("ntrack","ntrack",10,0,10));
  hman1.Add(new TH1D("tkl9q","tkl9q",100,0,5)); 
  for(int il=0;il<9;il++){
    sprintf(histn,"posxyl%d",il);
    if(il==0||il==8)hman1.Add(new TH2F(histn,histn,800,-200,200,800,-200,200));
    else     hman1.Add(new TH2F(histn,histn,400,-100,100,400,-100,100));
    for(int ixy=0;ixy<2;ixy++){
      sprintf(histn,"beamdxy%dl%d",ixy,il);
#ifdef USETESTBEAMA
      hman1.Add(new TH2D(histn,histn,910,0,910,200,-10,10));
#else
      hman1.Add(new TH2F(histn,histn,910,0,910,400,-6,6));
#endif
    }
  }
  hman1.Add(new TH2D("beamrun","beamrun",ntbrun+2,-1,ntbrun+1,911,-1,910));
  for(int iz=SCharge;iz<=MCharge;iz++){
    for(int ib=0;ib<2;ib++){
      sprintf(histn,"seleffp_b%d",ib);
      if(iz==SCharge)hman1.Add(new TH1D(histn,histn,10,0,10));
      sprintf(histn,"trigp_b%d",ib);
      if(iz==SCharge)hman1.Add(new TH1D(histn,histn,7,-2,5));
      sprintf(histn,"spotd_b%d",ib);
      if(iz==SCharge)hman1.Add(new TH1D(histn,histn,400,0,100));
      sprintf(histn,"seleff_z%db%d",iz,ib); 
      hman1.Add(new TH1D(histn,histn,10,0,10));
      sprintf(histn,"selext_z%db%d",iz,ib);
      hman1.Add(new TH1D(histn,histn,10,0,10));
    }
/*    sprintf(histn,"selmom_z%d",iz);
    hman1.Add(new TH1D(histn,histn,bin,hebin));*/
#ifdef TESTRIG
    for(int ib=0;ib<=4;ib++){
      sprintf(histn,"srigr1_q%db%d",iz,ib);
      hman1.Add(new TH1D(histn,histn,2000,-40,40));
      sprintf(histn,"srigr1_q%d",iz);
      if(ib==0)hman1.Add(new TH1D(histn,histn,2000,-40,40));
    }
    if(iz==SCharge){
      hman1.Add(new TH2F("srigr1b0","srigr1b0",910,0,910,500,-20,20));
      hman1.Add(new TH2F("srigr1b","srigr1b",910,0,910,500,-20,20));
    }
#endif
    for(int ixy=1;ixy<=2;ixy++){
      sprintf(histn,"ladnev_z%dxy%d",iz,ixy);
      hman1.Add(new TH1D(histn,histn,400,-200,200));
      sprintf(histn,"sennev_z%dxy%d",iz,ixy);
      hman1.Add(new TH2D(histn,histn,400,-200,200,16,0,16));
      int uxy=(ixy==1)?1:0; 
      if(iz==SCharge){
        sprintf(histn,"ladpos_xy%d",uxy);
        if(uxy==0)hman1.Add(new TH2F(histn,histn,400,-200,200,100,-35,35));
        else      hman1.Add(new TH2F(histn,histn,400,-200,200,20,-4,4));
        sprintf(histn,"laddir_xy%d",uxy);
        hman1.Add(new TH2D(histn,histn,400,-200,200,100,-1,1));
        sprintf(histn,"senpos_xy%d",uxy);
        if(uxy==0)hman1.Add(new TH2F(histn,histn,8000,-4000,4000,20,-2.5,2.5));
        else      hman1.Add(new TH2F(histn,histn,8000,-4000,4000,20,-4,4));
        sprintf(histn,"sendir_xy%d",uxy);
        hman1.Add(new TH2F(histn,histn,8000,-4000,4000,100,-0.5,0.5));
#if defined (AFTERALIGN) || defined (USENOSMEAR)
        sprintf(histn,"senres_xy%d",uxy);
        if(uxy==0)hman1.Add(new TH2F(histn,histn,8000,-4000,4000,100,-100,100));
        else      hman1.Add(new TH2F(histn,histn,8000,-4000,4000,100,-100,100));
        sprintf(histn,"ladres_xy%d",uxy);
        if(uxy==0)hman1.Add(new TH2F(histn,histn,400,-200,200,100,-100,100));
        else      hman1.Add(new TH2F(histn,histn,400,-200,200,100,-100,100));
#else
        sprintf(histn,"senres_xy%d",uxy);
        if(uxy==0)hman1.Add(new TH2F(histn,histn,8000,-4000,4000,100,-400,400));
        else      hman1.Add(new TH2F(histn,histn,8000,-4000,4000,100,-400,400));
        sprintf(histn,"ladres_xy%d",uxy);
        if(uxy==0)hman1.Add(new TH2F(histn,histn,400,-200,200,100,-800,800));
        else      hman1.Add(new TH2F(histn,histn,400,-200,200,100,-800,800));
#endif
      }
    }
    for(int il=0;il<9;il++){
      for(int ixy=0;ixy<2;ixy++){
        sprintf(histn,"res_z%dl%dxy%d",iz,il,ixy);
#if defined (AFTERALIGN) || defined (USENOSMEAR)
        hman1.Add(new TH1D(histn,histn,100,-100,100)); 
#else
        hman1.Add(new TH1D(histn,histn,100,-1000,1000));
#endif
      }
    }
    for(int ir=0;ir<4;ir++){
      for(int ixy=0;ixy<3;ixy++){
        sprintf(histn,"chisr%dz%dxy%d",ir,iz,ixy);
        hman1.Add(new TH1D(histn,histn,500,0,100));
      }
    }
  }
//----
//  const char *sfn="/afs/cern.ch/ams/Offline/AMSDataDir/v5.00/TestBeamPos_416.txt";
/*  const char *sfn="/afs/cern.ch/work/q/qyan/work/alignment/TestBeamPos_896V1.txt";
  int nb=896;*/
//  const char *sfn="/afs/cern.ch/work/q/qyan/work/alignment/TestBeamPos_906V2.txt";
//  const char *sfn="/afs/cern.ch/work/q/qyan/work/alignment/TestBeamPos_906V3.txt";
  const char *sfn="/afs/cern.ch/work/q/qyan/work/alignment/TestBeamPos_906V33.txt";
  int nb=906;
  ifstream fin(sfn);
   if (!fin) {
      cerr << "AMSEventR::GetBeamPos-E-File not found: " << sfn << endl;
   }
   int nPos=0;
   AMSPoint pPos, pDir;
   beampd.clear();
   for (int i = 0; i < nb; i++) {
      double x, y, z, dx, dy, dz;
      fin >> x >> y >> z >> dx >> dy >> dz;
      if (fin.eof()) break;
      pPos.setp( x,  y,  z);
      pDir.setp(dx, dy, dz);
      nPos++;
      beampd.push_back(make_pair(pPos,pDir));
    }
    cout<<"Beam-nPos="<<nPos<<endl;
    if (nPos != nb) {
      cerr << "AMSEventR::GetBeamPos-E-File format is wrong: "
           << sfn << endl;
      nPos = -1;
      beampd.clear();
    }
  //---- 
  return 0;
}


//-------------------Select Event
int QAnalysisN::SelectEvent(QEvent *ev,int pos){


//---Weight factor
  static char histn[1000];
  double ww=1;
  if(runlist.size()==0){
    for(int irun=0;irun<ntbrun;irun++){
      runlist[tbrun[irun]]=irun;
    }
  }
  if(beamlist.size()==0){
    for(int il=0;il<ntbbeam;il++){
      beamlist[tbbeam[il][0]][tbbeam[il][1]]=il;
//      cout<<"il="<<il<<" pp="<<tbbeam[il][0]<<","<<tbbeam[il][1]<<endl;
    }
  }
//----
  if(ev->Select_Particle())hman1.Fill("tofbetah",ev->tof_betah);

//---Trigger
  sprintf(histn,"seleffp_b0");
  hman1.Fill(histn,0);
//  cout<<"run="<<ev->run<<" time="<<ev->time[0]<<endl;
//  cout<<"isreal="<<ev->isreal<<endl;
  if(!ev->isreal){
    if(!ev->Select_Trigger())return 1;
  }
  hman1.Fill(histn,1);

//---Particle(TOF+Tk) 
  if(!ev->Select_Particle())return 1;
  hman1.Fill(histn,2);

//---TOFBeta
#ifdef USENEGBETA
  if(ev->tof_betah>-0.4)return 1;
#else
  if(!ev->Select_TofBeta())return 1;
  hman1.Fill(histn,3);
  if(ev->tof_betah<0.4)return 1;
//  if(ev->tof_betah<0.9)return 1;
#endif
  hman1.Fill(histn,4);

//---InnerNHit
   if(ev->GetTkInNHit(1)<5||ev->GetTkInNHit(0)<3||ev->GetTkInNPlane(1)<3){return 1;}//>=3 Yplans && >=3 XHits &&>=5 Hits
//   if(ev->GetTkInNHit(1)<4||ev->GetTkInNHit(0)<3||ev->GetTkInNPlane(1)<3){return 1;}//>=3 Yplans && >=3 XHits &&>=5 Hits
//   if(ev->GetTkInNHit(1)<5||ev->GetTkInNHit(0)<2||ev->GetTkInNPlane(1)<4){return 1;}//4 plans  
   hman1.Fill(histn,5);

//---InnerZ
   int  tk_z1=ev->GetTkInZ(2);
//--Normal Cut
   bool parqcut=(ev->Select_TkInQ(0,tk_z1)&&ev->Select_TofUQ(0,tk_z1)&&ev->Select_TofDQ(0,tk_z1));
   if(parqcut)hman1.Fill(histn,6);
   if(tk_z1<=2){//tight TOFQ cut
     parqcut=(parqcut&&ev->GetTOFUDQ(0)<tk_z1+1&&ev->GetTOFUDQ(1)<tk_z1+1);
   }
   if(parqcut)hman1.Fill(histn,7);
   bool partkcut=(ev->GetChis(1)<10000&&ev->GetChis(0)<10000);//~0.2cm
   if(parqcut&&partkcut)hman1.Fill(histn,8);
   bool cutl1hzt=(ev->GetTkLQ(0,2)>tk_z1-0.4&&ev->GetTkLQ(0,2)<tk_z1+1);
   bool cutl9hzt=(ev->GetTkLQ(8,2)>tk_z1-0.4&&ev->GetTkLQ(8,2)<tk_z1+1);
   bool cuttrig[2]={0};
   cuttrig[0]=((ev->jmembpatt>>14)&1);
   cuttrig[1]=((ev->jmembpatt>>15)&1);
   bool bttrig=(ev->isreal)?(cuttrig[0]&&cuttrig[1]):1;//beam test trig
//-------
   for(int ixy=0;ixy<2;ixy++){
     sprintf(histn,"chisoxy%d",ixy);
     hman1.Fill(histn,ev->GetChis(ixy));
   }
   for(int iud=0;iud<2;iud++){
     sprintf(histn,"tofqud%d",iud);
     hman1.Fill(histn,ev->GetTOFUDQ(iud));
   }
   hman1.Fill("tkinq",ev->GetTkInQ(2));
   if(ev->HasTkLHitXY(0)){
     hman1.Fill("tkl1q",ev->GetTkLQ(0,2));  
   }
   if(ev->HasTkLHitXY(8)){
     hman1.Fill("tkl9q",ev->GetTkLQ(8,2));
   }

   float mrig=ev->mmom/ev->mch;
   int   tk_zm=ev->mch;
   static const double tk_pz[9]={159.04,53.05,29.22,25.24,1.706,-2.292,-25.26,-29.25,-136.03};
///--Selection
   if(parqcut&&partkcut&&(tk_z1>=1&&tk_z1<=2)){//only use proton and helium
     int ib=0;
     if(!ev->isreal){
       bool issurvl8=(ev->mevmom1[13]!=-1000||ev->mevmom1[14]!=-1000);
       if(!issurvl8)ib=1;//background
     }
     sprintf(histn,"ntrack");
     hman1.Fill(histn,ev->ntrack);
     sprintf(histn,"seleff_z%db%d",tk_z1,ib);
     hman1.Fill(histn,0.);
     if(tk_z1!=1)return 2;//using proton
     hman1.Fill(histn,1.);
     if(ev->nbetah==1)hman1.Fill(histn,2.);
#if defined (TESTRIG) || defined (LOOSECUT)
     if(ev->ntrack>=3)return 3;
#else
     if(ev->ntrack!=1)return 3;
#endif
     hman1.Fill(histn,3.);
#ifdef USENEGBETA
     if(ev->tof_betah>-0.9)return 4;
#else
     if(ev->tof_betah<0.9)return 4;
#endif
     hman1.Fill(histn,4.);
//--------
     bool selbeam=ev->isreal;//only for data
     double cutbeamdis=1;
#ifdef USENEGBETA
     cutbeamdis=3;
#endif
#ifdef USETESTBEAMA
     if(!ev->isreal){//MC weight
       selbeam=1;
       cutbeamdis=10;
     }
#endif
     int ibeam=-1;
     int frun=-1;
     int beammat=1;
//     cout<<"jmem="<<((ev->jmembpatt>>14)&1)<<","<<((ev->jmembpatt>>15)&1)<<endl;
     if(selbeam){//test beam
       beammat=0;
       double mid=10000;
       double midl[2]={10000,10000};
       for(unsigned int i=0;i<beampd.size();i++){//match the closest
         double pd=0;
         double pl[2]={10000,10000};
         for(int iext=0;iext<2;iext++){
           int ul=(iext==0)?0:8;
           double z1=tk_pz[ul];
           AMSPoint p21=beampd[i].first+beampd[i].second/beampd[i].second.z()*(z1-beampd[i].first.z());
           double d1x=ev->tk_pos[ul][0]-p21[0];
           double d1y=ev->tk_pos[ul][1]-p21[1];
           double d1=sqrt(d1x*d1x+d1y*d1y);
           pl[iext]=d1;
           pd+=d1;
         }
         pd/=2.;
         if(pd<mid){mid=pd;ibeam=i;midl[0]=pl[0];midl[1]=pl[1];}
       }
       if(ibeam>=0){
         sprintf(histn,"spotd_b%d",bttrig?0:1);
         hman1.Fill(histn,mid);
//         if(ibeam>=416)cutbeamdis=1.;
         if(midl[0]<=cutbeamdis&&midl[1]<=cutbeamdis)beammat=1;
       }
       if(ev->isreal&&runlist.find(ev->run)!=runlist.end()){
         frun=runlist[ev->run];
         hman1.Fill("beamrun",frun,(beammat)?ibeam:-1);
         if(frun>=0){
           if(beamlist.find(frun)!=beamlist.end()){
             if(beamlist[frun].find(ibeam)==beamlist[frun].end())beammat=0;
           }
         }
       }
       for(int il=0;il<9;il++){
         sprintf(histn,"posxyl%d",il);
         hman1.Fill(histn,ev->tk_pos[il][0],ev->tk_pos[il][1]);
         if(ibeam<0)continue;
         int ul=0;
         if(il<4)ul=8;
         double z1=tk_pz[ul];
         AMSPoint p21=beampd[ibeam].first+beampd[ibeam].second/beampd[ibeam].second.z()*(z1-beampd[ibeam].first.z());
         double d1x=ev->tk_pos[ul][0]-p21[0];
         double d1y=ev->tk_pos[ul][1]-p21[1];
         double d1=sqrt(d1x*d1x+d1y*d1y);
         if(d1>cutbeamdis)continue;
         double z2=tk_pz[il];
         AMSPoint p22=beampd[ibeam].first+beampd[ibeam].second/beampd[ibeam].second.z()*(z2-beampd[ibeam].first.z());
         for(int ixy=0;ixy<2;ixy++){
           if(fabs(ev->tk_pos[il][1-ixy]-p22[1-ixy])>cutbeamdis)continue;
           sprintf(histn,"beamdxy%dl%d",ixy,il);
           hman1.Fill(histn,ibeam,ev->tk_pos[il][ixy]-p22[ixy]);
         }
       }
       for(int it=-1;it<3;it++){
         if     (it>=0&&it<=1){if(!cuttrig[it])continue;}
         else if(it==2)       {if(!(cuttrig[0]&&cuttrig[1]))continue;} 
         sprintf(histn,"trigp_b%d",beammat);
         hman1.Fill(histn,it); 
       }
#ifndef TESTRIG
#ifndef USENOBEAMMAT
       if(!beammat)return 5;
#endif
#endif
     }
     sprintf(histn,"seleff_z%db%d",tk_z1,ib);
     hman1.Fill(histn,5.);
     if(bttrig)hman1.Fill(histn,6.);
#ifdef USEBTTRIG
     if(!bttrig)return 6;
#endif
//-----estimation of L1 and L9 efficiency
     for(int iext=0;iext<2;iext++){
       int ul=(iext==0)?0:8;
       if(!ev->IsPassTkL(ul))continue;//should pass L1 and L9
       int hashit=ev->HasTkLHitXY(ul);
       for(int ixy=0;ixy<2;ixy++){
         sprintf(histn,"selext_z%db0",tk_z1);
         hman1.Fill(histn,iext*3);
         if(ixy==0){if(hashit!=2)continue;}
         else      {if(hashit==0)continue;}
         hman1.Fill(histn,iext*3+1);
         if(iext==0){if(!cutl1hzt)continue;}
         else       {if(!cutl9hzt)continue;}
         hman1.Fill(histn,iext*3+2);
       } 
     }
//-------
//     cout<<"ev="<<ev->run<<","<<ev->event<<" tk_z1="<<tk_z1<<" beta="<<ev->tof_betah<<endl;
     map<int, AMSPlaneM> plms;
     map<int, AMSPlaneM> plos;
     float chrg=tk_z1;
     double mass=TrFit::Mproton;
     int inlast=-1;
     int nhiti[2]={0,0};
     int ladidl[9]={0};
     int senidl[9]={0};
     for(int il=0;il<9;il++){
       int hashit=ev->HasTkLHitXY(il);
       if(hashit==0)continue;
//----
       if(il==0){if(!cutl1hzt)continue;}
       if(il==8){if(!cutl9hzt)continue;}
//----
       int tkid=ev->tk_lid[il];
       if(ladnev.find(tkid)==ladnev.end()){ladnev[tkid]=1;if(hashit==2)ladnevx[tkid]=1;}
       else {ladnev[tkid]++;if(hashit==2)ladnevx[tkid]++;}
//       if(tkid==-201||tkid==215||tkid==-701||tkid==715)cout<<"find tkid="<<tkid<<endl;
       double chanx=ev->tk_lch[il][0];
       double chany=ev->tk_lch[il][1];
       if(tkid==0||chanx<0||chany<0)continue;
//----
#ifdef USETESTBEAMA
       if(!ev->isreal){
         const int nladrej=6;
         int ladrej[]={-202,-302,-602,614,-702,714};
         for(int k=0;k<nladrej;k++){if(tkid==ladrej[k])return 7;}//reject these ladders as in TB
       }
#endif
       int tkid2=(tkid>0)?((tkid/100%100-1)*20+tkid%100):((tkid/100%100+1)*20+tkid%100);
       ladidl[il]=tkid2;
       TkLadderN *ladder=(TkLadderN *)tkfinal->modules_[tkid];
       AMSPlaneM p00=ladder->GetLocalCoo(chanx,chany);
       int senid2=(tkid2>=0)?tkid2*20+abs(p00.getid())%100:tkid2*20-abs(p00.getid())%100;
       senidl[il]=senid2;
       for(int ixy=1;ixy<=hashit;ixy++){
         sprintf(histn,"ladnev_z%dxy%d",tk_z1,ixy);
         hman1.Fill(histn,tkid2);
         int uxy=(ixy==1)?1:0;
       }
       for(int ixy=0;ixy<2;ixy++){
         sprintf(histn,"ladpos_xy%d",ixy);
         hman1.Fill(histn,tkid2,p00.getMGlobal()[ixy]);
         sprintf(histn,"laddir_xy%d",ixy);
         hman1.Fill(histn,tkid2,ev->tk_dir[il][ixy]/ev->tk_dir[il][2]);
         sprintf(histn,"senpos_xy%d",ixy);
         hman1.Fill(histn,senid2,(ixy==0)?p00.getM().X():p00.getM().Y());
         sprintf(histn,"sendir_xy%d",ixy);
         hman1.Fill(histn,senid2,ev->tk_dir[il][ixy]/ev->tk_dir[il][2]);
       }
//------
       AMSPlaneM plmr=(ev->isreal)?tkfinal->GetGlobalCoo(tkid,chanx,chany,111):TkTrackN::GetHead()->GetGlobalCoo(tkid,chanx,chany,0);
       AMSPlaneM plor(TVector3(0.,0.,ev->tk_hitc[il][2]), TVector3(1,0,0), TVector3(0,1,0),il);
       plor.setM(ev->tk_hitc[il][0],ev->tk_hitc[il][1]);
//       cout<<"id="<<plmr.getid()<<endl;
       for(int ixy=1;ixy<=hashit;ixy++){
         sprintf(histn,"sennev_z%dxy%d",tk_z1,ixy);
         hman1.Fill(histn,tkid2,abs(plmr.getid())%100);
       }
       if(!ev->isreal){//MC adding smearing
         if(ev->mtrmom[il]==0)continue;
         AMSPoint p0(ev->mtrcoo1[il][0],ev->mtrcoo1[il][1],ev->mtrcoo1[il][2]);
         AMSDir pdir(ev->mtrdir1[il][0],ev->mtrdir1[il][1],ev->mtrdir1[il][2]);
         double prig=ev->mtrmom[il];//has to divided by Z
         TrProp prop=TrProp(p0,pdir,prig);
         double nrig;
//--without smearing
         double slengr=prop.Interpolate(plmr,nrig);
//--with the smearing
         AMSPlaneM plmm=TkTrackN::GetHead()->GetGlobalCoo(tkid,chanx,chany,111);
         double slengm=prop.Interpolate(plmm,nrig);
//------
         if(slengr<-999||slengm<-999)continue;
         TVector3 sloc=plmm.getPLocal()-plmr.getPLocal();
//         cout<<"il="<<il<<" prig="<<prig<<" mrig="<<mrig<<" smear="<<sloc[0]<<","<<sloc[1]<<","<<sloc[2]<<" sleng="<<slengr<<","<<slengm<<endl;
         plmr.moveM(sloc[0],sloc[1]);//adding smearing to the original (would have bias for the matrix in gcalib)
//-----Find new channel in the smeared geometry
         TkSensorN *sensor=(TkSensorN *)TkTrackN::GetHead()->modules_[plmr.getid()];
         double disxy[2];
         double senchanx,senchany;
         for(int ixy=0;ixy<2;ixy++){//try to find new matched channel
           disxy[ixy]=sensor->FindLocalChan((ixy==0)?senchanx:senchany,ixy,(ixy==0)?plmr.getM().X():plmr.getM().Y());//new chan
         }
         double dist=sqrt(disxy[0]*disxy[0]+disxy[1]*disxy[1]);
         double ladchanx=(dist==0)?sensor->GetInLadderChanX(senchanx):-2;
//-------current geometry measurment 
         if(ladchanx>-1){
           AMSPlaneM plma=tkfinal->GetGlobalCoo(tkid,ladchanx,senchany,111);
           plmr=plma;
         }
         else {
           AMSPlaneM plma=tkfinal->GetGlobalCoo(tkid,chanx,chany,111);
           plmr.setO(plma.getO());
           plmr.setUV(plma.getU(),plma.getV());
         }
//------           
       }
       double errx = 25.e-4;
       double erry = 13.e-4;
       if(chrg>=2){mass = TrFit::Mhelium/2*chrg; errx=25.e-4*0.65 ; erry = 6.5e-4;}
/*       errx = 50.e-4;//amplify the error
       erry = 50.e-4;*/
/*       errx = 300.e-4;//amplify the error
       erry = 300.e-4;*/
#ifndef SAMEL19ERROR
       if(il==0)  {errx = std::sqrt(errx*errx+9.0e-4*9.0e-4);  erry = std::sqrt(erry*erry+9.0e-4*9.0e-4);}
       if(il==9-1){errx = std::sqrt(errx*errx+11.5e-4*11.5e-4);erry = std::sqrt(erry*erry+11.5e-4*11.5e-4);}
#endif
       float ferx=(hashit>=2) ? 1 : 0;
       float fery=(hashit>=1) ? 1 : 0;
       AMSPoint ferr(ferx*errx,fery*erry,0);
       for(int ixy=0;ixy<2;ixy++){plmr.setMCov(ixy,ixy,ferr[ixy]*ferr[ixy]); plor.setMCov(ixy,ixy,ferr[ixy]*ferr[ixy]);}
//       cout<<"il"<<il<<" errx="<<ferr[0]<<" erry="<<ferr[1]<<endl;
#if defined (USELAYERS)
       std::vector<std::pair<int,TMatrixD> > *malignder=plmr.getmalignder();
       malignder->erase(malignder->begin(),malignder->begin()+2);//erase frist two
#elif defined (USESENSORS)
       std::vector<std::pair<int,TMatrixD> > *malignder=plmr.getmalignder();
       malignder->erase(malignder->begin()+1,malignder->begin()+3);//erase ladder+layer
#elif defined (USELADDERS)     
       std::vector<std::pair<int,TMatrixD> > *malignder=plmr.getmalignder();
       malignder->erase(malignder->begin());//erase sensor and layer
       malignder->erase(malignder->begin()+1);
#endif
       plms[il]=plmr;
       plos[il]=plor;
       if(il>=1&&il<8){inlast=il;if(hashit>=2)nhiti[0]++;if(hashit>=1)nhiti[1]++;}
     }
     if(inlast>0){
/*       sprintf(histn,"selmom_z%d",tk_z1);
       hman1.Fill(histn,(ev->isreal)?ev->GetRigidity():ev->mtrmom[inlast]);//Data or MC*/
     }
     if(nhiti[0]<3||nhiti[1]<5)return 5;//Hit cut
     for(int ir=0;ir<4;ir++){//different fitting(ir==3 max-span)
#if defined (USEMAXSPAN)
       if(ir!=3)continue;
#elif defined (USEFULLSPAN)
       if(ir!=2)continue;
#else
       if(ir!=0)continue;
#endif
       if(ir==1||ir==2){if(plms.find(0)==plms.end())continue;}//without L1
       if(ir==2){if(plms.find(8)==plms.end())continue;}//wihout L9
       TrFit ptrfit;
       TrFit otrfit;
       for(int il=0;il<9;il++){
         if(plms.find(il)==plms.end())continue;
         if(!(ir==1||ir==2||ir==3)){if(il==0)continue;}
         if(!(ir==2||ir==3)){if(il==8)continue;}
         ptrfit.Add(plms[il]);
         otrfit.Add(plos[il]);
       }
       int method = TrFit::GBLFITTERAMS;
#if defined (USEMAXSPAN) || defined (USEFULLSPAN)
       int mscat=1;
#else
       int mscat=0;//force no multiple-scatting
#endif
#ifdef USENOMS
       mscat=0;//force no multiple-scatting
#endif
       int eloss=0;
       double beta=0;
       double rrgt=(!ev->isreal)?mrig:400;//using fix rigidity fit
#ifdef USENEGBETA
       rrgt=-rrgt;
#endif
       if(ev->isreal){
         static int nc=0;
         if(frun==-1&&nc++<=10)cerr<<"run="<<ev->run<<" not in the list"<<endl;
       }
//       if(ev->isreal&&frun==-1)rrgt=180;
       double rawrig=rrgt;
//       rrgt=0;
//       int uiter=0;//default
       int uiter=-2;//force 2 iterations
#ifdef TESTRIG
       mscat=1;
       rrgt=0;       
       uiter=0;
#endif
#ifdef USEOTRFIT
       ptrfit=otrfit;
#endif
       float fdone=ptrfit.DoFit(method,mscat,eloss,chrg,mass,beta,rrgt,uiter);
       bool done = (fdone>=0&&ptrfit.GetChisqX()>=0&&ptrfit.GetChisqY()>=0&&ptrfit.GetNdofX()>=1&&ptrfit.GetNdofY()>=1);//additional degree
       if(!done)continue;
/*       float odone=otrfit.DoFit(method,mscat,eloss,chrg,mass,beta,rrgt,uiter);
       bool oone = (odone>=0&&otrfit.GetChisqX()>=0&&otrfit.GetChisqY()>=0&&otrfit.GetNdofX()>=1&&otrfit.GetNdofY()>=1);//additional degree*/
//-----residual
       for(int ixy=0;ixy<2;ixy++){
         sprintf(histn,"chiso2xy%d",ixy);
         hman1.Fill(histn,ev->GetChis(ixy));
       }
       for(int ixy=0;ixy<3;ixy++){
         double pchis=0;
         if(ixy==0){
           if(ptrfit.GetNdofX()>0)pchis=ptrfit.GetChisqX()/ptrfit.GetNdofX();
         }
         else if(ixy==1){
           if(ptrfit.GetNdofY()>0)pchis=ptrfit.GetChisqY()/ptrfit.GetNdofY();
         }
         else  pchis=ptrfit.GetChisq();
         if(pchis>0)pchis=sqrt(pchis);
         sprintf(histn,"chisr%dz%dxy%d",ir,tk_z1,ixy);
         hman1.Fill(histn,pchis);
       }
       for(int layj=1;layj<=9;layj++){
         int ihit=ptrfit.iHitJ(layj);
         if(ihit<0)continue;
         for(int ixy=0;ixy<2;ixy++){
           double nrs=(ixy==0)?ptrfit.GetXr(ihit):ptrfit.GetYr(ihit);;
           if(nrs==0)continue;
           sprintf(histn,"res_z%dl%dxy%d",tk_z1,layj-1,ixy);
           hman1.Fill(histn,nrs*1e4);
           sprintf(histn,"senres_xy%d",ixy);
           hman1.Fill(histn,senidl[layj-1],nrs*1e4);
           sprintf(histn,"ladres_xy%d",ixy);
           hman1.Fill(histn,ladidl[layj-1],nrs*1e4);
         }
       }
//----abc
#if defined (TESTRIG)
       bool selhit=1;
       if(ir==3){//max span
         selhit=(plms.find(0)!=plms.end()&&plms.find(8)!=plms.end());//require both L1 and L9
       }
       for(int ibt=0;ibt<=4;ibt++){
         if(ibt==0){if(!(ev->ntrack==1&&beammat))continue;}
         if(ibt==1){if(!beammat)continue; if(ev->ntrack==1)continue;}
         if(ibt==2){if(!bttrig)continue;  if(beammat)continue;}
         if(ibt==3){if(!beammat)continue; if(bttrig)continue;}
         if(ibt==4){if(!(ev->ntrack==1&&bttrig))continue;}
         sprintf(histn,"srigr1_q%db%d",tk_z1,ibt);
         hman1.Fill(histn,1000.*(1./ptrfit.GetRigidity()-1./rawrig));
         if(ibt==0&&selhit){
           sprintf(histn,"srigr1_q%d",tk_z1);
           hman1.Fill(histn,1000.*(1./ptrfit.GetRigidity()-1./rawrig));
         }
         if(ibt==0){
           hman1.Fill("srigr1b0",ibeam,1000.*(1./ptrfit.GetRigidity()-1./rawrig));
           if(selhit)hman1.Fill("srigr1b",ibeam,1000.*(1./ptrfit.GetRigidity()-1./rawrig));
         }
       }
#endif
#if defined (USEOTRFIT) || defined (TESTRIG)
       continue;
#endif
//       cout<<"ir="<<ir<<" frig="<<ptrfit.GetRigidity()<<","<<mrig<<endl;
       if(!ptrfit.gblfitter)continue;
       genfit::GblFitter* gblfit=dynamic_cast<genfit::GblFitter*> (ptrfit.gblfitter);
       if(!gblfit)continue;
       gbl::GblTrajectory* traj=gblfit->getGblInfo();
//       unsigned int printlevel=1;
       unsigned int printlevel=0;
       traj->milleOut(*milleFile,printlevel); 
       for(int im=0;im<ptrfit.GetNhit();im++){
         AMSPlaneM *plm=ptrfit.GetHit(im);
         TVector3 dloc=plm->getDLocal();
         std::pair<std::vector<int>,TMatrixD> mat=plm->getResAlignDerGlobal(dloc); 
//----
         for(int ic=0;ic<mat.second.GetNrows();ic++){
           for(int ip=0;ip<mat.second.GetNcols();ip++){
//             cout<<"im="<<im<<" ic="<<ic<<" dlab="<<(mat.first)[ip]<<","<<(mat.second)(ic,ip)<<endl;
           }
         }
//------
       }
     }
   }
     
  
  return 0;
}


int Sum1_AMS_CINT(AMSChain *ch,TChain *ch1,const char *outfile,const char *outdir,const char *runlogdir,Long64_t num=-1, QAnalysisN *analysis2=0){

   QAnalysisN *analysis=analysis2;
   if(analysis==0)analysis=new QAnalysisN(ch1);
//   TkTrackN::GetHead()->LoadAlignModulePar("/afs/cern.ch/work/q/qyan/work/alignment/misaligninner.txt");
#if defined (USENOSMEAR)
   const char *smearfile=0;
#elif defined (USELAYERS)
   const char *smearfile="/afs/cern.ch/work/q/qyan/work/alignment/misalignlayer.txt";
#elif defined (USESENSORS)
   const char *smearfile="/afs/cern.ch/work/q/qyan/work/alignment/misalignsensor.txt";
#elif defined (USELADDERS)
   const char *smearfile="/afs/cern.ch/work/q/qyan/work/alignment/misalignladder.txt";
#else
//-----
#if defined (USETESTBEAMA)
//   const char *smearfile="/afs/cern.ch/work/q/qyan/work/alignment/noshear/misalign3calconsfixsenpsig_nobistb0.txt";
   const char *smearfile="/afs/cern.ch/work/q/qyan/work/alignment/misalign3ex19calcons2fixsenpsigcshear5_nobistb3MC0.txt";//including external layers
#else
   const char *smearfile="/afs/cern.ch/work/q/qyan/work/alignment/misaligninner2.txt";
#endif
//----
#endif
   if(smearfile)cout<<"alignment smearing file for MC="<<smearfile<<endl;
   TkTrackN::GetHead()->LoadAlignModulePar(smearfile);
//-----  

   TString milldata=outfile;
   milldata.ReplaceAll(".root","mille.bin");
   analysis->milleFile=new gbl::MilleBinary(milldata.Data());
   analysis->tkfinal=new TkTrackN();
   analysis->tkfinal->ConstructTracker();
#ifndef USENOSMEAR
#ifdef AFTERALIGN
   cout<<"use after align"<<endl;
//   analysis->tkfinal->LoadAlignModulePar("/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiter/millepede.res");
//   analysis->tkfinal->LoadAlignModulePar("/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/millepede.res");
//   analysis->tkfinal->LoadAlignModulePar("/afs/cern.ch/work/q/qyan/work/alignment/merge2iterpede.res");
//   analysis->tkfinal->LoadAlignModulePar("./test1/merge2iterc2p2pede.res");
//   const char *afterfile="./merge3itersmallerr.res";
//   const char *afterfile="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/millepede.res";
//   const char *afterfile="/afs/cern.ch/work/q/qyan/work/alignment/refine3itersmaller.res";
//   const char *afterfile="/afs/cern.ch/work/q/qyan/work/alignment/refine3itersmaller.res";
//   const char *afterfile="/afs/cern.ch/work/q/qyan/work/alignment/refine3itersmallernewcons.res";
//   const char *afterfile="merge3itersmallerr_after2.res";
//   const char *afterfile="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcons/4iter/millepede.res";
//   const char *afterfile="merge3itersmallerr_after2.res";
//   const char *afterfile="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcons/newgeom/millepede.res";
   const char *alignfile[2]={
//---MC
#if defined (AFTERALIGN2)
//     "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2MC.res",
//     "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2oladMC.res",
     "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2oladfixalphaMC.res",
#elif defined (USENOMS) && defined (SAMEL19ERROR)
     "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mcext/3itershearpresigfixsenfixoptloose/nomsne19/millepede.res",
#elif defined (USENOMS)
     "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mcext/3itershearpresigfixsenfixoptloose/noms/millepede.res",
#else
//     "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mcext/3itershearpresigfixsenfixoptloose/millepede.res",
//     "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mcext/3itershearpresigfixsenfixoptloose/inv/millepede.res",
     "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mcext/3itershearpresigfixsenfixoptloose/inv2fixalpha/millepede.res",
//     "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mcext/3itershearpresigfixsenfixoptloose/inv2/millepede.res",
#endif
//---Data
#if defined (AFTERALIGN3)
//    "/afs/cern.ch/work/q/qyan/work/alignment/after/after2/merge3itersmallerrext_after3.res",
    "/afs/cern.ch/work/q/qyan/work/alignment/after/after2/merge3itersmallerrext_after3inv2oladfixalpha.res",
#elif defined (AFTERALIGN2) && defined (USENOMS) && defined (SAMEL19ERROR)
    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_nomsne19after2.res",
#elif defined (AFTERALIGN2) && defined (USENOMS)
    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_nomsafter2.res",
#elif defined (AFTERALIGN2)
//    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2.res",
//     "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv.res",
//     "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2fixalpha.res",
//     "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2fixalphaloose.res",
//     "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2.res",
//     "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2fixsen19.res",
     "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2fixsen19x.res",
//     "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv3.res",
//     "/afs/cern.ch/work/q/qyan/work/alignment/after/bac/merge3itersmallerrext_after2inv2.res",
//     "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv3loose.res",
//     "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2loose.res",
//    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2oladfixalpha.res",
//    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2oladfixalphaloose.res",
//    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2olad.res",
//    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2oladloose.res",
#elif defined (USENOMS) && defined (SAMEL19ERROR)
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/nomsne19/millepede.res",
#elif defined (USENOMS)
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/noms/millepede.res",
#else
//     "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/inv2fixalpha/millepede.res",
//     "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/inv2fixalpha/loose/millepede.res",
     "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/inv2/millepede.res",
//     "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/inv2/loose/millepede.res",
#endif
   };
//   const char *afterfile="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcons/testrtol/millepede.res";
//   const char *afterfile="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcons/testdfmet/millepede.res";
//   const char *afterfile="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/millepede.res";
   const char *afterfile=(analysis->GetQRunManager()->qisreal)?alignfile[1]:alignfile[0];
   cout<<"afterfile="<<afterfile<<endl;
   analysis->tkfinal->LoadAlignModulePar(afterfile);
//   analysis->tkfinal->LoadAlignModulePar("/afs/cern.ch/work/q/qyan/work/alignment/test/merge2iterpede.res");
//   analysis->tkfinal->LoadAlignModulePar("/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/minres2iter/millepede.res");
//   analysis->tkfinal->LoadAlignModulePar("/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/minres3iter/millepede.res");
/*   long int btime=1167609600;  //manual set to 2007-01-01
   long int etime=1900000000-1;//manual set to 2030-03-17
   long int ptime=(btime+etime)/2;
   int vers=4;
   analysis->tkfinal->UseAlignVersion(vers);
   analysis->tkfinal->LoadAlignModuleTDV(ptime)*/
#endif
#endif
//----Book Histo
   analysis->BookHistos(outfile);
//----Exclude Bad Run
#if defined (USEP4ISS)
   analysis->SelectGoodRun(1);
#elif defined (USEBEFORENEWEXT)
   analysis->SelectGoodRun(5);
#elif defined (USEAFTERNEWEXT)
   analysis->SelectGoodRun(50);
#elif defined (USESHZD)
   analysis->SelectGoodRun(90);
#else
   analysis->SelectGoodRun();
#endif
///---Process
   int nstat=analysis->ProcessEvents(num);
   if(analysis->milleFile)delete analysis->milleFile;
   if(nstat==0){
//----Process Time
     analysis->Save();
     if(outdir){
       int cpstat=cpofile(milldata.Data(),outdir,0);
       if(cpstat==0)cpstat=cpofile(outfile,outdir,0);
       if(cpstat!=0)nstat=3;//cp error
     }
   }
   else {cerr<<"Sum1_AMS_CINT::error nstat="<<nstat<<endl;}
   cout<<"nladder="<<analysis->ladnev.size()<<" nladderx="<<analysis->ladnevx.size()<<endl;
   return nstat;
}


