*  $Id: timel.F,v 1.5 2018/01/05 15:20:42 choutko Exp $
      subroutine timeldum(ss)
      data ss0/2.e6/
      call timex(ss1)
      ss=ss0-ss1
      end
