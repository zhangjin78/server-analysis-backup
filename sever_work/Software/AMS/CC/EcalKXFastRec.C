#include "EcalKXFastRec.h"

EcalKXCalib *EcalKXFastRec::pEcalKXCalib=0;
EcalStaticCalibPar *EcalKXFastRec::pEcalStaticCalibPar=0;
EcalDynamicCalibPar *EcalKXFastRec::pEcalDynamicCalibPar=0;
EcalCellStatusParUnit *EcalKXFastRec::pEcalCellStatusParUnit=0;
bool EcalKXFastRec::IsSkipCalibration=0;
bool EcalKXFastRec::IsSkipAttenuationCorrection=0;

void EcalKXFastRec::Reset(){
	Run = Event = 0;
	Nhits = NhitsBad = 0;
	memset( ADCH, 0, sizeof(ADCH) );
	memset( ADCL, 0, sizeof(ADCL) );
	memset( PedH, 0, sizeof(PedH) );
	memset( PedL, 0, sizeof(PedL) );
	memset( Status, 0, sizeof(Status) );
	memset( Edep, 0, sizeof(Edep) );
	memset( EdepAttC, 0, sizeof(EdepAttC) );
	memset( LayAmp, 0, sizeof(LayAmp) );
	memset( LayLRR, 0, sizeof(LayLRR) );
	memset( LayCrd, 0, sizeof(LayCrd) );
	memset( LayPos, 0, sizeof(LayPos) );
	memset( LayEdep0, 0, sizeof(LayEdep0) );
	memset( LayPos0, 0, sizeof(LayPos0) );
	memset( LayLRR0, 0, sizeof(LayLRR0) );
	memset( LayCrd0, 0, sizeof(LayCrd0) );
	Z0 = X0 = Y0 = KX = KY = 0;
	N_Shwr = 0;
	memset(ShwrE0, 0, sizeof(ShwrE0));
	memset(ShwrEd0, 0, sizeof(ShwrEd0));
	memset(ShwrEd1, 0, sizeof(ShwrEd1));
	memset(ShwrX0, 0, sizeof(ShwrX0));
	memset(ShwrY0, 0, sizeof(ShwrY0));
	memset(ShwrZ0, 0, sizeof(ShwrZ0));
	memset(ShwrKX, 0, sizeof(ShwrKX));
	memset(ShwrKY, 0, sizeof(ShwrKY));
	memset(ShwrNhits, 0, sizeof(ShwrNhits));
}

void EcalKXFastRec::ReadADCMapFromAMSEvent( AMSEventR *pev ){
	Reset();
	// Used to read ISS/TB/MC event
	Time = pev->UTime();
	if( pev->nMCEventg() > 0 ){
		DataType = EcalKX3DRecDB::_MC_DATA; // MC simulation
		IsSkipCalibration = 0;
		IsSkipAttenuationCorrection = 0;
		MCVersion = pev->Version();
	}else{
		IsSkipCalibration = 0;
		IsSkipAttenuationCorrection = 0;
		if( Time < 1305759600 ) DataType = EcalKX3DRecDB::_TB_DATA;
		else                    DataType = EcalKX3DRecDB::_ISS_DATA;
	}

	EcalHitR echit;
	for(Int_t ihit=0; ihit<pev->nEcalHit(); ihit++){
		echit = pev->EcalHit(ihit);
		ADCH[echit.Plane][echit.Cell] = echit.ADC[0];
		ADCL[echit.Plane][echit.Cell] = echit.ADC[1];
		PedH[echit.Plane][echit.Cell] = echit.GetECALPed( echit.Plane, echit.Cell, 0 );
		PedL[echit.Plane][echit.Cell] = echit.GetECALPed( echit.Plane, echit.Cell, 1 );

		if( DataType==EcalKX3DRecDB::_MC_DATA && IsSkipCalibration && IsSkipAttenuationCorrection ){
			Edep[echit.Plane][echit.Cell] = echit.Edep;
		}
//		if( ADCH[echit.Plane][echit.Cell] > 0 ){
//			cout << Form("ADCH=%g, ADCL=%g, PedH=%g, PedL=%g", ADCH[echit.Plane][echit.Cell], ADCL[echit.Plane][echit.Cell], PedH[echit.Plane][echit.Cell], PedL[echit.Plane][echit.Cell] ) << endl;
//		}
	}
}

void EcalKXFastRec::ReadADCMap(Double_t _time, Float_t _adcH[][EcalKX3DRecDB::nCell], Float_t _adcL[][EcalKX3DRecDB::nCell], Int_t _stat[][EcalKX3DRecDB::nCell], Int_t _dataType){
	Reset();
	Time = _time;
	DataType = _dataType;
	if( DataType == EcalKX3DRecDB::_MC_DATA ){ MCVersion = _time; }
	memcpy( ADCH, _adcH, sizeof(ADCH) );
	memcpy( ADCL, _adcL, sizeof(ADCL) );
	memcpy( Status, _stat, sizeof(Status) );
}

void EcalKXFastRec::ReadADCMapFromToyEvent(Double_t _time, Float_t _edep[][EcalKX3DRecDB::nCell], Int_t _stat[][EcalKX3DRecDB::nCell]){
	Reset();
	Time = _time;
	DataType = EcalKX3DRecDB::_ToyMC_DATA;
	memcpy( Edep, _edep, sizeof(Edep) );
	memcpy( Status, _stat, sizeof(Status) );
}

int EcalKXFastRec::LoadCalibrationDataBase(){
	if( pEcalKXCalib == 0 ){
		pEcalKXCalib = EcalKXCalib::getHedaer();
	}
	if( DataType==EcalKX3DRecDB::_MC_DATA ){
		static UInt_t last_MCVersion = 0;
		if( last_MCVersion!= MCVersion ){
			cout << "--MC calibration loaded--MC version=" << MCVersion << endl;
			pEcalStaticCalibPar = pEcalKXCalib->getStaticCalibParMC(MCVersion);
			pEcalDynamicCalibPar = pEcalKXCalib->getDefaultDynamicCalibPar();
			pEcalCellStatusParUnit = pEcalKXCalib->getDefaultCellStatusParUnit();
			last_MCVersion = MCVersion;
		}
	}else if( DataType==EcalKX3DRecDB::_TB_DATA|| DataType==EcalKX3DRecDB::_ISS_DATA){
		pEcalStaticCalibPar = pEcalKXCalib->getStaticCalibPar(Time);
		pEcalDynamicCalibPar = pEcalKXCalib->getDynamicCalibPar(Time);
		pEcalCellStatusParUnit = pEcalKXCalib->getCellStatusParUnit(Time);
		if( pEcalStaticCalibPar && pEcalDynamicCalibPar && pEcalCellStatusParUnit ){
			return 1; // good
		}else{
			cerr << "--EcalKXFastRec--FAILURE--LoadCalibrationDataBase" << endl;
			cerr << "------------------------pEcalStaticCalibPar=" << pEcalStaticCalibPar << endl;
			cerr << "------------------------pEcalDynamicCalibPar=" << pEcalDynamicCalibPar << endl;
			cerr << "------------------------pEcalCellStatusParUnit=" << pEcalCellStatusParUnit << endl;
			return 0; // bad
		}
	}
	return 1;
}

