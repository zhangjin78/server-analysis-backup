{
	//load my scrip
	//gInterpreter->AddIncludePath(gSystem->ExpandPathName("/afs/cern.ch/user/c/chenyam/analisi/PhotonRead"));
	//gInterpreter->AddIncludePath(gSystem->ExpandPathName("/afs/cern.ch/user/c/chenyam/analisi/"));
	//gInterpreter->AddIncludePath(gSystem->ExpandPathName("/afs/cern.ch/user/c/chenyam/analisi/include"));
	
	//load the AMS software
	gInterpreter->AddIncludePath(gSystem->ExpandPathName("$AMSSRC/include"));
	//char libname[255];
	//sprintf(libname,"$AMSWD/lib/%s/ntuple_slc4_PG.so",gSystem->GetBuildArch()); 
	//// for loading AMSRoot libraries
	//gSystem->Load(libname);
	gSystem->Load("$AMSSRC/lib/linuxx8664icc5.34/ntuple_slc6_PG.so");
	//gSystem->Load("$AMSSRC/lib/linuxx8664icc6.14/ntuple_slc6_PG.so"); //--complie with root6
}

//------------
//rootlogon for lxplus
//------------
