#ifndef _RICHANALYSIS_
#define _RICHANALYSIS_

#include "typedefs.h"
#include "TObject.h"
#include "TH2D.h"
#include "math.h"


///// A geometrical hash useful for multidimensional studies

namespace GeomHashConstants{
  const int nBuffers=4;
};

using namespace GeomHashConstants;

// Faster (hopefully much faster) implementation of the tool
class GeomHash: public TObject{
 public:
  static bool storeEntries;
  static bool storeTemplates;
  static bool storeTemplatesRms;
  static bool storeMean;
  static bool storeRms;
  static bool storePeak;
  static bool storePeakWidth;
  static bool computePeakAsMean;
  static float peakFinderFraction; 

  static void setDefaults(){
    GeomHash::storeEntries=true;
    GeomHash::storeTemplates=true;
    GeomHash::storeTemplatesRms=true;
    GeomHash::storeMean=true;
    GeomHash::storeRms=true;
    GeomHash::storePeak=true;
    GeomHash::storePeakWidth=true;
    GeomHash::computePeakAsMean=false;
    GeomHash::peakFinderFraction=0.68;
  }

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

  GeomHash(int d=1);

  float *getTemplate(int h);
  float *getTemplateRms(int h);

  inline int offset(int which){return which*dimension;}
  //Evaluating
  int hash(float *point);
  int get(double x,...);

  // Virtual function
  void store(int *pointer,int size,int parent);                                           // Storing information in the tree

  // Growing
  vector<float> samples;  //! Vector storing all the samples with the format x0,x1,x2...xn,y,weight
  vector<float> values;   //! Vector storing the guiding values
  void push(float value,float *x);
  void fill(double x,...);


  void grow(int min_size=0);

  void grow(int *pointers,                          // Buffer pointing to the points indexes
	    int *scratch,
	    int min_size=0);        // Scratch region to store the distances to the points


  // grow a tree trying tries time for optimization, and with minSize minimum elements per node. size is the number of samples to be used
  // stored in buffer (as indexes to samples) and scratch is a temporary scratch space
  void  grow_internal(int *pointers,
                      int  size,
		      int *scratch,
                      int parent=0);



  float getMean(int node);
  float getRms(int node);
  float getPeak(int node);
  float getPeakWidth(int node);

  int getEntries(int node);


  // Some internal buffers
  static float *buffer[GeomHashConstants::nBuffers]; //!
  static int    bufferSize;       //!
  void checkBuffers();


ClassDef(GeomHash,1);
#ifdef VERSION6
#else
#ifndef __PPC64
#pragma omp threadprivate(fgIsA)
#endif
#endif
};


class GeomHashEnsemble: public GeomHash{
 public:
  GeomHashEnsemble(int d=1):GeomHash(d){}

  vector<GeomHash> hashes;
  void growOne(int minSize=10,bool bootstrap=false);
  void Eval(float *x);
  void Eval(double x,...);

  // Result from last evaluation
  double MeanValue;
  double ValueRms;
  double MeanRms;
  double RmsRms;
  double MeanPeak;
  double PeakRms;
  double MeanPeakWidth;
  double PeakWidthRms;
  double MeanEntries;
  double EntriesRms;
  int    Hashes;

  int numHashes();
  GeomHash &getHash(int i);

  static GeomHashEnsemble *readBin(TString input);
  
  ClassDef(GeomHashEnsemble,1);
#ifdef VERSION6
#else
#ifndef __PPC64
#pragma omp threadprivate(fgIsA)
#endif
#endif
};


void merge(GeomHashEnsemble &receiver,GeomHashEnsemble &small); // Merge several ensembless, to parallelize learning


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Additional containers classes /////////////////////////////////////
//////////////////////////////////////// for V2                        /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////




class GAMModel:public TObject{
public:
  std::vector<double> buffer;      // Buffer to store the weights
  std::vector<std::vector<float> > centroids;
  std::vector<double> scaling;    
  std::vector<double> means;
  TH2D index_correction;


  GAMModel(){}
  
  void retrieve_weights(float *args);
  void retrieve_weights(double *args);
  double eval(float *args);
  double eval(double *args);  
  double get_index_correction(float x,float y);

  static GAMModel *readBin(TString input);

  ClassDef(GAMModel,2);
#ifdef VERSION6
#else
#ifndef __PPC64
#pragma omp threadprivate(fgIsA)
#endif
#endif
};




///////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// TOOLS TO LEARN GAM MODELS //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

// Some linear algebra template classes