/*****************************************************
 *                Calibration status                 *
 * Need improvement, usage EcalCellStatusPar databse *
 *****************************************************/
void EcalKXFastRec::DefineCellStatus(){
	for(int l=0; l<EcalKX3DRecDB::kNL; l++){
		for(int c=0; c<EcalKX3DRecDB::nCell; c++){
			// Permenantly bad cell
			if( (l==6||l==7) && (c==16||c==17) ) Status[l][c] |= EcalKX3DRecDB::DeadCell;
			if( (l==14)&&(c==65) ) Status[l][c] |= EcalKX3DRecDB::DeadCell;
			// Online calibration status
			if( !IsSkipCalibration && pEcalCellStatusParUnit )
				Status[l][c] |= pEcalCellStatusParUnit->cellstat[l][c];
			// High gain saturation
			if( DataType==EcalKX3DRecDB::_MC_DATA ){
				if( MCVersion==1118 || MCVersion==1119 ){ // the electronics saturation was set to 3830 (instead of 4095) for these two versions
					if( ADCH[l][c] > EcalKX3DRecDB::ADCHighGainSatuMC-265 ) Status[l][c] |= EcalKX3DRecDB::HighGainSaturated;
					if( ADCL[l][c] > EcalKX3DRecDB::ADCLowGainSatuMC-265  ) Status[l][c] |= EcalKX3DRecDB::LowGainSaturated;
				}else{
					if( ADCH[l][c] > EcalKX3DRecDB::ADCHighGainSatuMC ) Status[l][c] |= EcalKX3DRecDB::HighGainSaturated;
					if( ADCL[l][c] > EcalKX3DRecDB::ADCLowGainSatuMC  ) Status[l][c] |= EcalKX3DRecDB::LowGainSaturated;
				}
			}else{
				if( ADCH[l][c] + PedH[l][c] > EcalKX3DRecDB::ADCHighGainSatu ) Status[l][c] |= EcalKX3DRecDB::HighGainSaturated;
				if( ADCL[l][c] + PedL[l][c] > EcalKX3DRecDB::ADCLowGainSatu  ) Status[l][c] |= EcalKX3DRecDB::LowGainSaturated;
			}
			if( ADCH[l][c] > 0 ){
//				cout << Form("l=%d, c=%d, ADCH=%g, ADCL=%g, Status=%d", l, c, ADCH[l][c], ADCL[l][c], Status[l][c]) << endl;
			}
		}
	}
}

void EcalKXFastRec::GainCalibration(){
	double gain=1;
	if( !pEcalKXCalib || !pEcalStaticCalibPar || !pEcalDynamicCalibPar ){
		cerr << "---EcalKXFastRec--FAILURE--GainCalibration: DataBase Not Loaded " << endl;
		exit(-1);
	}else{
		for(int l=0; l<EcalKX3DRecDB::nLayer; l++){
			for(int c=0; c<EcalKX3DRecDB::nCell; c++){
				if( ADCH[l][c]>0 || ADCL[l][c]>0 ){
					if( Status[l][c] & EcalKX3DRecDB::DeadCell ){
//						cout << Form("L%02d C%02d is dead cell", l, c) << endl;
						Edep[l][c] = 0.;
					}else if( !( Status[l][c] & (EcalKX3DRecDB::HighGainSaturated | EcalKX3DRecDB::HighGainBadPedestal | EcalKX3DRecDB::HighGainNoisyChannel) ) ){ // use high gain
					  	Edep[l][c] = ADCH[l][c];
						Status[l][c] |= EcalKX3DRecDB::HighGainUsed;
					}else if( !( Status[l][c] & (EcalKX3DRecDB::LowGainBadPedestal | EcalKX3DRecDB::LowGainNoisyChannel) ) ){ // use low gain
						Edep[l][c] = ADCL[l][c]*pEcalStaticCalibPar->hlr[l][c];
						Status[l][c] |= EcalKX3DRecDB::LowGainUsed;
					}else{
//						cout << Form("--EcalKXFastRec::GainCalibration()--SetDummy: L%02d C%02d, stat=%d, ADCH=%g, ADCL=%g", l, c, Status[l][c], ADCH[l][c], ADCL[l][c]) << endl;
						Edep[l][c] = 0.;
						Status[l][c] |= EcalKX3DRecDB::Dummy;
						NhitsBad ++;
					}
//					}else if( !( Status[l][c] & (DynodeSaturated | DynodeBadPedestal | DynodeNoisyChannel ) ) ){ // use dynode
//						Edep[l][c] = ADCD[l][c]*pEcalStaticCalibPar->hdr[l][c];
//						Status[l][c] |= DynodeUsed;

					// apply gain correction
					gain = pEcalStaticCalibPar->gain[l][c] * pEcalDynamicCalibPar->dyngain[l][c] / EcalKXCalib::MIP2MeV;
					if( gain > 0 )
						Edep[l][c] /= gain;
					else{
						printf("pEcalStaticCalibPar->gain[%d][%d]=%.3f, pEcalDynamicCalibPar->dyngain[%d][%d]=%.3f\n", l, c, pEcalStaticCalibPar->gain[l][c], l, c, pEcalDynamicCalibPar->dyngain[l][c]);
						exit(-1);
//						cout << Form("Bad Calibration, l%02d, c%02d, ADCH=%5.2f, ADCL=%5.2f, Edep=%5.2f, gain=%5.3f", l, c, ADCH[l][c], ADCL[l][c], Edep[l][c], gain ) << endl;
		 				Edep[l][c] = 0.;
					}
					// ADC->MeV
				}
			}
		}
	}
}

void EcalKXFastRec::ClearCellStatus(Int_t l, Int_t c, Int_t stat_ask){
	Status[l][c] &= ~stat_ask;
}
int EcalKXFastRec::CheckCellStatus(Int_t l, Int_t c, Int_t stat_ask){
	return (Status[l][c] & stat_ask)>0;
}
void EcalKXFastRec::ClearAllCellStatus(Int_t stat_ask){
	for(int l=0; l<EcalKX3DRecDB::kNL; l++){
		for(int c=0; c<EcalKX3DRecDB::nCell; c++){
			Status[l][c] &= ~stat_ask;
		}
	}
}

