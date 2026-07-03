      subroutine dpmjet_openinp (namelen, opened, filename)

      character*(*) filename
      integer       namelen
      logical       opened

      COMMON /DTFLKA/ LINP,LOUT,LDAT, LPRI
C ------------------------------------------------------------------------------
      LINP = 10
      opened = .TRUE.
      open (LINP, file=filename(1:namelen), status="OLD",
     & form="FORMATTED", err=1010)

      return

 1010 opened = .FALSE.
      return

      end
