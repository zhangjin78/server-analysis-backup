*  $Id: sgif.F,v 1.7 2018/01/05 15:20:42 choutko Exp $
                 subroutine amsalign
		  end
		 subroutine ipm
		 end
		 subroutine ispmci
		 end
                 subroutine closef(unit)
                 integer unit
                 close(unit) 
                 end

*
* $Id: sgif.F,v 1.7 2018/01/05 15:20:42 choutko Exp $
*
* $Log: sgif.F,v $
* Revision 1.7  2018/01/05 15:20:42  choutko
* *** empty log message ***
*
* Revision 1.6  2016/06/09 14:31:52  choutko
* *** empty log message ***
*
* Revision 1.5  2016/06/08 13:13:09  bshan
* To support JUQUEEN CNK system which is based on IBM BlueGene/Q.
*
* MPIEmulator: An MPI simulator to initialize and finalize the jobs submitted to JUQUEEN.
*
* system_bg:   A wrapper to implement some of the shell commands in CNK system, for systems other than CNY, ::system(cmd_line) is returned.
*
* Revision 1.4  2016/06/01 10:02:35  choutko
* ppc64 phase2
*
* Revision 1.2  1996/04/02 23:17:49  thakulin
* Add support for EPC Fortran:  remove char-int equivalences and use
* F90 transfer facility instead.
*
* Revision 1.1.1.1  1996/02/15 17:50:15  mclareni
* Kernlib
*
*




#ifdef __PPC64


      SUBROUTINE UHTOC (MS,NPW,MT,NCH)
C
C CERN PROGLIB# M409    UHTOC           .VERSION KERNFOR  4.21  890323
C ORIG. 10/02/89  JZ
C
      DIMENSION    MS(99)
      CHARACTER    MT*99


*
* $Id: sgif.F,v 1.7 2018/01/05 15:20:42 choutko Exp $
*
* $Log: sgif.F,v $
* Revision 1.7  2018/01/05 15:20:42  choutko
* *** empty log message ***
*
* Revision 1.6  2016/06/09 14:31:52  choutko
* *** empty log message ***
*
* Revision 1.5  2016/06/08 13:13:09  bshan
* To support JUQUEEN CNK system which is based on IBM BlueGene/Q.
*
* MPIEmulator: An MPI simulator to initialize and finalize the jobs submitted to JUQUEEN.
*
* system_bg:   A wrapper to implement some of the shell commands in CNK system, for systems other than CNY, ::system(cmd_line) is returned.
*
* Revision 1.4  2016/06/01 10:02:35  choutko
* ppc64 phase2
*
* Revision 1.1.1.1  1996/02/15 17:49:18  mclareni
* Kernlib
*
*
*
* wordsize.inc
*
*    # of bits/word
*    # of characters/word



      PARAMETER   (NBITPW=32)
      PARAMETER   (NCHAPW=4)
      CHARACTER    CHWORD*(NCHAPW)
      INTEGER      IWORD

      EQUIVALENCE (IWORD,CHWORD)


      IF   (NCH)             91, 29, 11
   11 IF (NPW.LE.0)          GO TO 91
      IF (NPW.EQ.1)          GO TO 21
      IF (NPW.LT.NCHAPW)     GO TO 31

C--------          NPW = maximum

      JT     = 0
      NWS    = NCH / NCHAPW
      NTRAIL = NCH - NWS*NCHAPW

      DO 14  JS=1,NWS
      IWORD  = MS(JS)



      MT(JT+1:JT+NCHAPW) = CHWORD

   14 JT  = JT + NCHAPW

      IF (NTRAIL.EQ.0)       RETURN

      IWORD = MS(NWS+1)



      MT(JT+1:JT+NTRAIL) = CHWORD(1:NTRAIL)
      RETURN

C--------          NPW = 1
C--                equivalent to 'CALL UH1TOC(MS,MT,NCH)'

   21 DO 24  JS=1,NCH
      IWORD  = MS(JS)



      MT(JS:JS) = CHWORD(1:1)
   24 CONTINUE
   29 RETURN