void EcalKXFastRec::ShowerSeedFinding(Int_t iter){

	Double_t v, val;
  // Int_t lx, mx, ly, my, nm, nl;
  Int_t lx, mx, ly, my, nm;
	Int_t l, m;
	KX=0;
	KY=0;
//	Int_t nKY=0, nKX=0;
	Int_t nw=3;
	// seed layer in X projection
define_mX:
	v  = EcalKX3DRecDB::Cutoff_MIP;// maximum energy
	lx = -1; // lay
	mx = -1; // cell
	for (l=0; l<18; l++) {
		SeedCell[l] = -1;
		if( l%4 < 2 ) continue;
		for (m=0; m<EcalKX3DRecDB::nCell; m++) {
			if ( EdepBuffer[l][m]>v ) {
				v = EdepBuffer[l][m];
				lx = l;
				mx = m;
			}
		}
	}
	// check the consistence of the found X seed
	if ( lx==2 || lx==15) nm = 1;
	else                  nm = 0;
	if ( mx >= 0 ) {
		val = 0.;
		if( lx/2 == l/2 ) nw = 1;
		else              nw = 3;
		for (l=2; l<16; l+=4) {
//			if ( l>(lx-5) && l<(lx+5) ) {
				for (m=TMath::Max(0,mx-nw); m<=TMath::Min(71,mx+nw); m++) {
					if ( ((l != lx) || (m != mx)) && EdepBuffer[l][m]>0. ) {
						nm  += 1;
						val += EdepBuffer[l][m];
					}
				}
//			}
//			if ( (l+1)>(lx-5) && (l+1)<(lx+5) ) {
				for (m=TMath::Max(0,mx-nw); m<=TMath::Min(71,mx+nw); m++) {
					if ( (((l+1) != lx) || (m != mx)) && EdepBuffer[l+1][m]>0. ) {
						nm  += 1;
						val += EdepBuffer[l+1][m];
					}
//				}
			}
		}
		if ((nm < 4) || (v > 2*val) ) {
			EdepBuffer[lx][mx] = EcalKX3DRecDB::Cutoff_MIP;
//			printf("-->ShowerClustering(): set L=%d,C=%d to MIP, nm=%d, v=%8.3f, val=%8.3f\n", lx, mx, nm, v, val);
			goto define_mX;
		}
	}
	fEX = nm;

	// define X footprint
	if ( mx >= 0 ) {
		dEX = v;
		// define KX use the two layers in the same superlayer of seed
		v=0.;
		/*
		Int_t lx1 = lx%2==0?lx+1:lx-1;
		SeedCell[lx] = mx;
		for (m=mx-3; m<=mx+3; m++) {
			if (m>=0 && m<=71 && EdepBuffer[lx1][m]>v ) {
				v = EdepBuffer[lx1][m];
				SeedCell[lx1] = m;
			}
		}
		if( SeedCell[lx1] > 0 ){ // define KX
			Double_t n=EdepBuffer[lx][SeedCell[lx]],  ex=EdepBuffer[lx][SeedCell[lx]]*(SeedCell[lx]),
						n1=EdepBuffer[lx1][SeedCell[lx1]], ex1=EdepBuffer[lx1][SeedCell[lx1]]*(SeedCell[lx1]);
			if( SeedCell[lx] > 0  ){ n+=EdepBuffer[lx][SeedCell[lx]-1]; ex += EdepBuffer[lx][SeedCell[lx]-1]*(SeedCell[lx]-1); }
			if( SeedCell[lx] < 71 ){ n+=EdepBuffer[lx][SeedCell[lx]+1]; ex += EdepBuffer[lx][SeedCell[lx]+1]*(SeedCell[lx]+1); }
			if( SeedCell[lx1] > 0  ){ n1+=EdepBuffer[lx1][SeedCell[lx1]-1]; ex1 += EdepBuffer[lx1][SeedCell[lx1]-1]*(SeedCell[lx1]-1); }
			if( SeedCell[lx1] < 71 ){ n1+=EdepBuffer[lx1][SeedCell[lx1]+1]; ex1 += EdepBuffer[lx1][SeedCell[lx1]+1]*(SeedCell[lx1]+1); }

			KX = (ex1/n1 - ex/n)/(lx1-lx);
			cout << "lx=" << lx <<", lx1=" << lx1 << ", mx=" << SeedCell[lx] << " mx1=" << SeedCell[lx1] << ", KX=" << KX << endl;
		}
		*/
		// go upstream
		nm = mx;
		for (l=(lx&0xFE)+1; l>1; l-=4) {
			v = 0.;
//			if( nKX>0 ) mx = (l-lx)*KX+SeedCell[lx];
			for (m=mx-15; m<=mx+15; m++) {
				if ( m>=0 && m<=71 && EdepBuffer[l][m]>v ) {
					v = EdepBuffer[l][m];
					SeedCell[l] = m;
				}
			}
			if ( SeedCell[l]   > 0 ) mx = SeedCell[l];

			v = 0.;
			for (m=mx-4; m<=mx+4; m++) {
				if ( m>=0 && m<=71 && EdepBuffer[l-1][m]>v ) {
					v = EdepBuffer[l-1][m];
					SeedCell[l-1] = m;
				}
			}

			if ( SeedCell[l-1] > 0 ) mx = SeedCell[l-1];
		}

		// go downstream
		mx = nm;
		for (l=(lx&0xFE)+4; l<16; l+=4) {
			v = 0.;
			for (m=mx-15; m<=mx+15; m++) {
				if ( m>=0 && m<=71 && EdepBuffer[l][m]>v ) {
					v = EdepBuffer[l][m];
					SeedCell[l] = m;
				}
			}
			if ( SeedCell[l]   > 0 ) mx = SeedCell[l];

			v = 0.;
			for (m=mx-4; m<=mx+4; m++) {
				if ( m>=0 && m<=71 && EdepBuffer[l+1][m]>v ) {
					v = EdepBuffer[l+1][m];
					SeedCell[l+1] = m;
				}
			}

			if ( SeedCell[l+1] > 0 ) mx = SeedCell[l+1];
		}
		mx = nm;                                            // restore mx
	} else {
		dEX = 0.;
	}

	//
	// maximal amplitudes in Y-projection
	//
define_mY:
	ly = -1;
	my = -1;
	v  = EcalKX3DRecDB::Cutoff_MIP;
	for (l=0; l<18; l+=4) {
		SeedCell[l]   = -1;
		SeedCell[l+1] = -1;
		for (m=0; m<EcalKX3DRecDB::nCell; m++) {
			if ( EdepBuffer[l][m]>v ) {
				v = EdepBuffer[l][m];
				ly = l;
				my = m;
			}
			if ( EdepBuffer[l+1][m]>v ) {
				v = EdepBuffer[l+1][m];
				ly = l+1;
				my = m;
			}
		}
	}
//	cout << "v=" << v << ", my=" << my << ", ly=" << ly << endl;

	// consistency of the found Y maximum
	if ( ly==0 || ly==17) nm = 1;
	else                  nm = 0;
	nm = 0;
	if ( my >= 0 ) {
		val = 0.;
		for (l=0; l<18; l+=4) {
//			printf("LY %02d, MY %02d, E-1 %8.3f, E %8.3f, E+1 %8.3f\n", l, my, EdepBuffer[l][my-1], EdepBuffer[l][my], EdepBuffer[l][my+1]);
//			printf("LY %02d, MY %02d, E-1 %8.3f, E %8.3f, E+1 %8.3f\n", l+1, my, EdepBuffer[l+1][my-1], EdepBuffer[l+1][my], EdepBuffer[l+1][my+1]);
//			if ( l>(ly-5) && l<(ly+5) ) {
			if( ly/2 == l/2 ) nw = 1;
			else nw = 3;
			for (m=TMath::Max(0,my-nw); m<=TMath::Min(71,my+nw); m++) {
					if ( (l != ly) || (m != my) ) {
						if ( EdepBuffer[l][m]>0. ) {
							nm  += 1;
							val += EdepBuffer[l][m];
//							printf("LY %02d, MY %02d, E %8.3f\n", l, m, EdepBuffer[l][m]);
						}
					}
				}
//			}
//			if ( (l+1)>(ly-5) && (l+1)<(ly+5) ) {
			for (m=TMath::Max(0,my-nw); m<=TMath::Min(71,my+nw); m++) {
				if ( (((l+1) != ly) || (m != my)) && EdepBuffer[l+1][m]>0. ) {
					nm  += 1;
					val += EdepBuffer[l+1][m];
					//printf("LY %02d, MY %02d, E %8.3f\n", l+1, m, EdepBuffer[l][m]);
				}
			}
		}
//		}
//		printf("LY %02d, MY %02d, Nhits %02d, Emax %8.3f, Eout %8.3f\n", ly, my, nm, v, val);
		if ((nm < 4) || (v > 2*val) ) {
			EdepBuffer[ly][my] = EcalKX3DRecDB::Cutoff_MIP;
//			printf("-->ShowerClustering(): set L=%d,C=%d to MIP, nm=%d, v=%8.3f, val=%8.3f\n", ly, my, nm, v, val);
			goto define_mY;
		}
	}
	fEY = nm;

	// define Y footprint
	if ( my >= 0 ) {
		dEY = v;
		// define KY use the two layers in the same superlayer of seed
		v=0.;
		/*
		Int_t ly1 = ly%2==0?ly+1:ly-1;
		SeedCell[ly] = my;
		for (m=my-3; m<=my+3; m++) {
			if ( EdepBuffer[ly1][m]>v ) {
				v = EdepBuffer[ly1][m];
				SeedCell[ly1] = m;
			}
		}
		if( SeedCell[ly1] >= 0 ){ // define KX
			nKY ++;
			Double_t n=EdepBuffer[ly][SeedCell[ly]], ex= EdepBuffer[ly][SeedCell[ly]]*(SeedCell[ly]),
						n1=EdepBuffer[ly1][SeedCell[ly1]], ex1=EdepBuffer[ly1][SeedCell[ly1]]*SeedCell[ly1];
			if( SeedCell[ly] > 0  ){ n+=EdepBuffer[ly][SeedCell[ly]-1]; ex += EdepBuffer[ly][SeedCell[ly]-1]*(SeedCell[ly]-1); }
			if( SeedCell[ly] < 71 ){ n+=EdepBuffer[ly][SeedCell[ly]+1]; ex += EdepBuffer[ly][SeedCell[ly]+1]*(SeedCell[ly]+1); }
			if( SeedCell[ly1] > 0  ){ n1+=EdepBuffer[ly1][SeedCell[ly1]-1]; ex1 += EdepBuffer[ly1][SeedCell[ly1]-1]*(SeedCell[ly1]-1); }
			if( SeedCell[ly1] < 71 ){ n1+=EdepBuffer[ly1][SeedCell[ly1]+1]; ex1 += EdepBuffer[ly1][SeedCell[ly1]+1]*(SeedCell[ly1]+1); }

			KY = (ex1/n1 - ex/n)/(ly1-ly);
			cout << "ly=" << ly << ", ex=" << ex << ", n=" << n << ", ly1=" << ly1 << ", ex1=" << ex1 << ", n1=" << n1 << ", KY=" << KY << endl;
		}
		*/

		// go upstream
		nm = my;
		for (l=(ly&0xFE)+1; l>0; l-=4) {
			v = 0.;
//			if( nKY>0 ) my = (l-ly)*KY+SeedCell[ly];// need extrapolation in case of large angle
//			cout << l << ": " << SeedCell[ly] << ", " << my << endl;
			for (m=my-15; m<=my+15; m++) {
				if ( m>=0 && m<=71 &&EdepBuffer[l][m]>v ) {
					v = EdepBuffer[l][m];
					SeedCell[l] = m;
				}
			}
			if ( SeedCell[l]   > 0 ) my = SeedCell[l];
//			cout << "Layer=" << l << ", my=" << my << ", v= "<< v << endl;

			v = 0.;
			for (m=my-4; m<=my+4; m++) {
				if ( m>=0 && m<=71 &&EdepBuffer[l-1][m]>v ) {
					v = EdepBuffer[l-1][m];
					SeedCell[l-1] = m;
				}
			}

			if      ( SeedCell[l-1] > 0 ) my = SeedCell[l-1];
//			cout << "Layer=" << l-1 << ", my=" << my << ", v= "<< v << endl;
//			else if ( SeedCell[l]   > 0 ) my = SeedCell[l];
		}

		// go downstream
		my = nm;
		for (l=(ly&0xFE)+4; l<18; l+=4) {
			v = 0.;
//			if( nKY>0 ) my = (l-ly)*KY+SeedCell[ly];// need extrapolation in case of large angle
//			cout << l << ": " << SeedCell[ly] << ", " << my << endl;
			for (m=my-15; m<=my+15; m++) {
				if ( m>=0 && m<=71 &&EdepBuffer[l][m]>v ) {
					v = EdepBuffer[l][m];
					SeedCell[l] = m;
				}
			}
			if ( SeedCell[l]   > 0 ) my = SeedCell[l];

			v = 0.;
			for (m=my-4; m<=my+4; m++) {
				if ( m>=0 && m<=71 &&EdepBuffer[l+1][m]>v ) {
					v = EdepBuffer[l+1][m];
					SeedCell[l+1] = m;
				}
			}

			if( SeedCell[l+1] > 0 ) my = SeedCell[l+1];
		}
		my = nm;                                            // restore my
	} else {
		dEY = 0.;
	}

//	XSeed = mx; // to be used in DefineFootprint
//	YSeed = my;
}

