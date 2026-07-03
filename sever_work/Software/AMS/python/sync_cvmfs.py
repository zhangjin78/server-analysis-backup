#!/usr/bin/env python2.4
import sys, os

#sys.path.insert(0,os.environ['Offline'] + '/vdev/python/lib')
sys.path.insert(0,'./python/lib/')
import SyncCvmfsScript

sync_cvmfs = SyncCvmfsScript.SyncCvmfsScript()
sync_cvmfs.run()
