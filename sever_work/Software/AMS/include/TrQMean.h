#ifndef __TrQMean__
#define __TrQMean__

#include <cmath>
#include <vector>


//! Class used to retrieve results of a mean calculation
class mean_t {
 public: 
  //! Mean type
  int   Type;
  //! Signal options
  int   Opt; 
  //! Side 
  int   Side;
  //! Number of points 
  int   NPoints;
  //! Mean
  float Mean;
  //! RMS
  float RMS;
  //! Constructor
  mean_t() { clear(); }
  //! Constructor
  mean_t(int t, int o = 0, int s = 0, int n = 0, float m = 0, float r = 0) { 
    clear(); Type = t; Opt = o; Side = s; NPoints = n; Mean = m; RMS = r; 
  }
  //! Copy constructor
  mean_t(const mean_t &that) { copy(that); } 
  //! Assignment operator
  mean_t& operator=(const mean_t& that) { if (this!=&that) copy(that); return *this; }
  //! Destructor
  ~mean_t() { clear(); }
  //! Clear
  void clear() { 
    Type = 0; Opt = 0; Side = 0; NPoints = 0; Mean = 0; RMS = 0; 
  }
  //! Copy
  void copy(const mean_t &that) { 
    Type = that.Type; Opt = that.Opt; Side = that.Side; NPoints = that.NPoints; Mean = that.Mean; RMS = that.RMS;
  }
  //! Print
  void print() {
    printf("Type %4x  Opt %4x  Side %1d  NPoints %1d  Mean %7.2f  RMS %7.2f\n",
           Type,Opt,Side,NPoints,Mean,RMS);
  }
};


//! Class used to retrieve results of a likelihood calculation
class like_t {
 public:
  //! Loglikelihood computation type
  int    Type;
  //! Pdf version (also includes option type)
  int    Ver;
  //! Side 
  int    Side;
  //! Charge hypothesis (0: e, 1: p, 2: He, ...)
  int    Z;
  //! Number of points 
  int    NPoints;
  //! Likelihood
  float  LogLike; 
  //! Probability
  float  Prob;
  //! Mean
  float  Mean;
  //! Constructor
  like_t() { clear(); }
  //! Constructor
  like_t(int t, int v = 0, int s = 0, int z = -1, int n = 0, float ll = -30, float p = 0, float m = 0) { 
    clear(); Type = t; Ver = v; Side = s; Z = z; NPoints = n; LogLike = ll; Prob = p; Mean = m; 
  }
  //! Destructor
  ~like_t() { clear(); }
  //! Copy constructor
  like_t(const like_t &that) { copy(that); }
  //! Assignment operator
  like_t& operator=(const like_t &that) { if (this!=&that) copy(that); return *this; }
  //! Copy
  void copy(const like_t &that) {
    Type = that.Type; Ver = that.Ver; Side = that.Side; Z = that.Z; NPoints = that.NPoints; LogLike = that.LogLike; Prob = that.Prob; Mean = that.Mean;
  }
  //! Relational operator <
  bool operator<(const like_t& that) const { return GetNormLogLike()<that.GetNormLogLike(); }
  //! Relational operator >
  bool operator>(const like_t& that) const { return GetNormLogLike()>that.GetNormLogLike(); }
  //! Relational operator <=
  bool operator<=(const like_t& that) const { return GetNormLogLike()<=that.GetNormLogLike(); }
  //! Relational operator >=
  bool operator>=(const like_t& that) const { return GetNormLogLike()>=that.GetNormLogLike(); }
  //! Clear
  void clear() { 
    Type = 0; Ver = 0; Side = 0; Z = -1; NPoints = 0; LogLike = -30; Prob = 0; Mean = 0; 
  }
  //! Print
  void print() {
    printf("Type %4x  Ver %2d  Side %1d  Z %2d  NPoints %2d  LogLike %7.3f  Prob %7.5f  Mean %7.2f NormLogLike %7.3f\n",
           Type,Ver,Side,Z,NPoints,LogLike,Prob,Mean,GetNormLogLike());
  }
  //! Normalized LogLike
  float GetNormLogLike() const { return (NPoints>0) ? LogLike/NPoints : -30; }
  //! Normalized Probability
  float GetNormProb() const { return (NPoints>0) ? pow(10,GetNormLogLike()) : 0; }    
};

#endif
