//  $Id: linkdef6.h,v 1.1 2016/06/01 12:44:20 choutko Exp $
#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#ifdef __AMSVMC__
#pragma link C++ class amsvmc_MCApplication+;
#pragma link C++ class amsvmc_DetectorConstruction+;
#pragma link C++ class amsvmc_PrimaryGenerator+;
#pragma link C++ class amsvmc_MCStack+;
#pragma link C++ class amsvmc_RootManager+;
#pragma link C++ class amsvmc_MagField+;
#pragma link C++ class amsvmc_RunConfiguration+;
#endif

#pragma link C++ class AMSSetupR::ISSAtt+;
#pragma link C++ class AMSSetupR::AMSSTK+;
#pragma link C++ class AMSSetupR::ISSSA+;
#pragma link C++ class AMSSetupR::ISSINTL+;
#pragma link C++ class AMSSetupR::ISSCTRS+;
#pragma link C++ class AMSSetupR::ISSCTRSR+;
#pragma link C++ class AMSSetupR::GPSWGS84+;
#pragma link C++ class AMSSetupR::GPSWGS84R+;
#pragma link C++ class AMSSetupR::ISSGTOD+;
#pragma link C++ class AMSSetupR::BadRun+;
#pragma link C++ class AMSSetupR::DSPError+;
#pragma link C++ class SlowControlDB+;
#pragma link C++ class std::pair<unsigned int,std::vector<float> >+;
#pragma link C++ class std::map<unsigned int,std::vector<float> >+;

#pragma link C++ class SubType+;
#pragma link C++ class std::pair<int,SubType>+;
#pragma link C++ class std::map<int,SubType>+;
#pragma link C++ class DataType+;
#pragma link C++ class std::pair<int,DataType>+;
#pragma link C++ class std::map<int,DataType>+;
#pragma link C++ class Node+;
#pragma link C++ class std::map<std::string,Node>+;
#pragma link C++ class std::pair<std::string,Node>+;
#pragma link C++ class std::map<std::string,unsigned int>+;
#pragma link C++ class std::pair<std::string,unsigned int>+;
#pragma link C++ class std::map<unsigned short,std::vector<RichRingBSegmentR> >+;

#endif
