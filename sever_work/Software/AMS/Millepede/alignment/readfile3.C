#include <cstdio>
#include <string>
#include "root_RVSP.h"
#include "amschain.h"
#include "TSystem.h"
#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>

//---Load List From File
void  LoadList(const char *file,map <string,unsigned int > &runlist){

     runlist.clear();
//--Clean File
     ifstream tsfile(file,ios::in);
     if(!tsfile)return;
//---
     unsigned int run;
     string fname;
     while (tsfile.good()){
       tsfile>>run;
       tsfile>>fname;
       if(!tsfile.good())continue;
//----cut string to root
       size_t bp=fname.rfind("/");
       size_t ep=fname.rfind(".root");
       if(bp==string::npos)bp=0;
       else                bp++;
       if(ep==string::npos||ep<=bp)continue;
       string cfname=fname.substr(bp,ep+5);
//---
       runlist[cfname]=run;
     }
     tsfile.close();
}

//---Search List
int SearchList(const char *listd,string fname){//0 not process--- 1 process -1 Error

    if(listd==0)return 1;

//--Cut String To Run
     size_t bp=fname.rfind("/"); 
     size_t ep=fname.rfind(".root");
     if(bp==string::npos)bp=0;
     else                bp++;
     if(ep==string::npos||ep<=bp){
        cerr<<"Error Search Input="<<fname<<endl;
        return -1;//Error for this run
     } 
     string cfname=fname.substr(bp,ep+5);
//----Cut To Run+.0000
     string cname = fname.substr(bp,ep-bp);//size 
     ep=cname.find(".");
     if(ep==string::npos){ep=fname.size()-1;}
//----Cut To Run
     string cname1=cname.substr(0,ep);
     int run=atoi(cname1.c_str());
     if(run==0||run<0){
        cerr<<"Error Search Input="<<fname<<endl;
        return -1;
     }

//---Now Search 
     static map <string,unsigned int > runlist;     
     static int pry=0,prm=0,prd=0;
     time_t tn=run;
     tm *tmp=gmtime(&tn);
///-Now Time Load
     if((tmp->tm_year!=pry||tmp->tm_mon!=prm||tmp->tm_mday!=prd)){//Change
         char  nfile[256],nfile1[1000];
         strftime(nfile,80,"%Y_%j",tmp);
         sprintf(nfile1,"%s/Run_%s-24H.cvs",listd,nfile);
         LoadList(nfile1,runlist);
         pry=tmp->tm_year; prm=tmp->tm_mon; prd=tmp->tm_mday;
     }

    map <string,unsigned int > ::iterator runit=runlist.find(cfname);
    int ispro;
    if(runit==runlist.end()){//not fond inlist should process
       ispro=1;
    }
    else { //exist should not process
      ispro=0;
   }

    return ispro;
}


//--------Write List
int WriteList(const char *listd,map <string,unsigned int > &runlistn){

   if(listd==0){return runlistn.size();}

   map <string,unsigned int > ::iterator it;
   char  nfile[256],nfile1[1000],nfile2[1000];
//---Write To previous+New Add
   for(int iw=0;iw<2;iw++){
     FILE * fp=0;
     int pry=0,prm=0,prd=0;
     for(it=runlistn.begin();it!=runlistn.end();it++){
       time_t tn=(*it).second;
       tm *tmp=gmtime(&tn);
       if((tmp->tm_year!=pry||tmp->tm_mon!=prm||tmp->tm_mday!=prd)){//Change
         if(fp){
           flock(fp->_fileno,LOCK_UN);
           fclose(fp);
           fp=0;
         }
         pry=tmp->tm_year; prm=tmp->tm_mon; prd=tmp->tm_mday;
         strftime(nfile,80,"%Y_%j",tmp);
         if(iw==0)sprintf(nfile1,"%s/Run_%s-24H.cvs",listd,nfile);
         else     sprintf(nfile1,"%s/RunN_%s-24H.cvs",listd,nfile);
         fp=fopen(nfile1,"a+");
         cout<<"otxt_name="<<nfile1<<" fp="<<fp<<endl;
         int fstat=flock(fp->_fileno,LOCK_EX);
//----
      }
      fprintf(fp,"%u %s\n",(*it).second,(*it).first.c_str());
      cout<<(*it).second<<" "<<(*it).first.c_str()<<" fp="<<fp<<endl;
     }
     if(fp){
       flock(fp->_fileno,LOCK_UN);
       fclose(fp);
       fp=0;
     }
   }

   return runlistn.size();
}


