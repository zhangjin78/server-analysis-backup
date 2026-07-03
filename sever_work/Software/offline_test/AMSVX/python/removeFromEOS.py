#!/usr/bin/env python2.4
#
import sys, os
#sys.path.insert(0,'./lib')
sys.path.insert(0,os.environ['Offline'] + '/vdev/python/lib')
#sys.path.insert(0,os.environ['AMSDataDir'] + '/DataManagement/python/lib')
import RemoteClient
force=0
verbose=1
irm=0
update=1
ftype=''
path=''
notverify=0
removeempty=0
update_castor_time=0
copy_castor=0
for x in sys.argv:
    if x == "-v": verbose=1
    elif x == "-f": force=1
    elif x == "-u": update=1
    elif x == "-i": irm=1
    elif x == "-notverify": notverify=1
    elif x == "-removeempty": removeempty=1
    elif x == "-updatecastortime": update_castor_time=1
    elif x == "-copycastor": copy_castor=1
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
html.RemoveFromEOS(path,ftype,verbose,update,irm,notverify,removeempty,update_castor_time,copy_castor,force)