void EcalKXFastRec::ShowerClustering(Int_t iter){
	memset(Cell_Min, 0, sizeof(Cell_Min));
	memset(Cell_Max, 0, sizeof(Cell_Max));
	EX = EY = 0.;
	for(Int_t l=0; l<EcalKX3DRecDB::kNL; l++){
		if(SeedCell[l]>=0){
			// search right
			if( SeedCell[l]==71 ) Cell_Max[l] = 71;
			else {
				for(Int_t m=SeedCell[l]+1; m<EcalKX3DRecDB::nCell; m++){
					if( EdepBuffer[l][m] <= 0 && EdepBuffer[l][m-1]<=0 ){
						Cell_Max[l] = m;
						break;
					}else if( m>=2 && EdepBuffer[l][m]>1.5*EdepBuffer[l][m-1] && EdepBuffer[l][m-1]>1.2*EdepBuffer[l][m-2] && EdepBuffer[l][m]>EcalKX3DRecDB::Cutoff_SEED && EdepBuffer[l][m-2]>0){
						//					cout << "m=" << m <<", "<< EdepBuffer[l][m] << ", m-1=" << EdepBuffer[l][m-1] << ", m-2=" << EdepBuffer[l][m-2] << endl;
						Cell_Max[l] = m-2;
						break;
					}else if( m==71 )
						Cell_Max[l] = 71;
				}
			}
			// search left
			if( SeedCell[l]==0 ) Cell_Min[l] = 0;
			else {
				for(Int_t m=SeedCell[l]-1; m>=0; m--){
					if( EdepBuffer[l][m] <= 0 && EdepBuffer[l][m+1]<=0 ){
						Cell_Min[l] = m+1;
						break;
					}else if( m<=69 && EdepBuffer[l][m]>1.5*EdepBuffer[l][m+1] && EdepBuffer[l][m+1]>1.2*EdepBuffer[l][m+2] && EdepBuffer[l][m]>EcalKX3DRecDB::Cutoff_SEED && EdepBuffer[l][m+2]>0){
						//					cout << "m=" << m <<", "<< EdepBuffer[l][m] << ", m+1=" << EdepBuffer[l][m+1] << ", m+2=" << EdepBuffer[l][m+2] << endl;
						Cell_Min[l] = m+2;
						break;
					}else if( m==0 )
						Cell_Min[l] = 0;
				}
			}
			for(Int_t m=Cell_Min[l]; m<=Cell_Max[l]; m++){
				if( EdepBuffer[l][m]>0 ){
					if( l%4<2 ) EY += EdepBuffer[l][m];
					else        EX += EdepBuffer[l][m];
//					cout << "l=" << l << ", m=" << m << ", e=" << EdepBuffer[l][m] << endl;
				}
			}
		}
		//cout << "iter=" << iter << ", N_Shwr=" << N_Shwr << ", l=" << l << ", Seed=" << SeedCell[l] << ", CMIN=" << Cell_Min[l] << ", CMAX=" << Cell_Max[l] << endl;
	}
}

