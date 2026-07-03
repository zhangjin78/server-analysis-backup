#include "TrCalDB.h"
#include "TrLadCal.h"
#include "TrHistoMan.h"
#include "TkDBc.h"

#include "MonitorUI.h"
#include "GenericSlider.h"
#include "MonSlider.h"
#include "CalSlider.h"

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TLatex.h"

#include <cstring>

#include <stdio.h>
#include <time.h>

int main(int argc, char **argv){
  if (argc<2) {
    printf("Usage: %s <File> [All/Cal/Mon=0/1/2] [GraphNumber] [HwId]\n",argv[0]);
    return 0;
  }
  // type
  int type = 0;
  if (argc>2) type = atoi(argv[2]);
  // graphtype
  int graphtype = 1;
  if (argc>3) graphtype = atoi(argv[3]);
  // hwid
  int hwid = 0;
  if (argc>4) hwid = atoi(argv[4]);
  // create database
  TkDBc::CreateTkDBc();
  TkDBc::Head->init();
  // set style
  MonitorUI::SetStyle(); 
  // open root file 
  TFile* file = new TFile(argv[1],"read");
  if ( (file->FindObjectAny("TrCalDB")!=0)&&( (type==0)||(type==1) ) ) {
    CalSlider* calslider = new CalSlider("CalSlider","CalSlider");
    calslider->setRootFile(argv[1]);
    calslider->selectGraph(0,graphtype);
    TrCalDB* trcaldb = (TrCalDB*) file->FindObjectAny("TrCalDB");
    calslider->showHwId(hwid);
    time_t rawtime = trcaldb->GetRun();
    struct tm  ts;
    char   time_human[80];
    ts = *gmtime(&rawtime);
    strftime(time_human, sizeof(time_human), "%a %h %d %H:%M:%S %Y", &ts);
    //    printf("%s\n", time_human);
    TCanvas* canvas = calslider->GetCanvas();
    if (canvas) {
      canvas->cd();
      TLatex latex;
      latex.SetTextSize(0.025);
      latex.SetTextAlign(13);  //align at top
      latex.DrawLatex(.7,.99,Form("(Cal %010d) - %s", trcaldb->GetRun(), time_human));
      printf("(Cal %010d) - %s\n", trcaldb->GetRun(), time_human);
      canvas->Print(Form("Cal_%010d_%02d_%03d.png", trcaldb->GetRun(), graphtype,hwid));
    }
  }
  //  if ( (file->FindObjectAny("TrOnlineMon")!=0)&&(file->FindObjectAny("timentuple")!=0)&&( (type==0)||(type==2) ) ) {
  //  printf("%p\n",  file->FindObjectAny("TrOnlineMon"));
  if ( file->FindObjectAny("TrOnlineMon")&&((type==0)||(type==2))) {
    MonSlider* monslider = new MonSlider("MonSlider","MonSlider");
    monslider->setRootFile(argv[1], true);
    monslider->selectGraph(0,graphtype);
    if (hwid!=0) monslider->showHwId(hwid);
    TNtuple* ntuple = (TNtuple*) file->FindObjectAny("timentuple");
    TrHistoManHeader* header = (TrHistoManHeader*) file->FindObjectAny("TrOnlineMonHeader");
    //    printf("%p\n", header);
    if (header && ntuple) {
      if ( (header->GetNRunNumbers()>0)&&(header->GetNFileNames()>0) ) {
	// first event time
	TIME_EVENT time_event;
	ntuple->SetBranchAddress("timebranch",&time_event);
	ntuple->GetEntry(0);
	int first_time = (int) time_event.Time;
	time_t t_first_time = time_event.Time;
	// first run number 
	int run_number = header->GetRunNumber(0);
	// parse file name to find block numbers
	const char* pointer1 = strrchr(header->GetFileName(0),'/')-4; 
	char copy1[5];
	strncpy(copy1,pointer1,4);
	copy1[4] = '\0';
	int dir = atoi(copy1);
	const char* pointer2 = strrchr(header->GetFileName(0),'/')+1;
	char copy2[4];
	strncpy(copy2,pointer2,3);
	copy2[3] = '\0';
	int block = atoi(copy2);
	//	printf("%ld %s\n", (long int)(gmtime(&t_first_time)), asctime(gmtime(&t_first_time)));
	TString first_time_human = asctime(gmtime(&t_first_time));
	first_time_human.ReplaceAll('\n', "");
	first_time_human.ReplaceAll(" ", "_");
	first_time_human.ReplaceAll(":", "");
	TString first_time_human_spaces = asctime(gmtime(&t_first_time));
	first_time_human_spaces.ReplaceAll('\n', "");
	TCanvas* canvas = monslider->GetCanvas();
	if (canvas) {
	  canvas->cd();
	  TLatex latex;
	  latex.SetTextSize(0.025);
	  latex.SetTextAlign(13);  //align at top
	  latex.DrawLatex(.65,.95,Form("%04d/%03d (Run %010d) - %s", dir, block, run_number, first_time_human_spaces.Data()));
	  printf("%04d/%03d (Run %010d) - %s\n", dir, block, run_number, first_time_human_spaces.Data());
	  canvas->Print(Form("Mon_%04d%03d_%010d_%010d_%s_%02d_%03d.png", dir, block, run_number, first_time, first_time_human.Data(),graphtype,hwid));
	}
      }
    }
    else {
      TString filename_stripped = file->GetName();
      filename_stripped.ReplaceAll(".root", "");
      //      printf("%s\n", filename_stripped.Data());
      std::string s_filename_stripped = filename_stripped.Data();
      std::string base_filename_stripped = s_filename_stripped.substr(s_filename_stripped.find_last_of("/\\") + 1);
      monslider->GetCanvas()->Print(Form("Mon_%s_%02d_%03d.png", base_filename_stripped.c_str(), graphtype, hwid));
    }
  }
  return 0;
}

