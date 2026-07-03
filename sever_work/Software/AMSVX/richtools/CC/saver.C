#include "TNamed.h"
#include <vector>

using namespace std;

class GeomHashBridge: public TNamed{
public:
  int dimension;
  int numNodes;
  int minSize;              // min size allowed to bin
  vector<unsigned char> points;       // Chosen direction
  vector<float> limit;     // The limit to separate among them
  vector<int>   nodes[2];   // If the node j is terminal, node[0][j]=-1, node[1][j]=entries in bin, point[0][j*dimension]=mean value, point[1][j*dimension]=rms                                                   
  vector<float> Means;
  vector<float> Rms;
  vector<int>   Entries;
  vector<float> Template;
  vector<float> TemplateRms;
  vector<float> Peak;
  vector<float> PeakWidth;

  GeomHashBridge(){}


  ClassDef(GeomHashBridge,1);
};

class GeomHashEnsembleBridge: public TNamed{
public:
  vector<GeomHashBridge> hashes;


  GeomHashEnsembleBridge(){}
  
  ClassDef(GeomHashEnsembleBridge,1);
};

ClassImp(GeomHashBridge);


#define _BRIDGE_ 
#ifdef _BRIDGE_
#include "RichTools.h"

void to_GeomHashBridge(GeomHashBridge &c,GeomHash &h){
#define C(_x) c._x=h._x;
    C(dimension);
    C(numNodes);
    C(minSize);
    C(points);
    C(limit);
    C(nodes[0]);
    C(nodes[1]);
    C(Means);
    C(Rms);
    C(Entries);
    C(Template);
    C(TemplateRms);
    C(Peak);
    C(PeakWidth);
#undef C
  }

void to_GeomHash(GeomHashBridge &c,GeomHash &h){
#define C(_x) h._x=c._x;
    C(dimension);
    C(numNodes);
    C(minSize);
    C(points);
    C(limit);
    C(nodes[0]);
    C(nodes[1]);
    C(Means);
    C(Rms);
    C(Entries);
    C(Template);
    C(TemplateRms);
    C(Peak);
    C(PeakWidth);
#undef C 
  }



void to_GeomHashEnsembleBridge(GeomHashEnsembleBridge &c,GeomHashEnsemble &h){
  c.hashes.clear();
  
  for(int i=0;i<h.hashes.size();i++){
    GeomHashBridge b;
    to_GeomHashBridge(b,h.hashes[i]);
    c.hashes.push_back(b);
  }
}

void to_GeomHashEnsemble(GeomHashEnsembleBridge &c,GeomHashEnsemble &h){
  h.hashes.clear();
  
  for(int i=0;i<c.hashes.size();i++){
    GeomHash b;
    to_GeomHash(c.hashes[i],b);
    h.hashes.push_back(b);
  }
}


#endif
