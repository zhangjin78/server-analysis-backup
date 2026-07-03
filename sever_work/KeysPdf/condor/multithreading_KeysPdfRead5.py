#!/usr/bin/python3
import time
import subprocess
import sys
import os

####################################################
def run_proc(WorkDir, mycc, codefile, ifile, odir, charge, qver, NVar, XY, layer, span, output, error):
	proc = subprocess.Popen(["/home/ams/yachen/analisi/KeysPdf/condor/protest_KeysPdfRead5.sh", WorkDir, mycc, codefile, ifile, odir, charge, qver, NVar, XY, layer, span], stdout=output, stderr=error)
	return proc

#######################################################

if __name__ == "__main__":
	start = time.time()
	procs = []
	
	WorkDir = sys.argv[1] #directory contains codes and executable
	mycc = sys.argv[2] #executable name
	codefile = sys.argv[3] #list of file to be copied
	JobFst = int(sys.argv[4])
	TotLst = int(sys.argv[5])
	odir = sys.argv[6] #dir for output root files, will be given to  main_run_analysis6.C par-2
	charge = sys.argv[7] #charge to study
	qver = sys.argv[8] #charge version (YJQ: 1)
	NVar = sys.argv[9] #no. template to be fitted
	XY = sys.argv[10] #X, Y or XY charge
	layer = sys.argv[11] #L2Q template or L1Q Template
	span = sys.argv[12] #span
	ncore = int(sys.argv[13]) #no. core used, which is also the no. subprocess per job
	
	JobLast = JobFst + ncore   # for one HTJob, the used list from JobFst to JobLast (in general 32 lists)
	if JobLast > TotLst:
		print("Job Last greater than Total list nb, reset it to total list nb!")
		JobLast = TotLst
	
	#if len(list0) != TotLst:
	#	print("Fatal err!!! Nb of input files not correct!!!")
	#	print("runlist dir: {}".format(ListDir))
	#	print("TotLst={}".format(TotLst))
	#	print("no. file in runlist dir={}".format(len(list0)))
	#	exit(0)
	
	for i in range(JobFst , JobLast):
		output=open(odir + "/output/" + str(i) + ".out", "w")
		error=open(odir + "/error/" + str(i) + ".err", "w")
		proc = run_proc(WorkDir, mycc, codefile, str(i), odir, charge, qver, NVar, XY, layer, span, output, error)
		procs.append(proc)
	 
	for proc in procs:
		proc.communicate()                      
		end = time.time()
		print ('Finished in %.3F' % (end - start))