///-------Read File
int readfile(const char *ifile, vector<string> &sfile,int nroot=-1,int castype=0,const char *otxt=0,int rmode=0){

   sfile.clear();
//----
   ifstream infile;
   string ifname;
   char rootfile[1000];
   infile.open(ifile);
   if( !infile){
      cerr<<"infile open cerr!"<<" "<<ifile<<endl;
      exit(-1);
   }
   cout<<"readfile:"<<ifile<<" rmode="<<rmode<<endl;
//---
   int cont=0;
   while(infile.good()){
      infile.getline(rootfile,1000,'\n');
      if(strcmp(rootfile,"")==0)continue;
      ifname=rootfile;
      size_t fond=ifname.find(".root");
      if(fond==string::npos)continue;
      fond=ifname.find("castor");
      if(fond!=string::npos){
        if(castype==0)ifname="root://castorpublic.cern.ch//"+ifname;
        else          ifname="rfio:"+ifname;
//        ifname+="?svcClass=amsuser";
      }
      fond=ifname.find("eos");
      if(fond!=string::npos){
         ifname="root://eosams/"+ifname;
      }
//--Not Process
      if(otxt!=0){
        int ispro=SearchList(otxt,ifname);
        if(ispro!=1){
          cout<<"ncont="<<ifname<<endl;continue;
        }
      }
//---
      cout<<"cont="<<cont<<" "<<ifname<<endl;
      if(rmode<10)sfile.push_back(ifname);
      cont++;
      if((nroot>=0)&&(cont>=nroot))break;
   }
   infile.close();
   if(cont>=1&&rmode>=10){//force to add all non-zero
     readfile(ifile,sfile,nroot,castype,0,rmode%10);
   }
//--   
   return sfile.size();
}


///-------Read File
int readfile(const char *ifile, AMSChain &ch,int nroot=-1,int castype=0,const char *otxt=0,int rmode=0){

   vector<string> sfile;
   int nread=readfile(ifile,sfile,nroot,castype,otxt,rmode);
   for(unsigned int i=0;i<sfile.size();i++){
     string &ifname=sfile[i];
     ch.Add(ifname.c_str());
   }
   return nread;
}

//------
int readfile(const char *ifile, TChain &ch,int nroot=-1,int mode=0,int castype=0,const char *otxt=0,int rmode=0){

   vector<string> sfile;
   int nread=readfile(ifile,sfile,nroot,castype,otxt,rmode);
   for(unsigned int i=0;i<sfile.size();i++){
     string &ifname=sfile[i];
     if(mode!=0)ch.AddFile(ifname.c_str());
     else       ch.Add(ifname.c_str());
   }
   return nread;
}

//---cp file to eos or castor
int cpofile(const char *ofile, const char *ofilecp,int isrm=0){
  
  if(ofilecp==0){cout<<"No CP Output"<<endl;return -1;}

//---CP
   std::string sofilecp=ofilecp;
   struct stat st_buf;
   if((sofilecp.back()=='/')||(stat(sofilecp.c_str(),&st_buf)==0&&S_ISDIR(st_buf.st_mode))){//ofilecp is a directory
     if((sofilecp.back()!='/'))sofilecp+='/';
     string sofile=ofile;
     size_t bp=sofile.rfind("/");
     if(bp==string::npos)bp=0;
     else                bp++;
     sofilecp+=sofile.substr(bp);//+dirname(ofile)
   }
//----
   string cpopt;
   string rmoptcp;
   size_t fonde=sofilecp.find("/eos");
   size_t fondc=sofilecp.find("/castor"); 
   if (fonde!=string::npos){
/*     cpopt="/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select cp";
     rmoptcp="/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select rm";*/
     cpopt="eos cp";
     rmoptcp="eos rm";
   }
   else if(fondc!=string::npos){
     cpopt="rfcp";
     rmoptcp="rfrm";
   }
   else {
     cpopt="cp";
     rmoptcp="rm";
   }
   cpopt=cpopt+" "+ofile+" "+sofilecp;
   rmoptcp=rmoptcp+" "+sofilecp;
   cout<<"cpopt=" <<cpopt<<endl; 
   int cperror=system(cpopt.c_str());
///---check copy again
//   int cperror=-2;
   const int ncp=3;
   for(int icp=0;icp<=ncp;icp++){
     TString lsopt;
//     if     (fonde!=string::npos)lsopt="/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select ls";
     if     (fonde!=string::npos)lsopt="eos ls";
     else if(fondc!=string::npos)lsopt="nsls ls";
     else                        lsopt="ls";
     lsopt=lsopt+" "+sofilecp;
     sleep(1);
     TString froot=gSystem->GetFromPipe(lsopt);
     TObjArray *rootarr=froot.Tokenize("\n");
     if(rootarr->GetEntries()<=0&&cperror==0)cperror=-2;
     if(cperror==0){cout<<"success-lsopt="<<lsopt.Data()<<" cperror="<<cperror<<endl;break;}
     cout<<"failed-lsopt="<<lsopt.Data()<<" cperror="<<cperror<<endl;
     if(icp>=ncp)break;
     sleep(10);
     cout<<"try"<<(icp+1)<<" cpopt=" <<cpopt<<endl;
     cperror=system(cpopt.c_str());
   }
   if(cperror!=0){
     cout<<"rmoptcp=" <<rmoptcp<<endl;
     system(rmoptcp.c_str());
     return cperror;
   }
///---
   if(isrm==0)return 0; 

//---RM
   string rmopt="rm"; 
   rmopt=rmopt+" "+ofile;
   cout<<"rmopt="<<rmopt<<endl;
   system(rmopt.c_str()); 

   return 0; 
}
