#ifndef RICHVAR_H
#define RICHVAR_H

#include <string>

const int nRichVar=9;
const string sVar[9] = {"NoPMTs", "Kolmogorov", "NpeRingToNpeTotal", "HitPMTQCons", "TofRichBetaDiff", "RichQ", "BetaDiffRec", "ExpectedNpe", "EmissionPos"};
const string sVarFull[9] = {"No. PMTs", "Kolmogorov Probability", "N_{pe}(Ring)/N_{pe}(Total)", "Hit/PMT charge consistency", "|#beta_{tof}-#beta_{rich}|/#beta_{rich}", "Rich charge", "|#beta_{LIP}-#beta_{CIEMAT}|", "Expected N_{pe}", "Emission point position"};
#endif //RICHVAR_H
