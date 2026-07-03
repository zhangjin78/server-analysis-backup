//  $Id: link.C,v 1.10 2016/04/21 07:38:41 choutko Exp $
// Author V. Choutko 24-may-1996
 
#include "link.h"

integer AMSlink::testlast( ){
 if ( _next==0 || (*this < *_next) || (*_next < *this))return 1;
 else return 0;
}

integer AMSlink::operator < ( AMSlink & o) const{
  return &o!=this?1:0;     // Dummy operator
}
