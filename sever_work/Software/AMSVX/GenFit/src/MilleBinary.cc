/*
 * MilleBinary.cpp
 *
 *  Created on: Aug 31, 2011
 *      Author: kleinwrt
 */

/** \file
 *  MilleBinary methods.
 *
 *  \author Claus Kleinwort, DESY, 2011 (Claus.Kleinwort@desy.de)
 *
 *  \copyright
 *  Copyright (c) 2011 - 2016 Deutsches Elektronen-Synchroton,
 *  Member of the Helmholtz Association, (DESY), HAMBURG, GERMANY \n\n
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version. \n\n
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details. \n\n
 *  You should have received a copy of the GNU Library General Public
 *  License along with this program (see the file COPYING.LIB for more
 *  details); if not, write to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <iostream>
#include "MilleBinary.h"
#ifdef _OPENMP
#include <omp.h>
#endif
//! Namespace for the general broken lines package
namespace gbl {

MilleEvent::MilleEvent(bool doublePrec,unsigned int aSize):intBuffer(), floatBuffer(), doubleBuffer(){
  intBuffer.reserve(aSize);
  intBuffer.push_back(0); // first word is error counter
  if(doublePrec) {
    doubleBuffer.reserve(aSize);
    doubleBuffer.push_back(0.);
  } else {
    floatBuffer.reserve(aSize);
    floatBuffer.push_back(0.);
  }
}

int MilleEvent::lenData(){
  if(isdoublePrec()){if(doubleBuffer.size()!=intBuffer.size())return -1;}
  else              {if(floatBuffer.size() !=intBuffer.size())return -1;}
  return intBuffer.size();
}

int MilleEvent::Clear(){
  intBuffer.resize(1);
  if (isdoublePrec())
    doubleBuffer.resize(1);
  else
    floatBuffer.resize(1);
  return 0;
}


/// Add data block to (end of) record.
/**
 * \param [in] aMeas Value
 * \param [in] aErr Error
 * \param [in] indLocal List of labels of local parameters
 * \param [in] derLocal List of derivatives for local parameters
 * \param [in] labGlobal List of labels of global parameters
 * \param [in] derGlobal List of derivatives for global parameters
 */
void MilleEvent::addData(double aMeas, double aErr,
		const std::vector<unsigned int> &indLocal,
		const std::vector<double> &derLocal, const std::vector<int> &labGlobal,
		const std::vector<double> &derGlobal) {

  if(isdoublePrec()) {
    intBuffer.push_back(0);
    doubleBuffer.push_back(aMeas);
    for(unsigned int i = 0; i < indLocal.size(); ++i) {
      intBuffer.push_back(indLocal[i]);
      doubleBuffer.push_back(derLocal[i]);
    }
    intBuffer.push_back(0);
    doubleBuffer.push_back(aErr);
    for(unsigned int i = 0; i < labGlobal.size(); ++i) {
      if(derGlobal[i]) {
        intBuffer.push_back(labGlobal[i]);
        doubleBuffer.push_back(derGlobal[i]);
      }
    }
  } else {
    intBuffer.push_back(0);
    floatBuffer.push_back(aMeas);
    for(unsigned int i = 0; i < indLocal.size(); ++i) {
      intBuffer.push_back(indLocal[i]);
      floatBuffer.push_back(derLocal[i]);
    }
    intBuffer.push_back(0);
    floatBuffer.push_back(aErr);
    for(unsigned int i = 0; i < labGlobal.size(); ++i) {
      if(derGlobal[i]) {
        intBuffer.push_back(labGlobal[i]);
        floatBuffer.push_back(derGlobal[i]);
       }
    }
  }
}

#ifdef USELONGINTPEDE
void MilleEvent::read(double *bufferDouble, float *bufferFloat, long int *bufferInt, long int *lengthBuffers, long int *errorFlag)
#else
void MilleEvent::read(double *bufferDouble, float *bufferFloat, int *bufferInt, int *lengthBuffers, int *errorFlag) //{
#endif
{
  if (!errorFlag)return;
  if(!bufferFloat||!bufferInt||!lengthBuffers) {*errorFlag = -1;return;}
//-----
  int dlength=lenData();
  if(dlength<=1){
    std::cerr<<"err MilleEvent::read length="<<dlength<<std::endl;
    *errorFlag=-2;
    return;
  }
  if(isdoublePrec()){
    if(int(doubleBuffer.size())>*lengthBuffers){
      *lengthBuffers=doubleBuffer.size();
      std::cerr<<"err MilleEvent::read dbuffers too short="<<(*lengthBuffers)<<"<"<<(doubleBuffer.size())<<std::endl;
      *errorFlag=-4;
      return;
    }
    *lengthBuffers=doubleBuffer.size();
    std::copy(doubleBuffer.begin(),doubleBuffer.end(),bufferDouble);
    *errorFlag=8;
  }
  else {
    if(int(floatBuffer.size())>*lengthBuffers){
      *lengthBuffers=floatBuffer.size();
      std::cerr<<"err MilleEvent::read fbuffers too short="<<(*lengthBuffers)<<"<"<<(floatBuffer.size())<<std::endl;
      *errorFlag=-4;
      return;
    }
    *lengthBuffers=floatBuffer.size();
    std::copy(floatBuffer.begin(),floatBuffer.end(),bufferFloat);
    std::copy(bufferFloat,bufferFloat+(*lengthBuffers),bufferDouble);//copy float to double
    *errorFlag=4;
  }
  std::copy(intBuffer.begin(),intBuffer.end(),bufferInt);
}