template <class T> class Matrix{
 public:
  typedef std::vector<T>   Data;
  Data _data;
  bool _transposed;   // This affects the accesor
  unsigned int _rows;
  unsigned int _columns;

  // Constructors
  Matrix(){};
  Matrix(int rows,int columns){initialize(rows,columns);}
  Matrix(std::vector<T> x){
    _transposed=false;
    _rows=x.size();
    _columns=1;
    _data=x;
  }

  
  void initialize(int rows,int columns){
    _transposed=false;
    _rows=rows;
    _columns=columns;
    _data.resize(_rows*_columns);
  }


  // Modifiers
  Matrix<T>& t(){_transposed=!_transposed;return *this;}
  

  // accessor
  T& operator()(unsigned int i, unsigned int j){
    int which=_transposed?_columns*j+i:_columns*i+j;
    return _data[which];
  }

  T& operator()(unsigned int i, unsigned int j,bool safe){
    int which=_transposed?_columns*j+i:_columns*i+j;
    return _data.at(which);
  }
T& operator()(unsigned int i){
    if(_rows!=1 && _columns!=1) throw("Matrix::operator(unsigned int i) Used with a matrix");
    return _data[i];
  }

  
  int rows() {return _transposed?_columns:_rows;}
  int columns() {return _transposed?_rows:_columns;}


  // Operations: they modify the receiver object
  // This is done on purpose, so that user can control copies.
  // a better strategy requires more code
  Matrix<T>& add( Matrix<T> &other){
    if(rows() != other.rows() || columns() !=other.columns()) throw("Matrix::add dimensions do not match");
    for(unsigned int i=0;i<other._data.size();i++) _data[i]+=other._data[i];
    return *this;
  }


  Matrix<T>& mult( double value){for(unsigned int i=0;i<_data.size();i++) _data[i]*=value; return *this;}
  Matrix<T>& mult( float value){for(unsigned int i=0;i<_data.size();i++) _data[i]*=value; return *this;}
  
  Matrix<T>& multDirect( Matrix<T> &other){
    if(rows() != other.rows() || columns() !=other.columns()) throw("Matrix::multDirect dimensions do not match");
    for(unsigned int i=0;i<other._data.size();i++) _data[i]*=other._data[i];
    return *this;
  }
    
    
  Matrix<T>& mult( Matrix<T> &other){
    if(columns() != other.rows()) throw("Matrix::mult dimensions do not match");
    Matrix<T> current=*this; // Make a copy ans use current one as repository for the result
    initialize(current.rows(),other.columns());

    // There are more inteligent ways of doing this, aligning data, for example
    for(int i=0;i<rows();i++) 
      for(int j=0;j<columns();j++){
        double value=0;
          for(int k=0;k<current.columns();k++){
            value+=current(i,k)*other(k,j);
	             }
        (*this)(i,j)=value;
      }
    return *this;
  }


  Matrix<T>& print(const char *message=""){
    using namespace std;

    cout<<message<<endl;
    for(int i=0;i<rows();i++){
      for(int j=0;j<columns();j++) cout<<(*this)(i,j)<<"\t";
      cout<<endl;
    }
    return (*this);
  }

  Matrix<T>& log(const char *message=""){
    return *this;
  }

  // Serializing is missing
};


// A least squares online fitter

template <class T> class LSFitter{
 public:
  Matrix<T> _covariance;  // covariance
  Matrix<T> _mean;
  int _rows;

  // Empty one
  LSFitter():_rows(0){}

  // Initialize with an initial error
  LSFitter(int rows,T error=1):_rows(rows){
    error*=error;; // I need to understand this. It works better with error=1e-3 !!!!! and should be 1e3
    _covariance.initialize(_rows,_rows);
    _mean.initialize(_rows,1);
    for(int i=0;i<_rows;i++) _covariance(i,i)=error;
    for(int i=0;i<_rows;i++) _mean(i,0)=0;
  }

  LSFitter(std::vector<T> values,T error=1){
    error*=error;
    _rows=values.size();
    _covariance.initialize(_rows,_rows);
    _mean.initialize(_rows,1);
    for(int i=0;i<_rows;i++) _covariance(i,i)=error;
    for(int i=0;i<_rows;i++) _mean(i,0)=values[i];
  }


  LSFitter(std::vector<T> values,std::vector<T> errors){
    _rows=values.size();
    _covariance.initialize(_rows,_rows);
    _mean.initialize(_rows,1);
    for(int i=0;i<_rows;i++) _covariance(i,i)=pow(errors[i],2);
    for(int i=0;i<_rows;i++) _mean(i,0)=values[i];
  }

  /////////////// Accessor: should be unchangeable
  Matrix<T>& error2(){return _covariance;}
  Matrix<T>& mean(){return _mean;}
  ///////////// Update
  void update(std::vector<T> x_org,T y,T error=1){
    if(error==0) throw("LSFitter::update null error makes no sense.");
    Matrix<T> x(x_org);
    x.mult(T(1.0)/error);
    y/=error;

    // Update inverse covariance

    _covariance.log("------ Starting covariance ------");

    auto A=_covariance; A.mult(x); 
    auto result=A; result.mult(A.t()); A.t();

    result.log("----- Numerator ----");


    auto temp=x; temp
                   .t()
                   .mult(A);

    temp.log("----- Denominator ------");


    // Some checks
    if(temp.rows()!=1 || temp.columns()!=1) throw("LSFitter::update Updading inv covariance resulted ina matrix when expecting a scalar");
    if(temp(0,0)==T(-1.0)) throw("LSFitter::update Updading inv covariance resulted in a zero scalar when expecting non-zero");
    if(result.rows()!=_covariance.rows() || result.columns()!=_covariance.columns()) throw("LSFitter::update Updading inv covariance resulted in a wrond dimension");

    result.mult(T(-1.0)/(T(1.0)+temp(0,0)));

    _covariance.add(result);
    
    // Update the mean
    temp=x;
    auto delta=(temp.t().mult(_mean)(0,0)-y);
    temp=_covariance; temp.mult(x).mult(-delta);
    _mean.add(temp);
  }

};