C--------          NPW = 2 ...

   31 JT     = 0
      NWS    = NCH / NPW
      NTRAIL = NCH - NWS*NPW

      DO 34  JS=1,NWS
      IWORD  = MS(JS)



      MT(JT+1:JT+NPW) = CHWORD(1:NPW)
   34 JT  = JT + NPW

      IF (NTRAIL.EQ.0)       RETURN

      IWORD = MS(NWS+1)



      MT(JT+1:JT+NTRAIL) = CHWORD(1:NTRAIL)
      RETURN

   91 CALL ABEND
      END

#endif
         subroutine lopen(lun,fname)
           character*(*) fname
           open(unit=lun,file=fname,status='old')
           end
           subroutine ltest(lun)
              read(lun,*)bb
              write(*,*)bb
           end
           subroutine lclose(lun)
              close(unit=lun,status='delete')
            end


#ifdef __PPC64
*CMZ :  3.21/02 29/03/94  15.41.19  by  S.Giani
*-- Author :
      SUBROUTINE GFPART(IPART,NAPART,ITRTYP,AMASS,CHARGE,TLIFE
     +          ,UBUF,NWBUF)
C.
C.    ******************************************************************
C.    *                                                                *
C.    *       Return parameters for particle of type IPART             *
C.    *                                                                *
C.    *    ==>Called by : <USER>, GTREVE                               *
C.    *       Author    R.Brun  *********                              *
C.    *                                                                *
C.    ******************************************************************
C.

*
* $Id: sgif.F,v 1.7 2018/01/05 15:20:42 choutko Exp $
*
* $Log: sgif.F,v $
* Revision 1.7  2018/01/05 15:20:42  choutko
* *** empty log message ***
*
* Revision 1.6  2016/06/09 14:31:52  choutko
* *** empty log message ***
*
* Revision 1.1.1.1  1995/10/24 10:20:31  cernlib
* Geant
*
*


*
*
* gcbank.inc
*




      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)

*
* $Id: sgif.F,v 1.7 2018/01/05 15:20:42 choutko Exp $
*
* $Log: sgif.F,v $
* Revision 1.7  2018/01/05 15:20:42  choutko
* *** empty log message ***
*
* Revision 1.6  2016/06/09 14:31:52  choutko
* *** empty log message ***
*
* Revision 1.1.1.1  1995/10/24 10:20:31  cernlib
* Geant
*
*


*
*
* gclink.inc
*




      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C

*
* gclink.inc
*








*
* gcbank.inc
*







*
* $Id: sgif.F,v 1.7 2018/01/05 15:20:42 choutko Exp $
*
* $Log: sgif.F,v $
* Revision 1.7  2018/01/05 15:20:42  choutko
* *** empty log message ***
*
* Revision 1.6  2016/06/09 14:31:52  choutko
* *** empty log message ***
*
* Revision 1.1.1.1  1995/10/24 10:20:32  cernlib
* Geant
*
*


*
*
* gcnum.inc
*

      COMMON/GCNUM/NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT
      COMMON /GCNUMX/ NALIVE,NTMSTO
C




*
* gcnum.inc
*







      CHARACTER*20 NAPART
      DIMENSION UBUF(10)
C.
C.    ------------------------------------------------------------------
C.
      ITRTYP=0
      IF (JPART.LE.0)GO TO 99
      IF (IPART.LE.0)GO TO 99
      IF (IPART.GT.NPART)GO TO 99
      JPA = LQ(JPART- IPART)
      IF (JPA.LE.0)GO TO 99
C
      CALL UHTOC(IQ(JPA+1),4,NAPART,20)
      ITRTYP = Q(JPA + 6)
      AMASS = Q(JPA + 7)
      CHARGE = Q(JPA + 8)
      TLIFE = Q(JPA + 9)
      NWBUF = IQ(JPA-1) - 9
      IF(NWBUF.LE.0) GO TO 99
      DO 20 I=1,NWBUF
  20  UBUF(I)=Q(JPA+9+I)
C
  99  RETURN
      END
#endif
#ifdef __PPC64
#ifndef G4MULTITHREADED
       subroutine abend
       end      
#endif
#endif

