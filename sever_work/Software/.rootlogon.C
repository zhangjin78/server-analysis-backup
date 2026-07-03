{
///--Author Qi Yan (qyan@cern.ch)


if(!gSystem->Getenv("AMSWD")){cerr<<"<<-----Error $AMSWD not set"<<endl; return; }


std::string rootv=gROOT->GetVersion();
rootv= rootv.substr(0,4);
if(rootv.find("5.27")!=std::string::npos)rootv="";

TString sysv=gSystem->GetFromPipe("cat /etc/redhat-release | awk -F'[^0-9]+' '{ print $2 }'");

char libname[255];
sprintf(libname,"$AMSWD/lib/%s%s_%s/ntuple_slc6_PG.so",gSystem->GetBuildArch(),rootv.c_str(),sysv.Data());
cout<<"AMSLib Name="<<libname<<endl;


//-----
gInterpreter->AddIncludePath(gSystem->ExpandPathName("$AMSWD/include"));
gInterpreter->AddIncludePath(gSystem->ExpandPathName("$AMSWD/GenFit/include"));
gInterpreter->AddIncludePath(gSystem->ExpandPathName("$QToolWD"));
//gSystem->AddIncludePath("-I$ROOTSYS/tmva/test");


//-----
TString intelds;
const char *inteld=gSystem->Getenv("INTELDIR");
if(inteld)intelds=inteld;


const int nc=4;
const char *fc[]={"icc","icpc","g++","icpx"};
for(int im=0;im<2;im++){
  for(int ic=0;ic<nc;ic++){
    TString umake;
    if(im==0)umake=gSystem->GetMakeSharedLib();
    else     umake=gSystem->GetMakeExe();
    TString flagopt=gSystem->GetFlagsOpt();
    if(!umake.Contains(fc[ic]))continue;
    bool newmake[2]={0}; 
    if(!umake.Contains("-D_GLIBCXX_USE_CXX11_ABI=0")){
      TString flagc=TString(fc[ic])+" -D_GLIBCXX_USE_CXX11_ABI=0";
      umake.ReplaceAll(fc[ic],flagc);
      newmake[0]=1;
    }
    if(!flagopt.Contains("-D_GLIBCXX_USE_CXX11_ABI=0")){
      flagopt.Append(" -D_GLIBCXX_USE_CXX11_ABI=0");
      newmake[1]=1;
    }
    if(ic<=1&&intelds.Contains("oneAPI")){
      umake.ReplaceAll(fc[ic],"icpx");
      newmake[0]=1;
      if(umake.Contains("-fp-model source -fast-transcendentals")){
        umake.ReplaceAll("-fp-model source -fast-transcendentals","-fp-model precise");
      }
      if(flagopt.Contains("-fp-model source -fast-transcendentals")){
        flagopt.ReplaceAll("-fp-model source -fast-transcendentals","-fp-model precise");
        newmake[1]=1;
      }
      umake.ReplaceAll(" -wd1476","");
      umake.ReplaceAll(" -wd1572","");
      umake.ReplaceAll(" -wd279", "");
      umake.ReplaceAll(" -wd873", "");
      umake.ReplaceAll(" -wd2536","");
      umake.ReplaceAll(" -wd597", "");
      umake.ReplaceAll(" -wd1098","");
      umake.ReplaceAll(" -wd1292","");
      umake.ReplaceAll(" -wd1478","");
      umake.ReplaceAll(" -wd3373","");
    }
    if(newmake[0]){
      if(im==0){
        cout<<endl;
        cout<<"makesharedlib befor="<<gSystem->GetMakeSharedLib()<<endl;
        gSystem->SetMakeSharedLib(umake.Data());
        cout<<"makesharedlib after="<<gSystem->GetMakeSharedLib()<<endl;
      }
      else {
        cout<<"makeexe befor="<<gSystem->GetMakeExe()<<endl;
        gSystem->SetMakeExe(umake.Data());
        cout<<"makeexe after="<<gSystem->GetMakeExe()<<endl;
      }
      cout<<endl;
    }
    if(newmake[1]){
      cout<<"flagopt befor="<<gSystem->GetFlagsOpt()<<endl;
      gSystem->SetFlagsOpt(flagopt.Data());
      cout<<"flagopt after="<<gSystem->GetFlagsOpt()<<endl;
      cout<<endl;
    }
    break;
  }
}


//-----
gSystem->Load(libname);


}
