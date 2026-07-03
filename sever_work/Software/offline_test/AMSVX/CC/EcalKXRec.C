#include "EcalKXRec.h"

EcalKXFastRec* EcalKXRec::pEcalKXFastRec = 0;
EcalKX3DRec* EcalKXRec::pEcalKX3DRec = 0;
EcalKXRec* EcalKXRec::Head = 0;
Int_t EcalKXRec::N_Shwr = 0;

EcalKXRec* EcalKXRec::GetHead(){
	if( Head == NULL ){
		Head = new EcalKXRec();
	}
	if( pEcalKXFastRec == NULL )
		pEcalKXFastRec = new EcalKXFastRec();
	if( pEcalKX3DRec == NULL )
		pEcalKX3DRec = new EcalKX3DRec();
	return Head;
}

EcalKXFastRec* EcalKXRec::GetEcalKXFastRec() {
	return GetHead()->pEcalKXFastRec;
}

EcalKX3DRec* EcalKXRec::GetEcalKX3DRec() {
	return GetHead()->pEcalKX3DRec;
}

void EcalKXRec::ProcessAMSEvent(AMSEventR *_pev, Int_t isHadronicShower){
	if( _pev == NULL ){
		cerr << "--ERROR--EcalKXRec::ProcessAMSEvent--NULL-Pointer-To-AMSEvent" << endl;
		exit(-1);
	}
	// Fast reconstruction
	pEcalKXFastRec->ReadADCMapFromAMSEvent(_pev);
	pEcalKXFastRec->DoFastReconstruction();

	Int_t _showershape_type = pEcalKXFastRec->DataType;
	if( _pev->Version() >= 1118 && _pev->nMCEventg() > 0 ){ // MC with shower shape tuned to data
		_showershape_type = EcalKX3DRecDB::_ISS_DATA;
	}
	Do3DReconstruction(isHadronicShower, _showershape_type);
}

void EcalKXRec::ProcessADCMap(UInt_t utime, Float_t ADCH[][72], Float_t ADCL[][72], Int_t Status[][72], Int_t dataType, Int_t isHadronicShower, Int_t _showershape_type){
	// dataType can be EcalKX3DRecDB::_ISS_DATA or _TB_DATA or _MC_DATA.
	// dataType is used in FastRec to assign different calibration parameters
	// _showershape_type is used in 3DRec to choose shower shape parameters for reconstruction
	pEcalKXFastRec->ReadADCMap(utime, ADCH, ADCL, Status, dataType);
	pEcalKXFastRec->DoFastReconstruction();
	if( _showershape_type==-1 ){ // by default set to the datatype is FastRec 
		_showershape_type = pEcalKXFastRec->DataType;
	}
	Do3DReconstruction(isHadronicShower, _showershape_type);
}

void EcalKXRec::Do3DReconstruction(Int_t isHadronicShower, Int_t dataType){// 3D fit reconstruction
	pEcalKX3DRec->SetCluster( pEcalKXFastRec->EdepAttC, pEcalKXFastRec->Status, 1 ); // energy unit in GeV
	if(isHadronicShower>0) pEcalKX3DRec->SetHadronicFlag();
	pEcalKX3DRec->SetDataType(dataType);
	pEcalKX3DRec->ShowerRec(0);
	pEcalKX3DRec->ShowerCombineFit();
//	pEcalKX3DRec->OverlapShowerRec();
	pEcalKX3DRec->Summary();
	N_Shwr = pEcalKX3DRec->N_Shwr;
}

