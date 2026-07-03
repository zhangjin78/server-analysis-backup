
/** \file
 *  Read from C/C++ binary files.
 *
 * \author Gero Flucke, University Hamburg, 2006
 * \author Claus Kleinwort, DESY (maintenance and developement)
 *
 *  \copyright
 *  Copyright (c) 2009 - 2019 Deutsches Elektronen-Synchroton,
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
 *
 *  C-methods to handle input of C/C++ binary files as input for
 *  the fortran **pede** program (see \ref peread).
 *  This includes macros utilising \c cfortran.h to allow direct callability
 *  from fortran.
 *
 *  \c initC() has to be called once in the beginning,
 *  followed by one or several calls to \c openC() to open one or several files.
 *  \c readC() is then called to read the records sequentially. \c resetC()
 *  allows to rewind files.
 *
 *  If compiled with preprocessor macro \c USE_SHIFT_RFIO, uses \c libRFIO,
 *  i.e. includes \c shift.h instead of \c stdio.h
 *
 *  If compiled with preprocessor macro \c USE_ZLIB, uses \c libz,
 *  enables direct reading of gzipped files.
 *
 *  Written by Gero Flucke (gero.flucke@cern.ch) in 2006/7
 *  - update on July 14th, 2008
 *  - update on October 29th, 2008: return for file number in \c readC()
 *
 *  Major updates on April 24th, 2012 by C.Kleinwort:
 *  - skip records larger than buffer size (to determine max record length)
 *  - dynamic allocation of file pointer list (no hard-coded max number of files)
 *
 *  Major update on February 26th, 2014 by C.Kleinwort:
 *  - implement reading of records containing doubles (instead of floats)
 *    indicated by negative record length.
 *
 *  Last major update on April 10th, 2019 by C.Kleinwort:
 *  - Option to close and reopen files 
 */

#ifdef USE_SHIFT_RFIO
#include <shift.h>
// or this??
// // needed?#define _LARGEFILE64_SOURCE
//#include <sys/types.h>
//#include "rfio_api.h"
#else
#include <stdio.h>
#endif
#include "cfortran.h"
#ifdef USE_ZLIB
#include <zlib.h>
#endif
#include <map>
#include <string>
#include <iostream>
#include "MilleBinary.h"
#ifdef USELONGINTPEDE
typedef long intpede;
#else
typedef int intpede;
#endif
/* ________ global variables used for file handling __________ */

#ifdef USE_ZLIB
gzFile **files;   ///< pointer to list of pointers to opened binary files
#else
FILE **files;     ///< pointer to list of pointers to opened binary files
#endif
std::string *fnames;
gbl::MilleBinary *millehead=0;
#pragma omp threadprivate (millehead)

unsigned int maxNumFiles;      ///< max number of files
unsigned int numAllFiles;      ///< number of opened files

/*______________________________________________________________*/
/// Initialises the 'global' variables used for file handling.
/**
 * \param[in]  nFiles  Maximal number of C binary files to use.
 */
void initC(intpede *nFiles) {
	maxNumFiles = *nFiles;
#ifdef USE_ZLIB
	printf(" initC: using zlib version %s\n",ZLIB_VERSION);
	files = (gzFile **) malloc(sizeof(gzFile *)*maxNumFiles);
#else
	files = (FILE **) malloc(sizeof(FILE *) * maxNumFiles);
#endif
	{
		intpede i = 0;
		for (; i < maxNumFiles; ++i) {
			files[i] = 0;
		}
	}
        fnames=new std::string[maxNumFiles];
	numAllFiles = 0;
}
#ifdef USELONGINTPEDE
FCALLSCSUB1( initC, INITC, initc, PLONG)
#else
FCALLSCSUB1( initC, INITC, initc, PINT)
#endif
/*______________________________________________________________*/

/* void rewinC() */
/* { */
/*   /\* rewind all open files and start again with first file *\/ */

