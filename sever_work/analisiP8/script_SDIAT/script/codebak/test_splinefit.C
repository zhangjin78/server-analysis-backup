#include "splineFit3.h"
#include <iostream>

using namespace std;
SplineFit *spFit1=NULL;

void test_splinefit() {
    cout << "Simple constructor test..." << endl;
    
    try {
        // 测试最简单的构造函数
        string fluxmodle_dir = "/home/ams/jzhang/AMS/Flux/FluxFit/SplineFit";
        string histn = Form("%s/fluxfit_amsd68n_q%d_20230304", fluxmodle_dir.c_str(), 18);
        cout << "   Data file path: " << histn << endl;
        spFit1 = new SplineFit(histn.c_str());
        // TF1* tf = spFit1->f1SplineFit;
        // if (tf) {
        //     cout << "   f1SplineFit address: " << tf << endl;
        //     cout << "   f1SplineFit name: " << tf->GetName() << endl;
        // } else {
        //     cout << "   ⚠ f1SplineFit is nullptr" << endl;
        // }
        
    } catch (...) {
        cout << "Constructor failed" << endl;
    }
}