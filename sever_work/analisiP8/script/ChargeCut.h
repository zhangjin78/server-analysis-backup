//------Selections for MC silicon, version: amsd47n_TMTFNTotORDHB_Si28MCB1116, fit from Full Span
const double MCSiSel[5][2] = {{13.5557, 0.715542}, 	//InnerQ
                              {14.8228, 0.32208}, 	//UTofQ
                              {14.6433, 0.323948}, 	//LTofQ
                              {14.3091, 0.58146}, 	//L1Q
                              {0., 0.}};			//L9Q

//------Selections for ISS Silicon(14), version: amsd47n -- correct TofQ
const double ISSSiSel[5][2] = {	{14.1185, 0.3159}, 	//InnerQ	//use the non-charge cut fitting result, see UNIGE meeting, 17/10/09
						{14.0442, 0.2589}, 	//UTofQ	//fitting range: 13.7-14.3
						{13.9726, 0.2577}, 	//LTofQ 	//fitting range: 13.7-14.3
						{14.0305, 0.6951}, 	//L1Q		//fitting range: 12.9-14.6
						{14.0579, 0.7429}};	//L9Q		//fitting range: 12.9-14.6(same as L1)

//------Selections for ISS Iron(26), version: amsd47n -- correct TofQ
const double ISSFeSel[5][2] = {	{26.2610, 0.8216}, 	//InnerQ	//fitting range: 25.6 - 27.8
						{25.9388, 0.4027}, 	//UTofQ	//fitting range: 25.5 - 26.8
						{26.0409, 0.3628}, 	//LTofQ	//fitting range: 25.5 - 26.8
						{25.9693, 1.5386}, 	//L1Q		//fitting range: 23.6 - 27.6
						{26.1385, 1.8265}};	//L9Q		//fitting range: 23.6 - 27.6 (same as L1Q)
					 
//------Selections for ISS Neon(10), version: amsd47n
const double ISSNeSel[5][2] = {{10.0351, 0.1890}, 	//InnerQ	//fitting range: 
					 { 9.9960, 0.2273}, 	//UTofQ	//fitting range: 
					 {0., 0.}, 			//LTofQ
					 {10.0375, 0.3835}, 	//L1Q		//fitting range: 
					 {0., 0.}};			//L9Q

//------Selections for ISS Magnesium(12), version: amsd47n
const double ISSMgSel[5][2] = {{12.0815, 0.2476}, 	//InnerQ	//fitting range: 
					 {12.0250, 0.2475}, 	//UTofQ	//fitting range: 
					 {0., 0.}, 			//LTofQ
					 {12.0932, 0.4940}, 	//L1Q		//L1Q		//fitting range: 
					 {0., 0.}};			//L9Q


//------Selections for ISS Sulfur(16), version: amsd47n
const double ISSSuSel[5][2] = {{16.1818, 0.3885}, 	//InnerQ	//fitting range: 
					 {15.9109, 0.3252}, 	//UTofQ	//fitting range: 
					 {0., 0.}, 			//LTofQ
					 {15.7633, 0.6608}, 	//L1Q		//fitting range: 
					 {0., 0.}};			//L9Q

//------Selections for ISS Cathium(20), version: amsd47n
const double ISSCaSel[5][2] = {{20.1512, 0.7267}, 	//InnerQ	//fitting range: 
					 {19.8800, 0.4251}, 	//UTofQ	//fitting range: 
					 {0., 0.}, 			//LTofQ
					 {0., 0.}, 	//L1Q		//fitting range: 
					 {0., 0.}};			//L9Q
//------------------------------------------------------------

//------version: amsd49n, norecon2P7
//----ISS
//--Silicon(14)
const double ISSSiSel2[5][2] = {	{14.2342, 0.3199}, 	//InnerQ	
						{14.0419, 0.2766}, 	//UTofQ	//fitting range: 13.7-14.3
						{14.0242, 0.2569}, 	//LTofQ 	//fitting range: 13.7-14.3
						{14.0153, 0.7043}, 	//L1Q		//fitting range: 12.9-14.6 (with bad ladder)
						{14.0174, 0.7263}};	//L9Q		//fitting range: 12.9-14.6(same as L1)