// A full GAM model with fitting buildon compatible with the GAMModel format

class GAM{
public:
  unsigned int dimensions;                              // Number of dimensions
  std::vector<std::vector<float> > centroids;  // Centroid for each dimension function
  std::vector<float>  centroid_x2;             // Store the x**2 addition of the centroids values to scale the values. This increases the stability of the fit
  std::vector<double> buffer;      // Buffer to store the weights
  std::vector<double> scaling;
  LSFitter<double> fitter;

  GAM():dimensions(0){};

  GAM(unsigned int d):dimensions(d){
    centroids.assign(dimensions,std::vector<float>());
    centroid_x2.assign(dimensions,0);
  }

  void initialize(int d){
    dimensions=d;
    centroids.assign(dimensions,std::vector<float>());
    centroid_x2.assign(dimensions,0);
  }


  // Add one centroid for a given dimension for the definition of the functions
  void add_centroid(int d,float value){
    centroid_x2[d]+=value*value;
    centroids[d].push_back(value);
  }

  // Compute a scaling parameter to improve stability of the fit
  void initialize_scaling(){
    scaling.assign(dimensions,1);
    for(unsigned int i=0;i<dimensions;i++) if(centroids[i].size()) scaling[i]=sqrt(centroid_x2[i]/centroids[i].size());  
  }

  // retrieve the weights for a given point
  void retrieve_weights(float *args){
    if(scaling.size()!=dimensions) initialize_scaling();

    buffer.clear();
    buffer.push_back(1); // This is the constant part of the fit

    for(unsigned int i=0;i<dimensions;i++) buffer.push_back(args[i]); // Linear part
    
    for(unsigned int i=0;i<dimensions;i++) 
      for(auto &v: centroids[i]) buffer.push_back(pow(fabs(v-args[i])/scaling[i],3));
  }

  void retrieve_weights(double *args){
    if(scaling.size()!=dimensions) initialize_scaling();

    buffer.clear();
    buffer.push_back(1); // This is the constant part of the fit

    for(unsigned int i=0;i<dimensions;i++) buffer.push_back(args[i]); // Linear part
    
    for(unsigned int i=0;i<dimensions;i++) 
      for(auto &v: centroids[i]) buffer.push_back(pow(fabs(v-args[i])/scaling[i],3));
  }


  ///////////////////////// fit a value /////////////////////
  bool fit(float *args,double value,double error=1){
    retrieve_weights(args);
    if(fitter._rows!=int(buffer.size())){
      fprintf(stderr, "Internal error: "
              " start_fit not called. Falling back to default initialization."
              "at %s, line %d.",
              __FILE__, __LINE__);
      return false;
    }
    fitter.update(buffer,value,error);
    return true;
  }
  
  bool fit(double *args,double value,double error=1){
    retrieve_weights(args);
    if(fitter._rows!=int(buffer.size())){
      fprintf(stderr, "Internal error: "
              " start_fit not called. Falling back to default initialization."
              "at %s, line %d.",
              __FILE__, __LINE__);
      return false;
    }
    fitter.update(buffer,value,error);
    return true;
  }
  /////////////////// initialize fitter /////////////////
  void start_fit(double mean=0,double mean_error=1e-2,double general_error=1e-1){
    int n=1+dimensions;
    for(auto &a:centroids) n+=a.size();
    vector<double> values,errors;
    values.assign(n,0);
    errors.assign(n,general_error);
    values[0]=mean;
    errors[0]=mean_error;
    
    fitter=LSFitter<double>(values,errors);
  }

  GAMModel &finish_fit(){
    GAMModel *output=new GAMModel;
    output->centroids=centroids;
    output->scaling=scaling;
    for(int i=0;i<fitter._rows;i++) output->means.push_back(fitter.mean()(i));
    return *output;
  }


};



#endif