void EcalKXFastRec::CalculateLayerCOG(Int_t iter){
	fX0 = fX1 = 0;
	fY0 = fY1 = 0;
	Int_t l, nm=0;
	memset( LayAmp, 0, sizeof(LayAmp) );
	memset( LayLRR, 0, sizeof(LayLRR) );
	memset( LayCrd, 0, sizeof(LayCrd) );
	if( iter>1 ){
		CellRatioParISS( ShwrE0[N_Shwr], fLrrPar0, fLrrPar1 );
	}
	double lrrpar;

//	cout << "------ iteration " << iter << " ----------" << endl;
	for (l=0; l<18; l++) {
		nm = SeedCell[l];
		if ( nm >= 0 ) {
			if( nm > 0 && nm<71 ){
				LayAmp[l] = EdepBuffer[l][nm-1] + EdepBuffer[l][nm] + EdepBuffer[l][nm+1];
				LayLRR[l] = EdepBuffer[l][nm+1]>0.010 && EdepBuffer[l][nm-1]>0.010 ? EdepBuffer[l][nm-1]/EdepBuffer[l][nm+1]:0; // >10MeV
				LayCrd[l] = EcalKX3DRecDB::Cell_Size*((nm-36.5)*EdepBuffer[l][nm-1] + (nm-35.5)*EdepBuffer[l][nm] + (nm-34.5)*EdepBuffer[l][nm+1])/LayAmp[l];
				if( iter>1 && LayLRR[l] > 0 ){
					lrrpar = fLrrPar0 + fLrrPar1*TMath::Max(3., l*sqrt(1+KX*KX+KY*KY));
					LayCrd[l] = (TMath::Log(LayLRR[l])/lrrpar + (nm-35.5) )*EcalKX3DRecDB::Cell_Size;
					//cout << "E0=" << ShwrE0[N_Shwr] << ", fLrrPar0=" << fLrrPar0 << ", fLrrPar1=" << fLrrPar1 << ", lrrpar=" << lrrpar << ", Eleft=" << EdepBuffer[l][nm-1] << ", Eright=" << EdepBuffer[l][nm+1] << ", LayCrd[l]=" << LayCrd[l] << endl;
				}
			}else if(nm==0){
				LayAmp[l] = EdepBuffer[l][nm] + EdepBuffer[l][nm+1];
				LayCrd[l] = ((nm-35.5)*EdepBuffer[l][nm] + (nm-34.5)*EdepBuffer[l][nm+1])/LayAmp[l];
			}else if(nm==71){
				LayAmp[l] = EdepBuffer[l][nm-1] + EdepBuffer[l][nm];
				LayCrd[l] = EcalKX3DRecDB::Cell_Size*((nm-36.5)*EdepBuffer[l][nm-1] + (nm-35.5)*EdepBuffer[l][nm])/LayAmp[l];
			}
//			cout << Form("lay %02d, seed %02d, Eleft %6.4f, Eseed %6.4f, Eright %6.4f, Amp %6.4f, Crd %5.2f, LRR %5.2f", l, nm, EdepBuffer[l][nm-1], EdepBuffer[l][nm], EdepBuffer[l][nm+1],  LayAmp[l], LayCrd[l], LayLRR[l]) << endl;
			// check the quality of the individual projections using the remaining energy depositions
	//		cout << nm << ", EdepBuffer[l][nm]=" << EdepBuffer[l][nm] << ", 0.8*EdepBuffer[l][nm+1]=" << 0.8*EdepBuffer[l][nm+1] << endl;
			if ( (nm>0 && nm<71 && EdepBuffer[l][nm]>0.8*EdepBuffer[l][nm-1] && EdepBuffer[l][nm]>0.8*EdepBuffer[l][nm+1] )
				  || (nm==0 && EdepBuffer[l][nm]>0.8*EdepBuffer[l][nm+1] )
				  ||  (nm==71 && EdepBuffer[l][nm]>0.8*EdepBuffer[l][nm-1] )
			  	  ) {
				if ( (l/2)%2 == 1 ) fX1 += 1;
				else                fY1 += 1;
			}
			if( iter==0 ){
				if ( (nm>0 && nm<71 && Edep[l][nm]>0.8*Edep[l][nm-1] && Edep[l][nm]>0.8*Edep[l][nm+1] )
				  || (nm==0 && Edep[l][nm]>0.8*Edep[l][nm+1] )
				  ||  (nm==71 && Edep[l][nm]>0.8*Edep[l][nm-1] )
			  	  ) { // original energy map
					if ( (l/2)%2 == 1 ) fX0 += 1;
					else                fY0 += 1;
				}
			}else{
				if ( (nm>0 && nm<71 && EdepAttC[l][nm]>0.8*EdepAttC[l][nm-1] && EdepAttC[l][nm]>0.8*EdepAttC[l][nm+1] )
						|| (nm==0 && EdepAttC[l][nm]>0.8*EdepAttC[l][nm+1] )
						||  (nm==71 && EdepAttC[l][nm]>0.8*EdepAttC[l][nm-1] )
					) { // original energy map
					if ( (l/2)%2 == 1 ) fX0 += 1;
					else                fY0 += 1;
				}
			}
		} else {
			LayAmp[l] = 0.;
			LayCrd[l] = 0.;
		}
	}
}

