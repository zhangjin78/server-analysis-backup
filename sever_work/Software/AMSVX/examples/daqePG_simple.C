///   \example stlv.C
///
#define stlv_cxx
#define _PGTRACK_
#include "root_RVSP.h"
#include "HistoMan.h"
#include <fstream>
/** 
 * This is an example of how to work with AMS Root Files. 
 *  \sa stlv
 */



///  This is an example of user class to process AMS Root Files 
/*!
  This class is derived from the AMSEventR class. \n

  The following member functions must be called by the user: \n
  void UBegin():       called everytime a loop on the tree starts,
  a convenient place to create your histograms. \n
  void UProcessFill(): called in the entry loop for all entries \n
  void UTerminate():   called at the end of a loop on the tree,
  a convenient place to draw/fit your histograms. \n

  The following member function is optional: \n
  bool UProcessCut(): called in the entry loop to accept/reject an event \n

  To use this file, try the following session on your Tree T: \n
  Root > T->Process("stlv.C+") -> compile library if not up to date \n
  Root > T->Process("stlv.C++") -> force always compilation \n
  Root > T->Process("stlv.C")  -> non-compiled (interpreted) mode \n
 */
static int runs[1000];
static int events[1000];
static int nruns=-1;
class daqePG_simple : public AMSEventR {
	public :

		daqePG_simple(TTree *tree=0){ };


		/// This is necessary if you want to run without compilation as:
		///     chain.Process("stlv.C");
#ifdef __CINT__
#include <process_cint.h>
#endif

		/// User Function called before starting the event loop.
		/// Book Histos
		virtual void    UBegin();

		/// User Function called to preselect events (NOT MANDATORY).
		/// Called for all entries.
		virtual bool    UProcessCut();
		virtual bool    UProcessStatus(unsigned long long status);

		/// User Function called to analyze each event.
		/// Called for all entries.
		/// Fills histograms.
		virtual void    UProcessFill();

		/// Called at the end of a loop on the tree,
		/// a convenient place to draw/fit your histograms. \n
		virtual void    UTerminate();
};


//create here pointers to histos and/or array of histos



void daqePG_simple::UBegin(){
	cout << " Begin calling"<<endl;
	for(int i=0;i<sizeof(runs)/sizeof(runs[0]);i++)runs[i]=0;

	//here create histograms
	hman.Enable();
        hman.Add(new TH1F("nTrTrack","nTrTrack",10,0,10));
        hman.Add(new TH1F("nTrTrackHit","nTrTrackHit",10,0,10));
        hman.Add(new TH1F("Rigidity","Rigidity",200,-1000,1000));
        hman.Add(new TH1F("Rigidity2","Rigidity2",200,-1000,1000));
	cout << " Begin called now"<<endl;



}
///*
bool daqePG_simple::UProcessStatus(unsigned long long status){
	if( !(status & 3) ||  !((status>>4)&1))return false;
	else return true;
}
//*/
// UProcessCut() is not mandatory
bool daqePG_simple::UProcessCut(){
	/*
	   static char f[1024]="";
	   const char * fname=_Tree->GetCurrentFile()->GetName();

	   if(strcmp(f,fname)){
	   cout <<" getfile "<<_Tree->GetCurrentFile()->GetName()<<endl;
	   strcpy(f,fname); 
	   }
	 */
	if (nParticle()==0  || nTrTrack()==0   ) return false;
	return true;
}

void daqePG_simple::UProcessFill()
{
	try{

		// User Function called for all entries .
		// Entry is the entry number in the current tree.
		// Fills histograms.
		//cout<<Run()<<" "<<Event()<<endl; 
		static int runo=0;
		//#pragma omp threadprivate (runo)
		if(Run()!=runo){
			cout <<"  tracks found run "<<Run()<<endl;
			runs[++nruns]=Run();
			events[nruns]=0;
			runo=Run();
		}   
		events[nruns]++;
		//if(events[nruns]>10000)return;
		bool cuts[23];
	        hman.Fill("nTrTrack",nTrTrack());
                for (int jj=0;jj<nTrTrack();jj++){
                   TrTrackR & tr=TrTrack(jj);
                   hman.Fill("Rigidity",tr.GetRigidity());
                   hman.Fill("nTrTrackHit",tr.GetNhits());
                   int fitcode=tr.iTrTrackPar(7,0,30);
	           if(fitcode>0)
                   hman.Fill("Rigidity2",tr.GetRigidity(fitcode));
		}
	}
	catch (...)
	{
		static int i=0;
		if(i++<100);cout <<"  daqe exception catched "<<Run()<< " " <<Event()<<endl;
		//abort();
		return ;
	}

}


void daqePG_simple::UTerminate()
{
	int evt=0;
	for(int i=0;i<sizeof(runs)/sizeof(runs[0]);i++){
		if(runs[i]){
			evt+=events[i];
			cout <<runs[i]<<" "<<events[i]<<" "<<evt<<endl;
		}
		else break;  
	}
	for(int i=0;i<sizeof(runs)/sizeof(runs[0]);i++){
		if(runs[i]){
			cout <<runs[i]<<",";
		}
		else break;
	}
	cout <<endl;
}