//--Iron(26)
const double ISSFeSel2[5][2] = {	{26.6457, 0.7629}, 	//InnerQ	//fitting range: 25.6 - 27.8
						{26.0123, 0.4097}, 	//UTofQ	//fitting range: 25.5 - 26.8
						{26.0611, 0.3702}, 	//LTofQ	//fitting range: 25.5 - 26.8
						{25.9532, 1.5380}, 	//L1Q		//fitting range: 23.6 - 27.6 (with bad ladder)
						{26.1654, 1.9346}};	//L9Q		//fitting range: 23.6 - 27.6 (same as L1Q)
//------------------------------------------------------------

//------version: amsd54n, B1130P7, QY dst
//------trigger period 1
//----ISS
//--Silicon(14)
/*const double ISSSiSel_B1130P7t1[5][2] = {	{14.2100, 0.2776}, 	//InnerQ	
							{14.0023, 0.2694}, 	//UTofQ	//fitting range: 13.7-14.3
							{14.0098, 0.2587}, 	//LTofQ 	//fitting range: 13.7-14.3
							{14.0371, 0.6883}, 	//L1Q		//fitting range: 12.9-14.6 (with bad ladder)
							{14.0544, 0.7383}};	//L9Q		//fitting range: 12.9-14.6(same as L1)

//--Iron(26)
const double ISSFeSel_B1130P7t1[5][2] = {	{26.4322, 0.6916}, 	//InnerQ	//fitting range: 25.6 - 27.8
							{26.0024, 0.3877}, 	//UTofQ	//fitting range: 25.5 - 26.8
							{26.0549, 0.3490}, 	//LTofQ	//fitting range: 25.5 - 26.8
							{25.9808, 1.5628}, 	//L1Q		//fitting range: 23.6 - 27.6 (with bad ladder)
							{26.1389, 1.7438}};	//L9Q		//fitting range: 23.6 - 27.6 (same as L1Q)
//------trigger period 2
//----ISS
//--Silicon(14)
const double ISSSiSel_B1130P7t2[5][2] = {	{14.2413, 0.3193}, 	//InnerQ	
							{14.0415, 0.2766}, 	//UTofQ	//fitting range: 13.7-14.3
							{14.0259, 0.2620}, 	//LTofQ 	//fitting range: 13.7-14.3
							{14.0109, 0.6996}, 	//L1Q		//fitting range: 12.9-14.6 (with bad ladder)
							{14.0274, 0.7439}};	//L9Q		//fitting range: 12.9-14.6(same as L1)

//--Iron(26)
const double ISSFeSel_B1130P7t2[5][2] = {	{26.6367, 0.7752}, 	//InnerQ	//fitting range: 25.6 - 27.8
							{26.0152, 0.4065}, 	//UTofQ	//fitting range: 25.5 - 26.8
							{26.0653, 0.3686}, 	//LTofQ	//fitting range: 25.5 - 26.8
							{25.9383, 1.5772}, 	//L1Q		//fitting range: 23.6 - 27.6 (with bad ladder)
							{26.1150, 1.8493}};	//L9Q		//fitting range: 23.6 - 27.6 (same as L1Q)*/ //now using new tracker charge result
//------------------------------------------------------------

//------version: amsd54n, B1130P7, Zhen's dst with new tracker charge calibration
//------trigger period 1
//----ISS
//--Sodium(11)
const double ISSNaSel_B1130P7t1[5][2] = {	{10.9838, 0.1482}, 	//InnerQ	
							{10.9900, 0.3059}, 	//UTofQ	//fitting range: 13.7-14.3
							{11.0025, 0.2865}, 	//LTofQ 	//fitting range: 13.7-14.3
							{11., 0.}, 	//L1Q		//fitting range: 12.9-14.6 (with bad ladder)
							{11., 0.}};	//L9Q		//fitting range: 12.9-14.6(same as L1)
							