void EcalKXFastRec::ParabolaZ0Fit(){
	//
	// Define coordinate of the shower maximum
	//
//	fZ0 = 1;
//	fB0 = 1;
	Apex = 18;

	Int_t l, nm = 0;
	Double_t v  = 0.;
	for (l=0; l<18; l++) {
		if ( LayAmp[l] > v ) {
			v  = LayAmp[l];
			nm = l;
		}
	}
	for(l=0; l<18; l++) {
//		cout<<"l="<<l<<", amp=" << LayAmp[l] << endl;
		if( LayAmp[l] > EcalKX3DRecDB::Cutoff_MIP*3 ){ Apex = l; break; }
	}
	// calculate inverse matrix a_ij
	Double_t m11, m12, m22, m23, m33, b1, b2, b3, a11, a12, a13, a22, a23, det;
	Double_t val, arg;
	m11 = m12 = m22 = m23 = m33 = b1 = b2 = b3 = 0.;

	if ( nm < 3 ) {
		val = v;
		arg =  1.0 + EcalKX3DRecDB::EcalZ[0] - EcalKX3DRecDB::EcalZ[nm];
		//    printf("L -1, arg %8.3f, val %8.3f\n", arg, val);
		m11 += arg*arg*arg*arg;
		m12 += arg*arg*arg;
		m22 += arg*arg;
		m23 += arg;
		m33 += 1.;
		b1  += arg*arg*val;
		b2  += arg*val;
		b3  += val;
	}

	// fit Z0 using +-3 layers
	for(l=TMath::Max(0,nm-3); l<TMath::Min(18,nm+4); l++ ) {
		val = v - LayAmp[l];
		arg = EcalKX3DRecDB::EcalZ[l] - EcalKX3DRecDB::EcalZ[nm];
		//    printf("L %2d, arg %8.3f, val %8.3f\n", l, arg, val);
		m11 += arg*arg*arg*arg;
		m12 += arg*arg*arg;
		m22 += arg*arg;
		m23 += arg;
		m33 += 1.;
		b1  += arg*arg*val;
		b2  += arg*val;
		b3  += val;
	}

	if ( nm > 15 ) {
		val = v- LayAmp[2*nm-18];
		arg =  EcalKX3DRecDB::EcalZ[17] - 1.0 - EcalKX3DRecDB::EcalZ[nm];
		//    printf("L 18, arg %8.3f, val %8.3f\n", arg, val);
		m11 += arg*arg*arg*arg;
		m12 += arg*arg*arg;
		m22 += arg*arg;
		m23 += arg;
		m33 += 1.;
		b1  += arg*arg*val;
		b2  += arg*val;
		b3  += val;

//		fZ0 = 0;
	}

	// m22 == m13 is used explicitely
	det = m11*m22*m33 + 2.*m12*m22*m23 - m22*m22*m22 - m12*m12*m33 - m23*m23*m11;

	if ( det != 0. ) {
		a11 = m22*m33 - m23*m23;
		a12 = m22*m23 - m12*m33;
		a13 = m12*m23 - m22*m22;
		a22 = m11*m33 - m22*m22;
		a23 = m12*m22 - m11*m23;

		arg = b1*a11 + b2*a12 + b3*a13;
		if ( arg > 0. ) {
			val = -0.5*(b1*a12 + b2*a22 + b3*a23)/arg;
		} else {
			val = 0.;
		}
	} else {
		val = 0.;
	}

	if ( TMath::Abs(val)<2.5 ) {
		Z0 = val + EcalKX3DRecDB::EcalZ[nm];
		dZ0 = 1.2;
	} else {
		Z0 = EcalKX3DRecDB::EcalZ[nm];
		dZ0 = 2.5;
	}
}

void EcalKXFastRec::ShowerAxisFit(){
	vector<Double_t> vx, vz, vw;
	// fit XZ
	for(Int_t l=0; l<18; l++){
		if( l%4>=2 && SeedCell[l]>0 ){
			vx.push_back( LayCrd[l] );
			vz.push_back( EcalKX3DRecDB::EcalZ[l]-Z0 );
			vw.push_back( LayAmp[l] );
		}
	}
	if( vx.size() > 1 )
		LinearFit(vx.size(), vx, vz, vw, KX, X0);
	else
		KX = X0 = 0.;

	vx.clear();
	vz.clear();
	vw.clear();
	for(Int_t l=0; l<18; l++){
		if( l%4<2 && SeedCell[l]>0 ){
			vx.push_back( LayCrd[l] );
			vz.push_back( EcalKX3DRecDB::EcalZ[l]-Z0 );
			vw.push_back( LayAmp[l] );
			//cout << "l=" << l << ", LayCrd=" << LayCrd[l] << ", LayAmp=" << LayAmp[l] << endl;
		}
	}
	if( vx.size() > 1 )
		LinearFit(vx.size(), vx, vz, vw, KY, Y0);
	else
		KY = Y0 = 0.;

}

void EcalKXFastRec::LinearFit(Int_t n, vector<Double_t> vx, vector<Double_t> vy, vector<Double_t> vw, Double_t &kx, Double_t &x0){

	Double_t m11, m12, m22, b1, b2, det;
	m11 = m12 = m22 = b1 = b2 = 0.;

	for(Int_t i=0; i<n; i++){
		if ( vw[i] > 0 ) {
			m11 += vy[i]*vy[i]*vw[i];
			m12 += vy[i]*vw[i];
			m22 += vw[i];
			b1  += vx[i]*vy[i]*vw[i];
			b2  += vx[i]*vw[i];
		}
	}
	det = m11*m22 - m12*m12;
	if ( det != 0. ) {
		kx = (b1*m22 - b2*m12) / det;
		x0  = (b2*m11 - b1*m12) / det;
	} else {
		kx = -1.;
		x0 =  0.;
	}
	return;
}

void EcalKXFastRec::AttenuationCorrection(){
	Nhits=0;
	float posfiber;
	for(int l=0;l<EcalKX3DRecDB::nLayer;l++){
		for(int c=Cell_Min[l];c<=Cell_Max[l];c++){
			if(CheckCellStatus(l, c, EcalKX3DRecDB::CellUsed )){
//				cout << "l=" << l << ", c=" << c << " USED " << endl;
			  	continue;
			}
			if(Edep[l][c]<=0){
//				cout << "l=" << l << ", c=" << c << " ZERO" << endl;
			  	continue;
			}
			if(l%4<2) posfiber = X0 + KX*(EcalKX3DRecDB::EcalZ[l] - Z0); // Y Projection
			else      posfiber = Y0 + KY*(EcalKX3DRecDB::EcalZ[l] - Z0); // X Projection
			EdepAttC[l][c] = Edep[l][c]*GetAttCorFactor(l, c, posfiber);

			if( EdepAttC[l][c] > 0 ){
				Status[l][c] |= EcalKX3DRecDB::CellUsed;
				Nhits++;
			}
		}
	}
}