/*   unsigned int i = numAllFiles; */
/*   while (i--) rewind(files[i]); /\* postfix decrement! *\/ */
/*   fileIndex = 0; */
/* } */
/* FCALLSCSUB0(rewinC,REWINC,rewinc) */

/*______________________________________________________________*/
/// Rewind file.
/**
 * \param[in]  nFileIn  File number (1 .. maxNumFiles)
 */
void resetC(intpede *nFileIn) {
	intpede fileIndex = *nFileIn - 1; /* index of current file */
	if (fileIndex < 0)
		return; /* no file opened at all... */
	if(fnames[fileIndex].find(".root")!=std::string::npos){
          if(millehead&&millehead->fname==fnames[fileIndex])millehead->rewind();
//          if(millehead){delete millehead;millehead=0;}
          return;
        }
#ifdef USE_ZLIB
	gzrewind(files[fileIndex]);
#else
	/* rewind(files[fileIndex]);  Does not work with rfio, so call: */
	fseek(files[fileIndex], 0L, SEEK_SET);
	clearerr_unlocked(files[fileIndex]); /* These two should be the same as rewind... */
#endif
}
#ifdef USELONGINTPEDE
FCALLSCSUB1( resetC, RESETC, resetc, PLONG)
#else
FCALLSCSUB1( resetC, RESETC, resetc, PINT)
#endif
/*______________________________________________________________*/
/// Close file.
/**
 * \param[in]  nFileIn  File number (1 .. maxNumFiles)
 */
void closeC(intpede *nFileIn) {
	intpede fileIndex = *nFileIn - 1; /* index of current file */
	if (fileIndex < 0)
		return; /* no file opened at all... */
	if(fnames[fileIndex].find(".root")!=std::string::npos){
	  if(millehead&&millehead->fname==fnames[fileIndex]){delete millehead;millehead=0;}
	  return;
 	}
#ifdef USE_ZLIB
	gzclose(files[fileIndex]);
#else
	fclose(files[fileIndex]);
#endif
        files[fileIndex] = 0;
        #pragma omp atomic 
        --numAllFiles; /* We have closed file! */
//	printf("atomic--");
}
#ifdef USELONGINTPEDE
FCALLSCSUB1( closeC, CLOSEC, closec, PLONG)
#else
FCALLSCSUB1( closeC, CLOSEC, closec, PINT)
#endif
/*______________________________________________________________*/
/// Open file.
void openC(const char *fileName, intpede *nFileIn, intpede *errorFlag)
/**
 * \param[in]  fileName  File name
 * \param[in]  nFileIn  File number (1 .. maxNumFiles) or <=0 for next one
 * \param[out] errorFlag error flag:
 *      * 0: if file opened and OK,
 *      * 1: if too many files open,
 *      * 2: if file could not be opened
 *      * 3: if file opened, but with error (can that happen?)
 */
{
	/* No return value since to be called as subroutine from fortran */

//        printf("numfile=%d_%d\n",numAllFiles,*nFileIn);
	if (!errorFlag)
		return; /* 'printout' error? */

	intpede fileIndex = *nFileIn - 1; /* index of specific file */
	if (fileIndex < 0) fileIndex = numAllFiles; /* next one */

	if (fileIndex >= maxNumFiles) {
		*errorFlag = 1;
	} else {
		std::string pname=fileName;
                fnames[fileIndex]=pname;
		if(pname.find(".root")==std::string::npos){//not rootfile
#ifdef USE_ZLIB
  		  files[fileIndex] = gzopen(fileName, "rb");
	  	  if (!files[fileIndex]) {
			*errorFlag = 2;
			return;
		  } 
#else
		  files[fileIndex] = fopen(fileName, "rb");
		  if (!files[fileIndex]) {
			*errorFlag = 2;
			return;
		  } else if (ferror_unlocked(files[fileIndex])) {
			fclose(files[fileIndex]);
			files[fileIndex] = 0;
			*errorFlag = 3;
			return;
		  }
#endif
                }
		else if((pname.find("eos")!=std::string::npos)&&(pname.find("root://eosams/")==std::string::npos)){//if rootfile
                  fnames[fileIndex]="root://eosams/"+pname;;
		}
		#pragma omp atomic 
		++numAllFiles; /* We have one more opened file! */
//		printf("index=%d numfile=%d_%s\n",fileIndex,numAllFiles,fileName);
		*errorFlag = 0;
	}
}
#ifdef USELONGINTPEDE
FCALLSCSUB3( openC, OPENC, openc, STRING, PLONG, PLONG)
#else
FCALLSCSUB3( openC, OPENC, openc, STRING, PINT, PINT)
#endif
/*______________________________________________________________*/
/// Read record from file.
/**
 * \param[out]    bufferDouble  read buffer for doubles
 * \param[out]    bufferFloat   read buffer for floats
 * \param[out]    bufferInt     read buffer for integers
 * \param[in,out] lengthBuffers in: buffer length, out: number of floats/ints in records
 *                              (> buffer size: record skipped)
 * \param[in]     nFileIn       File number (1 .. maxNumFiles)
 * \param[out]    errorFlag     error flag:
 *      *  -1: pointer to a buffer or lengthBuffers are null
 *      *  -2: problem reading record length
 *      *  -4: given buffers too short for record
 *      *  -8: problem with stream or EOF reading floats
 *      * -16: problem with stream or EOF reading ints
 *      * -32: problem with stream or EOF reading doubles
 *      *  =0: reached end of file (or read empty record?!)
 *      *  =4: found floats
 *      *  =8: found doubles
 */