//--Magnesium(12)
const double ISSMgSel_B1130P7t1[5][2] = {	{12.0172, 0.1525}, 	//InnerQ	
							{11.9952, 0.2523}, 	//UTofQ	//fitting range: 13.7-14.3
							{12.0063, 0.2393}, 	//LTofQ 	//fitting range: 13.7-14.3
							{12., 0.}, 	//L1Q		//fitting range: 12.9-14.6 (with bad ladder)
							{12., 0.}};	//L9Q		//fitting range: 12.9-14.6(same as L1)
							
//--Aluminium(13)
const double ISSAlSel_B1130P7t1[5][2] = {	{13.0268, 0.1673}, 	//InnerQ	
							{12.9759, 0.3501}, 	//UTofQ	//fitting range: 13.7-14.3
							{13.0020, 0.3090}, 	//LTofQ 	//fitting range: 13.7-14.3
							{13., 0.}, 	//L1Q		//fitting range: 12.9-14.6 (with bad ladder)
							{13., 0.}};	//L9Q		//fitting range: 12.9-14.6(same as L1)
							

//--Silicon(14)
const double ISSSiSel_B1130P7t1[5][2] = {	{14.0151, 0.1745}, 	//InnerQ	
							{14.0023, 0.2694}, 	//UTofQ	//fitting range: 13.7-14.3
							{14.0098, 0.2587}, 	//LTofQ 	//fitting range: 13.7-14.3
							{14., 0.}, 	//L1Q		//fitting range: 12.9-14.6 (with bad ladder)
							{14., 0.}};	//L9Q		//fitting range: 12.9-14.6(same as L1)
							
//--Phosphorus(15)
const double ISSPhSel_B1130P7t1[5][2] = {	{15., 0.}, 	//InnerQ	
							{15., 0.}, 	//UTofQ	//fitting range: 13.7-14.3
							{15., 0.}, 	//LTofQ 	//fitting range: 13.7-14.3
							{15., 0.}, 	//L1Q		//fitting range: 12.9-14.6 (with bad ladder)
							{15., 0.}};	//L9Q		//fitting range: 12.9-14.6(same as L1)
							
//--Sulfur(16)
const double ISSSuSel_B1130P7t1[5][2] = {	{16.0200, 0.2176}, 	//InnerQ	
							{15.9724, 0.3237}, 	//UTofQ	//fitting range: 13.7-14.3
							{16.0116, 0.3367}, 	//LTofQ 	//fitting range: 13.7-14.3
							{16., 0.}, 	//L1Q		//fitting range: 12.9-14.6 (with bad ladder)
							{16., 0.}};	//L9Q		//fitting range: 12.9-14.6(same as L1)
							
//--Chlorine(17)
const double ISSClSel_B1130P7t1[5][2] = {	{17., 0.}, 	//InnerQ	
							{16.8905, 0.5377}, 	//UTofQ	//fitting range: 13.7-14.3, L1Inner
							{17.0034, 0.4920}, 	//LTofQ 	//fitting range: 13.7-14.3
							{17., 0.}, 	//L1Q		//fitting range: 12.9-14.6 (with bad ladder)
							{17., 0.}};	//L9Q		//fitting range: 12.9-14.6(same as L1)
							
//--Iron(26)
const double ISSFeSel_B1130P7t1[5][2] = {	{26., 0.}, 	//InnerQ	//fitting range: 25.6 - 27.8
							{26.0024, 0.3877}, 	//UTofQ	//fitting range: 25.5 - 26.8
							{26.0549, 0.3490}, 	//LTofQ	//fitting range: 25.5 - 26.8
							{26., 1.}, 	//L1Q		//fitting range: 23.6 - 27.6 (with bad ladder)
							{26., 1.}};	//L9Q		//fitting range: 23.6 - 27.6 (same as L1Q)
//------trigger period 2
//----ISS
//--Sodium(11)
const double ISSNaSel_B1130P7t2[5][2] = {	{11., 0.}, 	//InnerQ	
							{11.0044, 0.3246}, 	//UTofQ	//fitting range: 13.7-14.3
							{11.0105, 0.2891}, 	//LTofQ 	//fitting range: 13.7-14.3
							{11., 0.}, 	//L1Q		//fitting range: 12.9-14.6 (with bad ladder)
							{11., 0.}};	//L9Q		//fitting range: 12.9-14.6(same as L1)
							
