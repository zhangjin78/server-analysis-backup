#!/usr/bin/env python2.4
#  $Id: revalidate.py,v 1.1 2019/06/13 11:53:06 bshan Exp $
# this script suppose to re-validate datasets
#
import sys, os
sys.path.insert(0,os.environ['AMSDataDir'] + '/DataManagement/python/lib')
sys.path.insert(0,'./lib')
import RemoteClient
f=0
v=1
i=0
u=1
b=0
run2p=""
run2pm=0
h=0
mt=0
source=""
donly=0
datamc=1
for x in sys.argv:
    if x == "-h": h=1
    elif x == "-v": v=1
    elif x == "-M": datamc=0
    elif x == "-f": f=1
    elif x == "-u": u=1
    elif x == "-o": donly=1
    elif x[0:2] == "-r" :
        run2p=(x[2:len(x)])
    elif x[0:2] == "-R" :
        run2pm=int(x[2:len(x)])
    elif x[0:2] == "-b" :
        b=int(x[2:len(x)])
    elif x[0:7] =="-datamc" :
        datamc=int(x[7:len(x)])
    elif x[0:3] == "-mt" :
        mt=int(x[3:len(x)])
    elif x[0:2] == "-d" :
        source=x[2:len(x)]
if(len(source)<2):
    print "dataset  not defined, exiting "
    sys.exit()
html= RemoteClient.RemoteClient()
html.ConnectDB(1)
html.RevalidateDataSet(run2p,source,u,v,f,donly,datamc,b,mt)