void readC(double *bufferDouble, float *bufferFloat, intpede *bufferInt,
		intpede *lengthBuffers, intpede *nFileIn, intpede *errorFlag) {
	/* No return value since to be called as subroutine from fortran,
	 negative *errorFlag are errors, otherwise fine.

	 *nFileIn: number of the file the record is read from,
	 starting from 1 (not 0)
	 */
	intpede doublePrec = 0;

        intpede bufrawr=*lengthBuffers;
	if (!errorFlag)
		return;
	*errorFlag = 0;
	intpede fileIndex = *nFileIn - 1; /* index of current file */
	if (fileIndex < 0)
		return; /* no file opened at all... */
	if (!bufferFloat || !bufferInt || !lengthBuffers) {
		*errorFlag = -1;
		return;
	}

	if(fnames[fileIndex].find(".root")!=std::string::npos){//this is root file
	   if(millehead&&millehead->fname!=fnames[fileIndex]){
//		printf("readC: file change%s->%s\n",millehead->fname.c_str(),fnames[fileIndex].c_str());
		delete millehead; millehead=0;
	   }
//std::cout<<"file1="<<fileIndex<<","<<fnames[fileIndex]<<std::endl;
	   if(!millehead)millehead=new gbl::MilleBinary(fnames[fileIndex],false,2000,-1);
//std::cout<<"file2="<<fileIndex<<","<<fnames[fileIndex]<<std::endl;
	   int rstat=millehead->readRecord();
//           if(rstat==-2||millehead->getpEntry()>=100000){*errorFlag = 0;return;} /* Means EOF of file. */
	   if(rstat==-2){*errorFlag = 0;return;} /* Means EOF of file. */
	   else if(rstat<0){printf("readC: problem reading %s\n", fnames[fileIndex].c_str());*errorFlag = -2;}
	   return millehead->getEvent()->read(bufferDouble,bufferFloat,bufferInt,lengthBuffers,errorFlag);
        }
	/* read length of 'record' */
	int recordLength = 0; /* becomes number of words following in file */
#ifdef USELONGINTPEDE
        int bufferIntI[10000]={0};
        int *bufferIntR=bufferIntI;
#else
        int *bufferIntR=bufferInt;
#endif
#ifdef USE_ZLIB
	int nCheckR = gzread(files[fileIndex], &recordLength, sizeof(recordLength));
	if (gzeof(files[fileIndex])) {
		/* gzrewind(files[fileIndex]); CHK: moved to binrwd */
		*errorFlag = 0; /* Means EOF of file. */
		return;
	}
	if (recordLength<0) {
		doublePrec = 1;
		recordLength = -recordLength;
	}
	if (sizeof(recordLength) != nCheckR) {
		printf("readC: problem reading length of record file %d\n", fileIndex);
		*errorFlag = -2;
		return;
	}

	if (recordLength/2 > *lengthBuffers) {
		/*     printf("readC: given buffers too short (%d, need > %d)\n", *lengthBuffers,
		 recordLength/2); */
		/* skip floats */
		int i=0;
		if (doublePrec) {
			for (; i< recordLength/2; ++i)
			{
				int nCheckD = gzread(files[fileIndex], bufferDouble, sizeof(bufferDouble[0]));
				if (nCheckD != sizeof(bufferDouble[0])) {
					printf("readC: problem with stream or EOF skipping doubles\n");
					*errorFlag = -32;
					return;
				}
			}
		} else {
			for (; i< recordLength/2; ++i)
			{
				int nCheckF = gzread(files[fileIndex], bufferFloat, sizeof(bufferFloat[0]));
				if (nCheckF != sizeof(bufferFloat[0])) {
					printf("readC: problem with stream or EOF skipping floats\n");
					*errorFlag = -8;
					return;
				}
			}
		}
		i=0;
		/* skip ints */
		for (; i< recordLength/2; ++i)
		{
			int nCheckI = gzread(files[fileIndex], bufferIntR, sizeof(bufferIntR[0]));
			if (nCheckI != sizeof(bufferIntR[0])) {
				printf("readC: problem with stream or EOF skipping ints\n");
				*errorFlag = -16;
				return;
			}
		}

		*errorFlag = -4;
		*lengthBuffers = recordLength/2;
#ifdef USELONGINTPEDE
                std::copy(bufferIntR,bufferIntR+(*lengthBuffers),bufferInt);
#endif
		return;
	} else {
		*lengthBuffers = recordLength/2;
	}

	/* read floats (i.e. derivatives + value + sigma) */
	if (doublePrec) {
		int nCheckD = gzread(files[fileIndex], bufferDouble, *lengthBuffers*8);
		if (nCheckD != *lengthBuffers*8) {
			printf("readC: problem with stream or EOF reading doubles\n");
			*errorFlag = -32;
			return;
		}
	} else {
		int nCheckF = gzread(files[fileIndex], bufferFloat, *lengthBuffers*4);
		if (nCheckF != *lengthBuffers*4) {
			printf("readC: problem with stream or EOF reading floats\n");
			*errorFlag = -8;
			return;
		}
		int i=0;
		for (; i< recordLength/2; ++i) bufferDouble[i] = (double) bufferFloat[i];
	}

	/* read ints (i.e. parameter labels) */
	int nCheckI = gzread(files[fileIndex], bufferIntR, *lengthBuffers*4);
	if (nCheckI != *lengthBuffers*4) {
		printf("readC: problem with stream or EOF reading ints\n");
		*errorFlag = -16;
		return;
	}
#else
	size_t nCheckR = fread_unlocked(&recordLength, sizeof(recordLength), 1,
			files[fileIndex]);
	if (feof_unlocked(files[fileIndex])) {
		/* rewind(files[fileIndex]);  Does not work with rfio, so call: */
		/* fseek(files[fileIndex], 0L, SEEK_SET); CHK: moved to binrwd
		clearerr_unlocked(files[fileIndex]); These two should be the same as rewind... */
		*errorFlag = 0; /* Means EOF of file. */
                return;
	}

	if (1 != nCheckR || ferror_unlocked(files[fileIndex])) {
		printf("readC: problem reading length of record, file %d\n", fileIndex);
		*errorFlag = -2;
                return;
	}

	if (recordLength < 0) {
		doublePrec = 1;
		recordLength = -recordLength;
	}
	if (recordLength / 2 > *lengthBuffers) {
		/* printf("readC: given buffers too short (%d, need > %d)\n", *lengthBuffers,
		 recordLength/2); */
		/* skip floats */
		int i = 0;
		if (doublePrec) {
			for (; i < recordLength / 2; ++i) {
				size_t nCheckD = fread_unlocked(bufferDouble, sizeof(bufferDouble[0]), 1,
						files[fileIndex]);
				if (ferror_unlocked(files[fileIndex]) || feof_unlocked(files[fileIndex])
						|| nCheckD != *lengthBuffers) {
					printf(
							"readC: problem with stream or EOF skipping doubles\n");
					*errorFlag = -32;
					return;
				}
			}
		} else {
			for (; i < recordLength / 2; ++i) {
				size_t nCheckF = fread_unlocked(bufferFloat, sizeof(bufferFloat[0]), 1,
						files[fileIndex]);
				if (ferror_unlocked(files[fileIndex]) || feof_unlocked(files[fileIndex])
						|| nCheckF != *lengthBuffers) {
					printf(
							"readC: problem with stream or EOF skipping floats\n");
					*errorFlag = -8;
					return;
				}
			}
		}
		i = 0;
		/* skip ints */
		for (; i < recordLength / 2; ++i) {
			size_t nCheckI = fread_unlocked(bufferIntR, sizeof(bufferIntR[0]), 1,
					files[fileIndex]);
			if (ferror_unlocked(files[fileIndex]) || feof_unlocked(files[fileIndex])
					|| nCheckI != *lengthBuffers) {
				printf("readC: problem with stream or EOF skiping ints\n");
				*errorFlag = -16;
 				return;
			}
		}

		*errorFlag = -4;
		*lengthBuffers = recordLength / 2;
#ifdef USELONGINTPEDE
                std::copy(bufferIntR,bufferIntR+(*lengthBuffers),bufferInt);
#endif
		return;
	} else {
		*lengthBuffers = recordLength / 2;
	}

//        printf("index=%d nfl=%d",fileIndex,*lengthBuffers);
	/* read floats (i.e. derivatives + value + sigma) */
        intpede bufraw=*lengthBuffers;
	if (doublePrec) {
		size_t nCheckD = fread_unlocked(bufferDouble, sizeof(bufferDouble[0]),
				*lengthBuffers, files[fileIndex]);
		if (ferror_unlocked(files[fileIndex]) || feof_unlocked(files[fileIndex])
				|| nCheckD != *lengthBuffers) {
			printf("readC: problem with stream or EOF reading doubles\n");
			*errorFlag = -32;
			return;
		}
	} else {
		size_t nCheckF = fread_unlocked(bufferFloat, sizeof(bufferFloat[0]),
				*lengthBuffers, files[fileIndex]);
		if (ferror_unlocked(files[fileIndex]) || feof_unlocked(files[fileIndex])
				|| nCheckF != *lengthBuffers) {
			printf("readC: problem with stream or EOF reading floats\n");
			*errorFlag = -8;
			return;
		}
		int i = 0;
		for (; i < recordLength / 2; ++i)
			bufferDouble[i] = (double) bufferFloat[i];
	}
	/* read ints (i.e. parameter labels) */
	size_t nCheckI = fread_unlocked(bufferIntR, sizeof(bufferIntR[0]), *lengthBuffers,
			files[fileIndex]);
	if (ferror_unlocked(files[fileIndex]) || feof_unlocked(files[fileIndex])
			|| nCheckI != *lengthBuffers) {
		printf("readC: problem with stream or EOF reading ints\n");
		*errorFlag = -16;
		return;
	}
#endif
#ifdef USELONGINTPEDE
        std::copy(bufferIntR,bufferIntR+(*lengthBuffers),bufferInt);
#endif
	*errorFlag = 4 * (doublePrec + 1);
}
#ifdef USELONGINTPEDE
FCALLSCSUB6(readC,READC,readc,PDOUBLE,PFLOAT,PLONG,PLONG,PLONG,PLONG)
#else
FCALLSCSUB6(readC,READC,readc,PDOUBLE,PFLOAT,PINT,PINT,PINT,PINT)
#endif