//--Magnesium(12)
const double ISSMgSel_B1130P7t2[5][2] = {	{12., 0.}, 	//InnerQ	
							{12.0227, 0.2566}, 	//UTofQ	//fitting range: 13.7-14.3
							{12.0190, 0.2435}, 	//LTofQ 	//fitting range: 13.7-14.3
							{12., 0.}, 	//L1Q		//fitting range: 12.9-14.6 (with bad ladder)
							{12., 0.}};	//L9Q		//fitting range: 12.9-14.6(same as L1)
							
//--Aluminium(13)
const double ISSAlSel_B1130P7t2[5][2] = {	{13., 0.}, 	//InnerQ	
							{13.0101, 0.3773}, 	//UTofQ	//fitting range: 13.7-14.3
							{13.0168, 0.3125}, 	//LTofQ 	//fitting range: 13.7-14.3
							{13., 0.}, 	//L1Q		//fitting range: 12.9-14.6 (with bad ladder)
							{13., 0.}};	//L9Q		//fitting range: 12.9-14.6(same as L1)
							

//--Silicon(14)
const double ISSSiSel_B1130P7t2[5][2] = {	{14., 0.}, 	//InnerQ	
							{14.0415, 0.2766}, 	//UTofQ	//fitting range: 13.7-14.3
							{14.0259, 0.2620}, 	//LTofQ 	//fitting range: 13.7-14.3
							{14., 0.}, 	//L1Q		//fitting range: 12.9-14.6 (with bad ladder)
							{14., 0.}};	//L9Q		//fitting range: 12.9-14.6(same as L1)
							
//--Phosphorus(15)
const double ISSPhSel_B1130P7t2[5][2] = {	{15., 0.}, 	//InnerQ	
							{15., 0.}, 	//UTofQ	//fitting range: 13.7-14.3
							{15., 0.}, 	//LTofQ 	//fitting range: 13.7-14.3
							{15., 0.}, 	//L1Q		//fitting range: 12.9-14.6 (with bad ladder)
							{15., 0.}};	//L9Q		//fitting range: 12.9-14.6(same as L1)
							
//--Sulfur(16)
const double ISSSuSel_B1130P7t2[5][2] = {	{16., 0.}, 	//InnerQ	
							{16.0173, 0.3502}, 	//UTofQ	//fitting range: 13.7-14.3
							{16.0120, 0.3436}, 	//LTofQ 	//fitting range: 13.7-14.3
							{16., 0.}, 	//L1Q		//fitting range: 12.9-14.6 (with bad ladder)
							{16., 0.}};	//L9Q		//fitting range: 12.9-14.6(same as L1)
							
//--Chlorine(17)
const double ISSClSel_B1130P7t2[5][2] = {	{17., 0.}, 	//InnerQ	
							{16.9431, 0.6497}, 	//UTofQ	//fitting range: 13.7-14.3
							{17.0257, 0.5445}, 	//LTofQ 	//fitting range: 13.7-14.3
							{17., 0.}, 	//L1Q		//fitting range: 12.9-14.6 (with bad ladder)
							{17., 0.}};	//L9Q		//fitting range: 12.9-14.6(same as L1)

//--Iron(26)
const double ISSFeSel_B1130P7t2[5][2] = {	{26., 0.}, 	//InnerQ	//fitting range: 25.6 - 27.8
							{26.0152, 0.4065}, 	//UTofQ	//fitting range: 25.5 - 26.8
							{26.0653, 0.3686}, 	//LTofQ	//fitting range: 25.5 - 26.8
							{25., 1.}, 	//L1Q		//fitting range: 23.6 - 27.6 (with bad ladder)
							{26., 1.}};	//L9Q		//fitting range: 23.6 - 27.6 (same as L1Q)
//------------------------------------------------------------
