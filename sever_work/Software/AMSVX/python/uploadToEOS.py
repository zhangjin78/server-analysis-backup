#!/usr/bin/env python2.4
#
import sys, os
sys.path.insert(0,os.path.dirname(os.path.realpath(__file__)) + '/lib')
import RemoteClient
verbose=1
ftype=''
path=''
overwrite=0
for x in sys.argv:
    if x == "-v": verbose=1
    elif x == "-overwrite": overwrite=1
    elif x[0:6] =="-ftype" :
        ftype=x[6:len(x)]
    elif x[0:2] == "-d" :
        path=x[2:len(x)]
if(len(path)<2):
    print "dataset  not defined, exiting "
    sys.exit()
html= RemoteClient.RemoteClient()
html.ConnectDB(1)
#donly :  only delete data, not dataruns
html.UploadToEOS(path,ftype,verbose,overwrite)