/// Create binary file.
/**
 * \param [in] fileName File name
 * \param [in] doublePrec Flag for storage as double values
 * \param [in] aSize Buffer size
 */
MilleBinary::MilleBinary(const std::string fileName, bool doublePrec,
                unsigned int aSize, int usegz) : usegzFile(usegz),fs(0),ts(0),ch(0),entry(0) {
  fname=fileName;
  ev=new MilleEvent(doublePrec,aSize);
  if     (usegzFile==0)binaryFile.open(fileName.c_str(), std::ios::binary | std::ios::out);
  else if(usegzFile==1)gzbinaryFile=gzopen(fileName.c_str(),"wb");
  else if(usegzFile>=2){
    fs=new TFile(fileName.c_str(),"RECREATE");
    bookTree();
  }
  else {
#pragma omp critical (millebinarych)
   {
    bookTree();
#ifdef _OPENMP
//    std::cout <<"  thread0="<<omp_get_thread_num()<<" name="<<fileName.c_str()<<std::endl;
#endif
    if(fileName.size()>=1){ch->Add(fileName.c_str());ch->GetEntries();}
#ifdef _OPENMP
//    std::cout <<"  thread2="<<omp_get_thread_num()<<" name="<<fileName.c_str()<<","<<(ch->GetEntries())<<std::endl;
#endif
   }
  }

}

MilleBinary::MilleBinary(TFile *fo,bool doublePrec,
                unsigned int aSize) : usegzFile(2),fs(0),ts(0),ch(0),entry(0) {
  ev=new MilleEvent(doublePrec,aSize);
  if(fo)fo->cd();
  bookTree();	
}


int MilleBinary::bookTree(){
  if(usegzFile>=2){
    ts=new TTree("milltree","milltree");
    ts->Branch("evbranch",&ev,32000,99);
  }
  else if(usegzFile<0){
    ch=new TChain("milltree");
    ch->SetBranchAddress("evbranch",&ev);
    entry=0;
  }
  return usegzFile;
}


int MilleBinary::resetChain(){
  if(usegzFile<0){ch->Reset();entry=0;return 0;}
  else return -1;
}

MilleBinary::~MilleBinary() {
  if(ev)delete ev;
  if     (usegzFile==0)binaryFile.close();
  else if(usegzFile==1)gzclose(gzbinaryFile);
  else if(usegzFile>=2){if(ts)ts->Write();if(fs)delete fs;}
  else {
#pragma omp critical (millebinaryclean)
   {
//     std::cout<<"delete millebinary"<<std::endl;
//    if(ch)delete ch;//ch should delete, however, it will crash in multi-threads. To be fixed
   }
  }
}


void MilleBinary::addData(double aMeas, double aErr,
                const std::vector<unsigned int> &indLocal,
                const std::vector<double> &derLocal, const std::vector<int> &labGlobal,
                const std::vector<double> &derGlobal) {
  ev->addData(aMeas,aErr,indLocal,derLocal,labGlobal,derGlobal);
}

/// Write record to file.
void MilleBinary::writeRecord() {

  if     (usegzFile<0)return;
  else if(usegzFile>=2){ts->Fill();}
  else {
    const int recordLength=(ev->isdoublePrec()) ? -ev->intBuffer.size() * 2 : ev->intBuffer.size() * 2;
    if(usegzFile>0)gzwrite(gzbinaryFile,reinterpret_cast<const char*>(&recordLength),sizeof(recordLength));
    else           binaryFile.write(reinterpret_cast<const char*>(&recordLength),sizeof(recordLength));
    if(ev->isdoublePrec()){
      if(usegzFile>0)gzwrite(gzbinaryFile,reinterpret_cast<char*>(&(ev->doubleBuffer[0])),ev->doubleBuffer.size() * sizeof(ev->doubleBuffer[0]));
      else          binaryFile.write(reinterpret_cast<char*>(&(ev->doubleBuffer[0])),ev->doubleBuffer.size() * sizeof(ev->doubleBuffer[0]));
    }
    else {
      if(usegzFile>0)gzwrite(gzbinaryFile,reinterpret_cast<char*>(&(ev->floatBuffer[0])),ev->floatBuffer.size() * sizeof(ev->floatBuffer[0]));
      else           binaryFile.write(reinterpret_cast<char*>(&(ev->floatBuffer[0])),ev->floatBuffer.size() * sizeof(ev->floatBuffer[0]));
    }
    if(usegzFile>0)gzwrite(gzbinaryFile,reinterpret_cast<char*>(&(ev->intBuffer[0])),ev->intBuffer.size() * sizeof(ev->intBuffer[0]));
    else           binaryFile.write(reinterpret_cast<char*>(&(ev->intBuffer[0])),ev->intBuffer.size() * sizeof(ev->intBuffer[0]));
  }
// start with new record
  ev->Clear();
}

int MilleBinary::readRecord(Long64_t i){
   if(usegzFile>=0)return -1;
   else {
     if(i>=0)entry=i;
#ifdef _OPENMP
//   std::cout <<"  thread="<<omp_get_thread_num()<<std::endl;
#endif
     if(entry>=ch->GetEntries()){ev->Clear(); return -2;}//entry overflow
     else {ch->GetEntry(entry);entry++;return 0;}
   }
}

}