double EcalKXFastRec::GetAttCorFactor(int l, int c, float posfiber){

	if( IsSkipAttenuationCorrection ) return 1.;

	const static float hlfiber = 32.4;
	static bool isAttenuationParLoaded = 0;
	static float sAttPar[EcalKX3DRecDB::nLayer][EcalKX3DRecDB::nCell][3];
	static float sAtt0[EcalKX3DRecDB::nLayer][EcalKX3DRecDB::nCell];

	float dist,	atty;

	if( isAttenuationParLoaded==0 ){
		isAttenuationParLoaded = 1;
		memset( sAttPar, 0, sizeof( sAttPar ) );
		memset( sAtt0,   0, sizeof( sAtt0   ) );
		for(int _l=0; _l<EcalKX3DRecDB::nLayer; _l++){
			for(int _c=0; _c<EcalKX3DRecDB::nCell; _c++){
				for(int ipar=0; ipar<3; ipar++) sAttPar[_l][_c][ipar] = pEcalStaticCalibPar->attpar[_l][_c][ipar];
				sAtt0[_l][_c] = sAttPar[_l][_c][0]*exp(-hlfiber/sAttPar[_l][_c][1]) + (1-sAttPar[_l][_c][0])*exp(-hlfiber/sAttPar[_l][_c][2]);
			}
		}
	}

	if( (l%4<2 && c%4<2) || (l%4>=2 && c%4>=2) )
		dist = posfiber + hlfiber;
	else
		dist = hlfiber - posfiber;
	if( dist < 0         ) dist = 0;
	if( dist > hlfiber*2 ) dist = hlfiber*2;

	atty = sAttPar[l][c][0]*exp(-dist/sAttPar[l][c][1]) + (1-sAttPar[l][c][0])*exp(-dist/sAttPar[l][c][2]);

	if( atty > 0 )
		return sAtt0[l][c]/atty;
	else{
		cout << "--Error--BadAttenuationCorrection-- lay " << l << " cell " << c << ", sAtt0[l][c]=" << sAtt0[l][c] << ", sAttPar[l][c]=(" << sAttPar[l][c][0] << ", " << sAttPar[l][c][1] << ", " << sAttPar[l][c][2] << "), atty=" << atty << ", dist=" << dist << endl;
		return -1; // this hit will not be used
	}
}

void EcalKXFastRec::OrphanHitsAttCor(){
	float posfiber;
	Double_t dist, mindist=FLT_MAX;
	Double_t d1, d2;

	for(int l=0;l<18;l++){
		for(int c=0;c<=71;c++){
			if( Edep[l][c]<=0 || CheckCellStatus( l, c, EcalKX3DRecDB::CellUsed ) ) continue; // if isolated hit

			// find the nearest cluster
			Int_t I_shwr=0;
			mindist=FLT_MAX;
			for(int i_shwr=0; i_shwr<N_Shwr; i_shwr++){
				d1 = TMath::Abs( c - ShwrCellMin[i_shwr][l] );
				d2 = TMath::Abs( c - ShwrCellMax[i_shwr][l] );
				dist = TMath::Min( d1, d2 );
				if( dist*dist < mindist ){
					mindist = dist*dist;
					I_shwr = i_shwr;
				}
			}
			if(l/2%2==0) posfiber = ShwrX0[I_shwr] + ShwrKX[I_shwr]*(EcalKX3DRecDB::EcalZ[l] - ShwrZ0[I_shwr]);
			else         posfiber = ShwrY0[I_shwr] + ShwrKY[I_shwr]*(EcalKX3DRecDB::EcalZ[l] - ShwrZ0[I_shwr]);

			EdepAttC[l][c] = Edep[l][c]*GetAttCorFactor(l, c, posfiber);

			if( EdepAttC[l][c] > 0 ){
				Status[l][c] |= EcalKX3DRecDB::CellUsed;
				ShwrNhits[I_shwr]++;
			}
		}
	}
}

void EcalKXFastRec::DoCalibration(){
	if(!IsSkipCalibration) LoadCalibrationDataBase();
	DefineCellStatus(); // mainly for saturation
	if(!IsSkipCalibration) GainCalibration();
	CellEnergyMeV2GeV();
}

void EcalKXFastRec::DoFastReconstruction(){

	int iter=0;
  	N_Shwr=0;
	Int_t flag_x, flag_y;

	DoCalibration();

	Int_t MaxIteration=1;
	if( IsSkipAttenuationCorrection ) MaxIteration = 1;
	else                              MaxIteration = 2;

	while( iter <= MaxIteration ){
		N_Shwr = 0;
		memset(ShwrNhits, 0, sizeof(ShwrNhits));
		memset(ShwrCellMin, 0, sizeof(ShwrCellMin));
		memset(ShwrCellMax, 0, sizeof(ShwrCellMax));
		memset(ShwrX0, 0, sizeof(ShwrX0));
		memset(ShwrY0, 0, sizeof(ShwrY0));
		memset(ShwrKX, 0, sizeof(ShwrKX));
		memset(ShwrKY, 0, sizeof(ShwrKY));
		memset(ShwrZ0, 0, sizeof(ShwrZ0));
		while( N_Shwr < EcalKX3DRecDB::kNSFast ){
			// Only search the most energetic shower in current available(NOT yet used) hits
			if( iter==0 ){
//				cout << "-----> iter==0"<<endl;
				memcpy( EdepBuffer, Edep, sizeof(EdepBuffer) );
			}
			else{
//				cout << "-----> iter==1"<<endl;
				memcpy( EdepBuffer, EdepAttC, sizeof(EdepBuffer) );
			}
			if( N_Shwr == 0 ){ // 1st shower
//				cout << "---> N_Shwr==0"<<endl;
				ClearAllCellStatus( EcalKX3DRecDB::CellUsed );
			}else{
//				cout << "---> N_Shwr==" << N_Shwr <<endl;
				for(int l=0; l<EcalKX3DRecDB::kNL; l++) for(int c=0; c<EcalKX3DRecDB::nCell; c++)
					if( CheckCellStatus( l, c, EcalKX3DRecDB::CellUsed ) ) EdepBuffer[l][c] = 0;
			}

			ShowerSeedFinding(iter);
			ShowerClustering(iter);
			CalculateLayerCOG(iter);
			ParabolaZ0Fit();
			ShowerAxisFit();

			// quality of the new shower
			if      ( (dEX>EcalKX3DRecDB::Cutoff_SEED) && (fX1>=EcalKX3DRecDB::Cutoff_NPL)                      ) flag_x = 2;
			else if ( (dEX>EcalKX3DRecDB::Cutoff_SEED) && (fEX>=EcalKX3DRecDB::Cutoff_NNB) && (fX0>=EcalKX3DRecDB::Cutoff_NPL) ) flag_x = 2;
			else if ( (dEX>EcalKX3DRecDB::Cutoff_SEED) && (fEX>=EcalKX3DRecDB::Cutoff_NNB  ||  fX0>=EcalKX3DRecDB::Cutoff_NPL) ) flag_x = 1;
			else if ( (dEX>EcalKX3DRecDB::Cutoff_MIP)  && (EX>2.*dEX)                            ) flag_x = 1;
			else                                                                    flag_x = 0;

			if      ( (dEY>EcalKX3DRecDB::Cutoff_SEED) && (fY1>=EcalKX3DRecDB::Cutoff_NPL)                      ) flag_y = 2;
			else if ( (dEY>EcalKX3DRecDB::Cutoff_SEED) && (fEY>=EcalKX3DRecDB::Cutoff_NNB) && (fY0>=EcalKX3DRecDB::Cutoff_NPL) ) flag_y = 2;
			else if ( (dEY>EcalKX3DRecDB::Cutoff_SEED) && (fEY>=EcalKX3DRecDB::Cutoff_NNB  ||  fY0>=EcalKX3DRecDB::Cutoff_NPL) ) flag_y = 1;
			else if ( (dEY>EcalKX3DRecDB::Cutoff_MIP) && (EY>2.*dEY)                             ) flag_y = 1;
			else                                                                    flag_y = 0;
			// is there a compelling shower?
	//		cout << "KX=" << KX << ", X0=" << X0 << ", KY=" << KY << ", Y0=" << Y0 << ", Z0=" << Z0 << endl;
	//		cout << "EX=" << EX << ", dEX=" << dEX << ", fX1=" << fX1 << ", fEX=" << fEX << ", fX0=" << fX0 << endl;
	//		cout << "EY=" << EY << ", dEY=" << dEY << ", fY1=" << fY1 << ", fEY=" << fEY << ", fY0=" << fY0 << endl;
	//		cout << "flag_x=" << flag_x << ", flag_y=" << flag_y << endl;

//			if ( N_Shwr==0 && ((fEX<EcalKX3DRecDB::Cutoff_NNB && fX0<EcalKX3DRecDB::Cutoff_NPL) || (fEY<EcalKX3DRecDB::Cutoff_NNB && fY0<EcalKX3DRecDB::Cutoff_NPL)) ) break;
//			if ( N_Shwr>0  && (flag_x<1 || flag_y<1)                                                     ) break;
			if ( flag_x<1 || flag_y<1 ) break;

			AttenuationCorrection();
			EstimateEnergy();
			if( iter==MaxIteration && N_Shwr==0 ){ // the primary shower, last iteration
				memcpy( SeedCell0, SeedCell, sizeof(SeedCell) );
				memcpy( LayCrd0, LayCrd, sizeof(LayCrd) );
				memcpy( LayLRR0, LayLRR, sizeof(LayLRR) );
				memcpy( LayEdep0, LayEdep, sizeof(LayEdep) );
				memcpy( LayPos0, LayPos, sizeof(LayPos) );
			}

			ShwrE0[N_Shwr] = E0;
			ShwrEd0[N_Shwr] = Ed0;
			ShwrEd1[N_Shwr] = Ed1;
			ShwrX0[N_Shwr] = X0;
			ShwrY0[N_Shwr] = Y0;
			ShwrZ0[N_Shwr] = Z0;
			ShwrKX[N_Shwr] = KX;
			ShwrKY[N_Shwr] = KY;
			for(Int_t l=0; l<18; l++){
				ShwrCellMin[N_Shwr][l] = Cell_Min[l];
				ShwrCellMax[N_Shwr][l] = Cell_Max[l];
			}
			ShwrNhits[N_Shwr] = Nhits;
//			cout << "iter=" << iter <<", N_Shwr=" << N_Shwr << ", Z0=" << Z0 << ", X0=" << X0 << ", Y0=" << Y0 << ", KX=" << KX << ", KY=" << KY <<", Nhits=" << Nhits << endl;
			N_Shwr++;
		}
		if( N_Shwr==0 ) break;
		if( N_Shwr>0 ){
			OrphanHitsAttCor();
		}
		iter++;
	}
}


