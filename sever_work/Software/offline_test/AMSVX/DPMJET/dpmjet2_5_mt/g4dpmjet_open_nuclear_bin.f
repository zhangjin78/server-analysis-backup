      subroutine g4dpmjet_open_nuclear_bin (namelen, unit,
     + opened, filename)
      
      character*(*) filename
      integer       namelen
      integer       unit
      logical       opened
C
C ------------------------------------------------------------------------------
C
      opened = .TRUE.
c       write(*,*)'trying to open ',filename,filename(1:namelen),
c     + namelen
C      write (6,'(A)') filename(1:namelen)
C      close (6)
      open (unit=unit, file=filename(1:namelen), status="OLD", 
     + form="FORMATTED", err=1010)
c      write(*,*)' opened ',filename,filename(1:namelen),
c     + namelen

      return
      
 1010 opened = .FALSE.
      return
      
      end
      
