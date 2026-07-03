#!/usr/bin/env python2.4

import sys, os
sys.path.insert(0,os.path.dirname(os.path.realpath(__file__)) + '/lib')
import RemoteClient

v=1
i=0
d=1
h=0
mt=0
tapeonly=1
force=0
s=0
m=0
eos=0
skipcrc=0

for x in sys.argv:
    if x == "-h": h=1
    elif x == "-i": i=1
    elif x == "-v": v=1
    elif x == "-d": d=1
    elif x == "-tapeonly": tapeonly=1
    elif x[0:3] == "-mt":
        mt=1
        if(len(x)>3):
            mt=int(x[3:len(x)])
    elif x == "-s": s=1
    elif x == "-m": m=1
    elif x == "-eos": eos=1
    elif x == "-force": force=1
    elif x == "-skipcrc": skipcrc=1
html= RemoteClient.RemoteClient()
html.ConnectDB(1)
if(html.ServerConnect(3)):
    html.parseJournalFiles(d,i,v,h,s,m,mt,tapeonly,eos,force,skipcrc)