void EcalKXFastRec::CellRatioParMC(double ene, double &par0, double &par1){
	double p_0[3] = {-6.68, 360, 0.124};
	double p_1[3] = {0.145, 0.378, 1.71};
	double log10ene = log10(ene);
	if( log10ene < 1.2 ) log10ene = 1.2;
	par0 = p_0[0]+p_0[1]*exp(-log10ene/p_0[2]);
	par1 = p_1[0]+p_1[1]*exp(-log10ene/p_1[2]);
}

void EcalKXFastRec::CellRatioParToyMC(double ene, double &par0, double &par1){
	double p_0[3] = {-6.54, 1.75, 1.16};
	double p_1[3] = {0.164, 0.328, 0.714};
	double log10ene = log10(ene);
	if( log10ene < 1.2 ) log10ene = 1.2;
	par0 = p_0[0]+p_0[1]*exp(-log10ene/p_0[2]);
	par1 = p_1[0]+p_1[1]*exp(-log10ene/p_1[2]);
}

void EcalKXFastRec::CellRatioParISS(double ene, double &par0, double &par1){
	double p_0[3] = {-7.669,  7.828, 0.132  };
	double p_1[3] = { 0.539, -0.480, -0.0551};
	if( ene < 2 ) ene = 2.;
	par0 = p_0[0] + p_0[1]/(ene+0.1) + p_0[2]*log(ene+1);
	par1 = p_1[0] + p_1[1]/(ene+0.1) + p_1[2]*log(ene+1);
}

void EcalKXFastRec::EstimateEnergy(){
	memset( LayEdep, 0, sizeof(LayEdep) );
	Ed0=0;
	for(int l=0; l<EcalKX3DRecDB::nLayer; l++){
		for(int c=Cell_Min[l]; c<=Cell_Max[l]; c++){
			LayEdep[l] += EdepAttC[l][c];
		}
		Ed0 += LayEdep[l];
	}
	// PMT efficiency correction
	for(int l=0; l<EcalKX3DRecDB::nLayer; l++){
		if( l%4<2 ){ // Y coord
			LayPos[l] = Y0 + KY*(EcalKX3DRecDB::EcalZ[l]-Z0);
		}else{
			LayPos[l] = X0 + KX*(EcalKX3DRecDB::EcalZ[l]-Z0);
		}
	}
	E0 = Ed0;
	for(int iter=0; iter<2; iter++){
		EnergyCorrection(); // calculate Ed1 and E0
	}
}

void EcalKXFastRec::EnergyCorrection(){
	double logE0 = TMath::Log10(E0);
	double A = 0.0295431 - 0.527707*logE0;
	double B = -0.108753 + 0.0173606*logE0;
	Ed1 = 0.;
	double l2r = 0.;
	double cor;
	for(int l=0; l<EcalKX3DRecDB::nLayer; l++){
		double dpmt = (LayPos[l]+32.4)/0.9 - int((LayPos[l]+32.4)/0.9);
		double par = A*(1 + B*l);
		cor = 1 + par*(dpmt-0.5)*(dpmt-0.5);
		if( cor<=0 ){
//			cout << "--EcalKXFastRec::EnergyCorrection-PMTEfficiency-Error--cor=" << cor << ", par=" << par << ", l=" << l << ", E0=" << E0 << endl;
			cor = 1.;
		}
		Ed1 += LayEdep[l]/cor;

		if( l>=16 ){
			l2r += LayEdep[l]/cor;
		}
	}
	if( Ed1>0 ) l2r /= Ed1;
	const static double C = -1.65;
//	static int nwarning=0;
	cor = 1 + C*l2r;
	if( cor<= 0.3 ){
//		if(nwarning++<10){
//			cout << "--EcalKXFastRec::EnergyCorrection-RearLeakage-Error--cor=" << cor << ", l2r=" << l2r << endl;
//		}
		cor=0.3;
	}
	E0 = Ed1/cor;
}
