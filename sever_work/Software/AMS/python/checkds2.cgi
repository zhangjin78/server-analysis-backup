#!/usr/bin/env python2.4
#  $Id: checkds2.cgi,v 1.3 2021/06/23 10:17:31 bshan Exp $
# this script suppose to check datasets
#
import sys, os
import cgi
import cgitb; cgitb.enable()
sys.path.insert(0,os.path.dirname(os.path.realpath(__file__)) + '/lib')
import RemoteClient
f=0
v=1
i=0
run2p=0
h=0
tabulated=0
runmin=1305853512
runmax=2000000000
tagmin=0
tagmax=65535
source="" 
for x in sys.argv:
    if x == "-h": h=1
    elif x == "-v": v=1
    elif x == "-f": f=1
    elif x == "-ff": f=2
    elif x[0:2] == "-r" :
        run2p=int(x[2:len(x)])
    elif x[0:2] == "-d" :
        source=(x[2:len(x)])
    elif x[0:3] == "-mi" :
        runmin=(x[3:len(x)])
    elif x[0:3] == "-ma" :
        runmax=(x[3:len(x)])
    elif x[0:7] == "-tagmin" :
        tagmin=(x[7:len(x)])
    elif x[0:7] == "-tagmax" :
        tagmax=(x[7:len(x)])
html= RemoteClient.RemoteClient()
html.ConnectDB(1)
html.CheckDataSet(run2p,source,v,f,tabulated,runmin,runmax,tagmin,tagmax)


